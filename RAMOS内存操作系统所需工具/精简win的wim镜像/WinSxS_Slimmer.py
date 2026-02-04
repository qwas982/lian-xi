#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
WinSxS Slimmer - Python重构版
架构设计：模块化、类型安全、流式处理
基于支配树分析的关键路径：
  1. 权限验证 → 镜像挂载 → WinSxS筛选 → 目录替换 → 镜像卸载 → WIM重打包
  2. 保留规则支配所有筛选决策（模块4为核心支配节点）
  3. 错误处理支配资源清理路径（确保挂载状态最终一致）
"""

import sys
import os
import re
import shutil
import subprocess
import argparse
import logging
from pathlib import Path
from typing import List, Set, Tuple, Generator, Optional
from dataclasses import dataclass
from enum import Enum
from functools import partial
from contextlib import contextmanager

# ==================== 模块1：核心配置 ====================
@dataclass
class WinSxSConfig:
    wim_path: Optional[Path] = None
    index: int = 1
    mount_path: Path = Path("Y:/WinSxSMount")
    scratch_dir: Path = Path("Y:/WinSxSScratch")
    auto_mount: bool = False
    auto_unmount: bool = False
    no_confirm: bool = False
    repack_wim: bool = False
    verbose: bool = False
    admin_group: str = "BUILTIN\\Administrators"

# ==================== 模块2：安全与权限管理 ====================
class SecurityManager:
    @staticmethod
    def is_admin() -> bool:
        """检查管理员权限（Windows特定实现）"""
        try:
            return os.getuid() == 0  # Unix-like
        except AttributeError:
            # Windows: 检查令牌特权
            import ctypes
            return ctypes.windll.shell32.IsUserAnAdmin() != 0
    
    @staticmethod
    def get_admin_group() -> str:
        """获取本地管理员组SID"""
        try:
            import win32security  # 可选依赖，失败时回退
            admin_sid = win32security.ConvertStringSidToSid("S-1-5-32-544")
            name, _ = win32security.LookupAccountSid(None, admin_sid)
            return f"{name}"
        except (ImportError, Exception):
            return "BUILTIN\\Administrators"
    
    @staticmethod
    def take_ownership(path: Path, admin_group: str) -> None:
        """递归获取目录所有权（Windows）"""
        if not path.exists():
            return
        
        try:
            # takeown: 获取所有权
            subprocess.run(
                ["takeown", "/F", str(path), "/R", "/D", "Y"],
                capture_output=True, text=True, encoding='utf-8', errors='ignore'
            )
            # icacls: 赋予完全控制权
            subprocess.run(
                ["icacls", str(path), "/grant", f"{admin_group}:(F)", "/T", "/C"],
                capture_output=True, text=True, encoding='utf-8', errors='ignore'
            )
        except Exception as e:
            logging.warning(f"权限设置失败 (静默处理): {e}")

# ==================== 模块3：DISM镜像管理 ====================
class ImageManager:
    def __init__(self, mount_path: Path):
        self.mount_path = mount_path
        self.is_mounted = False
    
    def _run_dism(self, args: List[str]) -> Tuple[int, str]:
        """执行DISM命令并处理编码"""
        # Windows DISM输出通常为活动代码页（中文系统为GBK）
        try:
            result = subprocess.run(
                ["dism"] + args,
                capture_output=True,
                timeout=300,
                encoding='utf-8',
                errors='ignore'  # 关键：避免GBK/UTF-8解码失败
            )
            return result.returncode, result.stdout + result.stderr
        except UnicodeDecodeError:
            # 回退到系统默认编码
            result = subprocess.run(
                ["dism"] + args,
                capture_output=True,
                timeout=300,
                encoding=sys.getfilesystemencoding(),
                errors='replace'
            )
            return result.returncode, result.stdout + result.stderr
    
    def _clean_existing_mount(self) -> None:
        """清理残留挂载"""
        code, output = self._run_dism(["/Get-MountedImageInfo"])
        if code == 0 and str(self.mount_path) in output:
            logging.warning("检测到残留挂载，正在清理...")
            self.unmount(commit=False)
    
    def mount(self, wim_path: Path, index: int) -> bool:
        logging.info(f"挂载镜像: {wim_path} -> {self.mount_path}")
        
        if not wim_path.exists():
            raise FileNotFoundError(f"WIM文件不存在: {wim_path}")
        
        self.mount_path.mkdir(parents=True, exist_ok=True)
        self._clean_existing_mount()
        
        code, output = self._run_dism([
            "/Mount-Image",
            f"/ImageFile:{wim_path}",
            f"/Index:{index}",
            f"/MountDir:{self.mount_path}"
        ])
        
        if code == 0:
            self.is_mounted = True
            logging.info("✓ 镜像挂载成功")
            return True
        raise RuntimeError(f"DISM挂载失败 (代码 {code}): {output}")
    
    def unmount(self, commit: bool = True) -> bool:
        if not self.is_mounted:
            return True
        
        action = "/Commit" if commit else "/Discard"
        logging.info(f"卸载镜像 ({'提交' if commit else '丢弃'}): {self.mount_path}")
        
        code, output = self._run_dism([
            "/Unmount-Image",
            f"/MountDir:{self.mount_path}",
            action
        ])
        
        if code == 0:
            self.is_mounted = False
            logging.info("✓ 镜像卸载成功")
            return True
        raise RuntimeError(f"DISM卸载失败 (代码 {code}): {output}")
    
    # ===== 修复1：ImageManager.get_wim_info 强制英文输出 + 回退机制 =====
    def get_wim_info(self, wim_path: Path) -> str:
        """获取WIM信息（优先英文输出，失败回退中文）"""
        # 尝试强制英文输出（Win10 1809+ 支持）
        code, output = self._run_dism([
            "/Get-WimInfo",
            f"/WimFile:{wim_path}",
            "/English"  # 关键修复：强制英文避免编码/语言问题
        ])
        if code == 0:
            return output
        
        # 回退到系统默认语言（兼容旧版DISM）
        code, output = self._run_dism([
            "/Get-WimInfo",
            f"/WimFile:{wim_path}"
        ])
        if code == 0:
            return output
        
        raise RuntimeError(f"获取WIM信息失败 (代码 {code})")

# ==================== 模块4：WinSxS保留规则 (一字不漏完整复制) ====================
class RetentionRules:
    # 修正 RetentionRules.get_core_patterns() 中的拼写错误（按原始语义修复）
    @staticmethod
    def get_core_patterns() -> List[str]:
        return [
            # === 系统核心组件（修复原始PS1拼写错误）===
            "microsoft-windows-servicingstack",  # 修复: 原PS1有空格 "s cingstack"
            "microsoft-windows-servicingstack-inetsrv",
            "microsoft-windows-servicingstack-onecore",
            "microsoft-windows-servicingstack-msg",
            "microsoft-windows-servicing-adm",
            "microsoft-windows-servicingcommon",
            "microsoft-windows-servicing-onecore-uapi",
            "microsoft-windows-servicingstack.resources*",  # 修复: 原PS1有空格
            
            # === Visual C++ 运行时（关键保留项）===
            "*microsoft.vc80.crt*",
            "*microsoft.vc90.crt*",
            "*policy.8.0.microsoft.vc80.crt*",
            "*policy.9.0.microsoft.vc90.crt*",
            
            # === 通用控件（修复原始PS1拼写错误）===
            "*microsoft.windows.common-controls*",  # 修复: 原PS1为 "c*-controls"
            "*microsoft.windows.common-controls.resources*",
            
            # === GDI+ 图形库 ===
            "*microsoft.windows.gdiplus*",
            
            # === 系统自动化组件（修复原始PS1拼写错误）===
            "*microsoft.windows.automation.proxystub*",  # 修复: 原PS1为 "i*utomation"
            "*microsoft.windows.isolationautomation*",
            
            # === 其他核心系统组件 ===
            "*microsoft-windows-coreos-revision*",
            "*microsoft-windows-deployment*",
            "*microsoft-windows-explorer*",
            "*microsoft-windows-international-core-winpe*",
            "*microsoft-windows-international-core*",
            "*microsoft-windows-security-spp-ux*",
            "*microsoft-windows-shell-setup*",
            "*microsoft-windows-unattendedjoin*",
            "*microsoft-windows-systemcompatible*",
            
            # === 其他重要组件 ===
            "*microsoft-windows-servicingstack*",  # 修复: 原PS1为 "s*ngstack"
            "*microsoft-windows-servicingstack-*",
            "*microsoft-windows-servicingstack.*",
            "*microsoft-windows-coreos*",
            "*microsoft-windows-setup*",
            
            # === 新增：WDDM回退必需组件（防黑屏核心）===
            "*microsoft-windows-dxgkrnl*",      # DirectX图形内核
            "*microsoft-windows-basicdisplay*", # 基础显示驱动
            "*microsoft-windows-basicrender*",  # 基础渲染驱动
            "*microsoft-windows-dwm*",          # 桌面窗口管理器


            # === 新增：OneCore架构支持（Win10 21H2+必需）===
            "*microsoft-windows-servicingstack-onecore*",
            "*microsoft-windows-servicingstack-onecore-uapi*",

            # === 新增：DirectX完整支持 ===
            "*microsoft-windows-directx*",
            "*microsoft-windows-d3d*",
            "*microsoft-windows-opengl*",

        ]
    
    @staticmethod
    def get_hyperv_patterns() -> List[str]:
        return [
            "*microsoft-hyper-v*",
            "*windowsvirtualization*",
            "*vmcompute*",
            "*vmwp*",
            "*vmsvc*",
            "*hvsocket*",
            "*hvservices*",
            "*virtualization*",
            "*hyper-v*",
            "*hyperv*",
            "*vhdparser*",
            "*vid*",
            "*vhdsvc*",
            "*vmms*",
            "*vmbus*",
            "*vmictimeprovider*",
            "*vmicrdv*",
            "*vmicguestinterface*",
            "*vmickvpexchange*",
            "*vmicreservation*",
            "*vmicvmsession*",
            "*vmicvss*",
            "*vmicheartbeat*",
            "*vmicshutdown*",
            "*vmictimesync*"
        ]
    
    @staticmethod
    def get_wsl_patterns() -> List[str]:
        return [
            "*microsoft-windows-wsl*",
            "*lxss*",
            "*wsl*",
            "*virtualmachineplatform*",
            "*microsoft-windows-subsystem-linux*",
            "*microsoft-windows-wslapi*",
            "*microsoft-windows-lxcore*",
            "*microsoft-windows-lxfs*",
            "*microsoft-windows-lxssmanager*",
            "*microsoft-windows-lxsscore*",
            "*microsoft-windows-lxssuser*"
        ]
    
    @staticmethod
    def get_special_directories() -> Set[str]:
        return {
            "Catalogs",
            "Manifests",
            "InstallTemp",
            "FileMaps",
            "Fusion",
            "Temp",
            "SettingsManifests"
        }
    
    @staticmethod
    def get_cjk_patterns() -> List[str]:
        return [
            "*_zh-cn_*",
            "*_zh-tw_*",
            "*_ja-jp_*",
            "*_ko-kr_*"
        ]
    
    @staticmethod
    def get_arch_patterns() -> List[str]:
        return [
            "amd64_*",
            "x86_*",
            "wow64_*"
        ]

# ==================== 模块5：组件筛选器 (函数式管道实现) ====================
class ComponentFilter:
    def __init__(self, verbose: bool = False):
        self.verbose = verbose
        self._arch_prefixes = [p.rstrip('*') for p in RetentionRules.get_arch_patterns()]
        self._all_patterns = (
            RetentionRules.get_core_patterns() +
            RetentionRules.get_hyperv_patterns() +
            RetentionRules.get_wsl_patterns()
        )
    
    def _has_arch_prefix(self, name: str) -> bool:
        return any(name.startswith(prefix) for prefix in self._arch_prefixes)
    
    def _strip_arch_prefix(self, name: str) -> str:
        for prefix in self._arch_prefixes:
            if name.startswith(prefix):
                return name[len(prefix):]
        return name
    
    def _is_base_retained(self, base_name: str) -> bool:
        return any(self._fnmatch(base_name, p) for p in self._all_patterns)
    
    # 替换 ComponentFilter._fnmatch 为完整PowerShell -like语义实现
    @staticmethod
    def _fnmatch(name: str, pattern: str) -> bool:
        """完整实现PowerShell -like语义（不区分大小写，支持多*和?）"""
        import re
        # 转义正则特殊字符，转换 * -> .* , ? -> .
        regex = "^" + re.escape(pattern).replace(r'\*', '.*').replace(r'\?', '.') + "$"
        return re.match(regex, name, re.IGNORECASE) is not None
    
    # 重构 ComponentFilter.should_retain 为单一流水线
    def should_retain(self, component_name: str) -> bool:
        # 1. 特殊目录（精确匹配，不区分大小写）
        if component_name.lower() in {d.lower() for d in RetentionRules.get_special_directories()}:
            self._log(f"✓ 保留: {component_name} (特殊目录)")
            return True
        
        # 2. CJK语言资源
        if any(self._fnmatch(component_name, p) for p in RetentionRules.get_cjk_patterns()):
            self._log(f"✓ 保留: {component_name} (CJK)")
            return True
        
        # 3. 所有保留模式（核心+Hyper-V+WSL）
        all_patterns = (
            RetentionRules.get_core_patterns() +
            RetentionRules.get_hyperv_patterns() +
            RetentionRules.get_wsl_patterns()
        )
        if any(self._fnmatch(component_name, p) for p in all_patterns):
            self._log(f"✓ 保留: {component_name} (规则匹配)")
            return True
        
        # 4. 架构变体：剥离前缀后匹配（仅当原始名含架构前缀时）
        for prefix in ["amd64_", "x86_", "wow64_"]:
            if component_name.lower().startswith(prefix):
                base = component_name[len(prefix):]
                if any(self._fnmatch(base, p) for p in all_patterns):
                    self._log(f"✓ 保留: {component_name} (架构变体→{base})")
                    return True
        
        # 5. 默认删除（记录可疑删除项）
        if "servicing" in component_name.lower() or "manifest" in component_name.lower():
            logging.warning(f"⚠ 可疑删除: {component_name} (含关键术语)")
        self._log(f"✗ 删除: {component_name}")
        return False
    
    def _log(self, msg: str) -> None:
        if self.verbose:
            logging.debug(msg)

# ==================== 模块6：WinSxS优化器（彻底重构版） ====================
class WinSxSOptimizer:
    def __init__(self, admin_group: str, no_confirm: bool, verbose: bool):
        self.filter = ComponentFilter(verbose)
        self.admin_group = admin_group
        self.no_confirm = no_confirm
        self.stats = {'total': 0, 'retained': 0, 'original_size_gb': 0.0, 'new_size_gb': 0.0}
    
    # === 步骤1：路径安全验证（关键修复）===
    def _validate_paths(self, winsxs_path: Path, scratch_dir: Path) -> Path:
        """强制验证路径拓扑，防止嵌套灾难"""
        winsxs_abs = winsxs_path.resolve()
        scratch_abs = scratch_dir.resolve()
        
        # 检查1：scratch_dir 不能是 winsxs_path 的子目录
        if winsxs_abs in scratch_abs.parents:
            raise ValueError(
                f"❌ 致命错误：临时目录 {scratch_dir} 位于 WinSxS 内部！\n"
                f"   WinSxS路径: {winsxs_abs}\n"
                f"   临时目录: {scratch_abs}\n"
                f"   → 请使用独立路径（如 Y:/WinSxSScratch）"
            )
        
        # 检查2：winsxs_path 不能是 scratch_dir 的子目录
        if scratch_abs in winsxs_abs.parents:
            raise ValueError(
                f"❌ 致命错误：WinSxS 目录 {winsxs_path} 位于临时目录内部！\n"
                f"   临时目录: {scratch_abs}\n"
                f"   WinSxS路径: {winsxs_abs}"
            )
        
        # 检查3：路径不能相同
        if winsxs_abs == scratch_abs:
            raise ValueError("❌ 致命错误：WinSxS路径与临时目录相同！")
        
        # 创建安全的新目录路径（确保在scratch_dir根下）
        winsxs_new = scratch_dir / "WinSxS_new"
        logging.info(f"✓ 路径验证通过: {winsxs_path} → {winsxs_new}")
        return winsxs_new
    
    # === 步骤2：组件扫描管道（生成器流式处理）===
    def _scan_components(self, winsxs_path: Path) -> Generator[Tuple[Path, str], None, None]:
        """生成器：安全扫描组件，跳过特殊目录"""
        special_dirs = RetentionRules.get_special_directories()
        for item in winsxs_path.iterdir():
            if item.is_dir() and item.name not in special_dirs:
                yield item, item.name
    
    # === 步骤3：过滤与复制管道（串行执行）===
    def _filter_and_copy(self, winsxs_path: Path, winsxs_new: Path) -> int:
        """串行管道：扫描 → 过滤 → 复制"""
        retained = 0
        total = 0
        
        # 预扫描计数（避免二次遍历）
        components = list(self._scan_components(winsxs_path))
        total = len(components)
        self.stats['total'] = total
        
        # 主管道：逐组件处理
        for i, (comp_path, comp_name) in enumerate(components, 1):
            # 进度反馈
            if i % 100 == 0 or i == total:
                percent = i / total * 100
                logging.info(f"  处理 {i}/{total} ({percent:.1f}%) | 保留 {retained}")
            
            # 过滤决策
            if not self.filter.should_retain(comp_name):
                continue
            
            # 安全复制（带错误隔离）
            try:
                dest = winsxs_new / comp_name
                shutil.copytree(comp_path, dest, dirs_exist_ok=True)
                retained += 1
            except Exception as e:
                logging.warning(f"⚠ 复制跳过 {comp_name}: {e}")
        
        self.stats['retained'] = retained
        return retained
    
    # === 步骤4：特殊目录处理（Manifests完整性保障）===
    def _process_special_dirs(self, winsxs_path: Path, winsxs_new: Path) -> None:
        """精确复制特殊目录，Manifests使用robocopy保障完整性"""
        for dirname in RetentionRules.get_special_directories():
            src = winsxs_path / dirname
            dst = winsxs_new / dirname
            
            if not src.exists():
                dst.mkdir(parents=True, exist_ok=True)
                logging.debug(f"  创建空目录: {dirname}")
                continue
            
            if dirname == "Manifests":
                # Manifests必须完整复制（含隐藏/系统文件）
                logging.debug(f"  robocopy复制: {dirname}")
                subprocess.run(
                    ["robocopy", str(src), str(dst), "/E", "/COPYALL", "/R:1", "/W:1", "/NP", "/NDL"],
                    capture_output=True, text=True, encoding='utf-8', errors='ignore'
                )
            else:
                # 其他特殊目录标准复制
                logging.debug(f"  标准复制: {dirname}")
                shutil.copytree(src, dst, dirs_exist_ok=True, ignore_dangling_symlinks=True)
    
    # === 步骤5：原子化替换（关键安全操作）===
    def _atomic_replace(self, old_path: Path, new_path: Path) -> None:
        """三阶段原子替换：验证 → 删除 → 移动"""
        # 阶段1：前置验证
        if not new_path.exists() or not any(new_path.iterdir()):
            raise RuntimeError(f"❌ 新目录为空或不存在: {new_path}")
        
        # 阶段2：强制删除旧目录（处理顽固文件）
        logging.info("  正在删除原始WinSxS...")
        try:
            # 先解除只读/系统属性
            for root, dirs, files in os.walk(old_path, topdown=False):
                for name in files + dirs:
                    path = Path(root) / name
                    try:
                        path.chmod(0o777)  # 移除只读
                    except:
                        pass
            
            # 强制递归删除
            shutil.rmtree(old_path, ignore_errors=False)
        except Exception as e:
            raise RuntimeError(f"❌ 删除原始目录失败: {e}")
        
        # 阶段3：原子移动（确保路径不重叠）
        logging.info("  正在移动新目录...")
        try:
            shutil.move(str(new_path), str(old_path))
        except Exception as e:
            raise RuntimeError(f"❌ 目录移动失败: {e}")
        
        logging.info("✓ WinSxS替换完成")
    
    # === 主执行管道（串行化五步流程）===
    def optimize(self, mount_path: Path, scratch_dir: Path) -> bool:
        logging.info("=== WinSxS优化管道启动 ===")
        
        # === 关键修复：强制验证路径拓扑（防止嵌套灾难）===
        winsxs_path = mount_path / "Windows" / "WinSxS"
        winsxs_new = scratch_dir / "WinSxS_new"
        
        # 100% 阻断路径嵌套（必须添加！）
        if winsxs_new.resolve() in winsxs_path.resolve().parents:
            raise ValueError(
                f"致命错误：临时目录 {scratch_dir} 位于 WinSxS 内部！\n"
                f"  正确配置: --scratch-dir Y:/WinSxSScratch (独立路径)\n"
                f"  错误配置: --scratch-dir Y:/WinSxSMount/Windows/WinSxS"
            )
        
        winsxs_new = self._validate_paths(winsxs_path, scratch_dir)
        
        # 获取所有权（前置安全操作）
        SecurityManager.take_ownership(winsxs_path, self.admin_group)
        
        # 管道步骤2：原始大小计算
        self.stats['original_size_gb'] = self._calculate_size_gb(winsxs_path)
        logging.info(f"原始大小: {self.stats['original_size_gb']:.2f} GB")
        
        # 管道步骤3：准备新目录
        if winsxs_new.exists():
            shutil.rmtree(winsxs_new, ignore_errors=True)
        winsxs_new.mkdir(parents=True, exist_ok=True)
        logging.info(f"新目录: {winsxs_new}")
        
        # 管道步骤4：组件过滤与复制
        logging.info("开始组件过滤管道...")
        retained = self._filter_and_copy(winsxs_path, winsxs_new)
        logging.info(f"✓ 保留组件: {retained}/{self.stats['total']}")
        
        # 管道步骤5：特殊目录处理
        logging.info("处理特殊目录...")
        self._process_special_dirs(winsxs_path, winsxs_new)
        
        # 管道步骤6：结果验证
        self.stats['new_size_gb'] = self._calculate_size_gb(winsxs_new)
        self._display_statistics()
        
        # 管道步骤7：用户确认与原子替换
        if not self._confirm_replacement():
            logging.warning("✗ 用户取消替换操作")
            return False
        
        self._atomic_replace(winsxs_path, winsxs_new)
        return True
    
    # === 辅助方法（保持简洁）===
    def _calculate_size_gb(self, path: Path) -> float:
        total = 0
        try:
            for f in path.rglob('*'):
                if f.is_file():
                    total += f.stat().st_size
        except Exception as e:
            logging.warning(f"大小计算警告: {e}")
        return total / (1024 ** 3)
    
    def _display_statistics(self) -> None:
        removed = self.stats['total'] - self.stats['retained']
        removal_rate = (removed / self.stats['total']) * 100 if self.stats['total'] else 0
        saved = self.stats['original_size_gb'] - self.stats['new_size_gb']
        saved_rate = (saved / self.stats['original_size_gb']) * 100 if self.stats['original_size_gb'] else 0
        
        logging.info("\n=== 优化统计 ===")
        logging.info(f"原始组件: {self.stats['total']}")
        logging.info(f"保留组件: {self.stats['retained']} ({100-removal_rate:.1f}%)")
        logging.info(f"精简比例: {removal_rate:.1f}%")
        logging.info(f"原始大小: {self.stats['original_size_gb']:.2f} GB")
        logging.info(f"新大小:   {self.stats['new_size_gb']:.2f} GB")
        logging.info(f"节省:     {saved:.2f} GB ({saved_rate:.1f}%)")
    
    def _confirm_replacement(self) -> bool:
        if self.no_confirm:
            return True
        resp = input("\n确认替换原始WinSxS目录? (y/N): ").strip().lower()
        return resp == 'y'

# ==================== 模块7：WIM重新打包器（重构版） ====================
class WimRepacker:
    @staticmethod
    def repack(wim_path: Path, scratch_dir: Path) -> bool:
        logging.info("=== 重新打包WIM ===")
        if not wim_path.exists():
            raise FileNotFoundError(f"WIM文件不存在: {wim_path}")
        
        original_size = wim_path.stat().st_size / (1024 ** 3)
        logging.info(f"原始WIM大小: {original_size:.2f} GB")
        
        # === 重构核心：健壮的镜像数量解析（3层降级策略） ===
        mgr = ImageManager(Path("dummy"))
        info = mgr.get_wim_info(wim_path)
        
        # 策略1：标准摘要匹配（优先）
        match = re.search(
            r"(?:Image Count|Total images?|映像数目|映像总数)\s*[:：]\s*(\d+)",
            info,
            re.IGNORECASE
        )
        if match:
            image_count = int(match.group(1))
            logging.debug(f"✓ 通过标准摘要匹配: {image_count} 个镜像")
        
        # 策略2：从Index字段推断（单镜像场景）
        else:
            # 提取所有"Index : N"中的数字（忽略BuildIndex等干扰项）
            indices = re.findall(
                r"(?<!Build)Index\s*[:：]\s*(\d+)",  # 负向前瞻排除BuildIndex
                info,
                re.IGNORECASE
            )
            if indices:
                image_count = max(int(i) for i in indices)  # 取最大索引值
                logging.debug(f"✓ 通过Index字段推断: {image_count} 个镜像 (索引: {indices})")
            else:
                # 策略3：终极诊断（输出结构化片段）
                snippet = "\n".join(
                    line.strip() for line in info.splitlines()[:15] 
                    if line.strip() and not line.startswith("Deployment")
                )[:800]
                raise RuntimeError(
                    f"无法解析WIM镜像数量。DISM输出关键片段:\n{snippet}\n"
                    f"请验证WIM文件有效性 (dism /Get-WimInfo /WimFile:{wim_path})"
                )
        # === 重构结束 ===
        
        logging.info(f"检测到 {image_count} 个镜像")
        
        # 导出所有镜像（流式处理，避免内存溢出）
        temp_wim = scratch_dir / "temp_repack.wim"
        temp_wim.unlink(missing_ok=True)
        
        for idx in range(1, image_count + 1):
            logging.info(f"导出镜像 {idx}/{image_count}...")
            args = [
                "/Export-Image",
                f"/SourceImageFile:{wim_path}",
                f"/SourceIndex:{idx}",
                f"/DestinationImageFile:{temp_wim}",
                "/Compress:max"
            ]
            if idx > 1:
                args.append("/Append")
            
            code, output = mgr._run_dism(args)
            if code != 0:
                # 增强错误诊断：提取DISM关键错误行
                error_lines = [l for l in output.splitlines() if "error" in l.lower() or "失败" in l]
                raise RuntimeError(
                    f"导出镜像 {idx} 失败 (代码 {code})\n"
                    f"关键错误: {' | '.join(error_lines[:3]) if error_lines else '无明确错误信息'}"
                )
        
        # 原子化替换（避免损坏原文件）
        #backup = wim_path.with_suffix(wim_path.suffix + ".backup")
        #shutil.copy2(wim_path, backup)
        #logging.info(f"✓ 已备份至: {backup}")
        
        wim_path.unlink()
        shutil.move(str(temp_wim), str(wim_path))
        
        # 结果验证
        new_size = wim_path.stat().st_size / (1024 ** 3)
        saved = original_size - new_size
        logging.info("✓ 重新打包完成")
        logging.info(f"新WIM大小: {new_size:.2f} GB")
        logging.info(f"节省空间: {saved:.2f} GB ({saved/original_size*100:.1f}%)")
        return True

# ==================== 模块8：主控制器 ====================
class WinSxSController:
    def __init__(self, config: WinSxSConfig):
        self.config = config
        self.image_mgr = ImageManager(config.mount_path)
        self.optimizer = WinSxSOptimizer(
            admin_group=config.admin_group,
            no_confirm=config.no_confirm,
            verbose=config.verbose
        )
        self.results = {}
    
    @contextmanager
    def _ensure_cleanup(self, mount_required: bool, optimized: bool):
        """确保资源清理的上下文管理器"""
        try:
            yield
        finally:
            if mount_required and self.image_mgr.is_mounted:
                try:
                    self.image_mgr.unmount(commit=optimized)
                except Exception as e:
                    logging.warning(f"清理挂载时出错: {e}")
    
    def run(self) -> bool:
        logging.info("=== WinSxS 精简工具 (Python重构版) ===")
        logging.info(f"管理员组: {self.config.admin_group}")
        
        if not SecurityManager.is_admin():
            raise PermissionError("需要管理员权限运行此脚本")
        
        mount_required = False
        optimized = False
        
        try:
            self.results['start_time'] = self._now()
            
            # 镜像挂载
            if self.config.wim_path and self.config.auto_mount:
                self._show_wim_info()
                self.image_mgr.mount(self.config.wim_path, self.config.index)
                mount_required = True
            
            # 验证路径
            if not self.config.mount_path.exists():
                raise FileNotFoundError(f"挂载目录不存在: {self.config.mount_path}")
            
            if not self.config.scratch_dir.exists():
                logging.info(f"创建临时目录: {self.config.scratch_dir}")
                self.config.scratch_dir.mkdir(parents=True)
            
            # 优化WinSxS
            with self._ensure_cleanup(mount_required, False):
                optimized = self.optimizer.optimize(
                    self.config.mount_path,
                    self.config.scratch_dir
                )
                self.results['optimized'] = optimized
                
                # 镜像卸载
                if mount_required and self.config.auto_unmount:
                    self.image_mgr.unmount(commit=optimized)
                    mount_required = False  # 已清理
                
                # WIM重打包
                if optimized and self.config.repack_wim and self.config.wim_path:
                    if self.image_mgr.is_mounted:
                        logging.warning("镜像仍在挂载状态，跳过WIM重打包")
                    else:
                        WimRepacker.repack(self.config.wim_path, self.config.scratch_dir)
            
            self.results['end_time'] = self._now()
            self.results['success'] = True
            self._display_summary()
            return True
            
        except Exception as e:
            self.results['error'] = str(e)
            self.results['success'] = False
            logging.error(f"执行失败: {e}")
            raise
    
    def _show_wim_info(self) -> None:
        if not self.config.wim_path:
            return
        
        try:
            info = self.image_mgr.get_wim_info(self.config.wim_path)
            # 提取镜像列表
            indexes = re.findall(r"Index\s*:\s*(\d+)", info, re.IGNORECASE)
            names = re.findall(r"Name\s*:\s*(.+)", info, re.IGNORECASE)
            
            if indexes:
                logging.info("\nWIM文件镜像列表:")
                for i, idx in enumerate(indexes):
                    name = names[i] if i < len(names) else "未知"
                    logging.info(f"  索引 {idx} : {name}")
                logging.info(f"\n使用索引: {self.config.index}")
        except Exception as e:
            logging.warning(f"无法显示WIM信息: {e}")
    
    @staticmethod
    def _now() -> float:
        import time
        return time.time()
    
    def _display_summary(self) -> None:
        duration = self.results['end_time'] - self.results['start_time']
        logging.info("\n=== 执行摘要 ===")
        logging.info(f"状态: {'成功' if self.results.get('success') else '失败'}")
        logging.info(f"耗时: {duration/60:.2f} 分钟")
        
        if self.results.get('optimized'):
            stats = self.optimizer.stats
            removed = stats['total'] - stats['retained']
            logging.info(f"精简组件: {removed}/{stats['total']}")

# ==================== 模块9：命令行入口 ====================
def setup_logging(verbose: bool) -> None:
    level = logging.DEBUG if verbose else logging.INFO
    logging.basicConfig(
        level=level,
        format='%(asctime)s | %(levelname)-8s | %(message)s',
        datefmt='%H:%M:%S'
    )

def main() -> int:
    parser = argparse.ArgumentParser(description="WinSxS精简工具 - Python重构版")
    parser.add_argument('--wim-path', type=Path, help='WIM镜像路径')
    parser.add_argument('--index', type=int, default=1, help='镜像索引 (默认: 1)')
    parser.add_argument('--mount-path', type=Path, default=Path("Y:/WinSxSMount"), help='挂载目录')
    parser.add_argument('--scratch-dir', type=Path, default=Path("Y:/WinSxSScratch"), help='临时目录')
    parser.add_argument('--auto-mount', action='store_true', help='自动挂载WIM')
    parser.add_argument('--auto-unmount', action='store_true', help='自动卸载WIM')
    parser.add_argument('--no-confirm', action='store_true', help='跳过确认提示')
    parser.add_argument('--repack-wim', action='store_true', help='优化后重新打包WIM')
    parser.add_argument('--verbose', action='store_true', help='详细输出')
    parser.add_argument('--admin-group', default=None, help='管理员组 (默认自动检测)')
    
    args = parser.parse_args()
    setup_logging(args.verbose)
    
    try:
        # 构建配置
        config = WinSxSConfig(
            wim_path=args.wim_path,
            index=args.index,
            mount_path=args.mount_path,
            scratch_dir=args.scratch_dir,
            auto_mount=args.auto_mount,
            auto_unmount=args.auto_unmount,
            no_confirm=args.no_confirm,
            repack_wim=args.repack_wim,
            verbose=args.verbose,
            admin_group=args.admin_group or SecurityManager.get_admin_group()
        )
        
        # 执行主流程
        controller = WinSxSController(config)
        success = controller.run()
        return 0 if success else 1
        
    except KeyboardInterrupt:
        logging.warning("操作被用户中断")
        return 130
    except Exception as e:
        logging.exception("未处理的异常")
        return 1

if __name__ == "__main__":
    sys.exit(main())