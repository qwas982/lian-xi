# WinSxS Slimmer for Windows 10/11 - 优化的精简逻辑
# 基于nano11builder.ps1逻辑，重新设计以保留必需组件


# 需求: PS7.5+
# 用法:
#.\WinSxS的精简.ps1 `
#  -WimPath "Z:\install.wim" `
#  -Index 3 `
#  -MountPath "Y:\Mount" `
#  -ScratchDir "Y:\Temp" `
#  -AutoMount `
#  -AutoUnmount `
#  -NoConfirm

param(
    [Parameter(Mandatory=$false)]
    [string]$WimPath,  # WIM/ESD文件路径
    
    [Parameter(Mandatory=$false)]
    [int]$Index = 3,   # 镜像索引（默认1）
    
    [Parameter(Mandatory=$false)]
    [string]$MountPath = "Y:\WinSxSMount",  # 挂载目录
    
    [Parameter(Mandatory=$false)]
    [string]$ScratchDir = "Y:\WinSxSScratch",  # 临时工作目录
    
    [Parameter(Mandatory=$false)]
    [switch]$AutoMount,  # 自动挂载模式
    [Parameter(Mandatory=$false)]
    [switch]$AutoUnmount,  # 自动卸载模式
    [Parameter(Mandatory=$false)]
    [switch]$NoConfirm,  # 无需确认直接替换
    [Parameter(Mandatory=$false)]
    [string]$AdminGroup = "BUILTIN\Administrators"  # 管理员组
)

# 获取管理员组信息
function Get-AdminGroup {
    try {
        $adminSID = New-Object System.Security.Principal.SecurityIdentifier("S-1-5-32-544")
        $adminGroupAccount = $adminSID.Translate([System.Security.Principal.NTAccount])
        return $adminGroupAccount.Value
    } catch {
        Write-Warning "无法获取管理员组信息，使用默认值: $AdminGroup"
        return $AdminGroup
    }
}

# 检查是否以管理员身份运行
function Test-Administrator {
    $currentUser = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($currentUser)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

function Get-WinSxSRetentionRules {
    <#
    .SYNOPSIS
    返回需要保留的WinSxS组件匹配规则
    #>
    
    # 核心保留组件模式
    $corePatterns = @(
        # === 系统核心组件 ===
        "*microsoft-windows-servicingstack*",
        "*microsoft-windows-servicingstack-inetsrv*",
        "*microsoft-windows-servicingstack-onecore*",
        "*microsoft-windows-servicingstack-msg*",
        "*microsoft-windows-servicing-adm*",
        "*microsoft-windows-servicingcommon*",
        "*microsoft-windows-servicing-onecore-uapi*",
        "*microsoft-windows-s..cingstack.resources*",
        
        # === Visual C++ 运行时 ===
        "*microsoft.vc80.crt*",
        "*microsoft.vc90.crt*",
        "*policy.8.0.microsoft.vc80.crt*",
        "*policy.9.0.microsoft.vc90.crt*",
        
        # === 通用控件 ===
        "*microsoft.windows.common-controls*",
        "*microsoft.windows.c..-controls.resources*",
        
        # === GDI+ 图形库 ===
        "*microsoft.windows.gdiplus*",
        
        # === 系统自动化组件 ===
        "*microsoft.windows.i..utomation.proxystub*",
        "*microsoft.windows.isolationautomation*",
        
        # === 其他核心系统组件 ===
        "*microsoft-windows-coreos-revision*",
        "*microsoft-windows-deployment*",
        "*microsoft-windows-explorer*",
        "*microsoft-windows-i..national-core-winpe*",
        "*microsoft-windows-international-core*",
        "*microsoft-windows-security-spp-ux*",
        "*microsoft-windows-shell-setup*",
        "*microsoft-windows-unattendedjoin*",
        "*microsoft.windows.systemcompatible*"
    )
    
    # Hyper-V 虚拟化组件
    $hypervPatterns = @(
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
    )
    
    # WSL (Windows Subsystem for Linux) 组件
    $wslPatterns = @(
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
    )
    
    # 特殊保留的元数据目录（必须保留）
    $specialDirectories = @(
        "Catalogs",
        "Manifests",
        "InstallTemp",
        "FileMaps",
        "Fusion",
        "Temp",
        "SettingsManifests"
    )
    
    return @{
        CorePatterns = $corePatterns
        HyperVPatterns = $hypervPatterns
        WSLPatterns = $wslPatterns
        SpecialDirectories = $specialDirectories
    }
}

function Should-RetainComponent {
    <#
    .SYNOPSIS
    判断WinSxS组件是否应该保留
    #>
    param(
        [string]$ComponentName
    )
    
    $rules = Get-WinSxSRetentionRules
    
    # 1. 检查是否是特殊元数据目录
    if ($rules.SpecialDirectories -contains $ComponentName) {
        return $true
    }
    
    # 2. 检查中日韩语言资源
    $cjkSuffixPatterns = @("*_zh-cn_*", "*_zh-tw_*", "*_ja-jp_*", "*_ko-kr_*")
    foreach ($pattern in $cjkSuffixPatterns) {
        if ($ComponentName -like $pattern) {
            return $true
        }
    }
    
    # 3. 检查核心组件模式
    foreach ($pattern in $rules.CorePatterns) {
        if ($ComponentName -like $pattern) {
            return $true
        }
    }
    
    # 4. 检查Hyper-V组件
    foreach ($pattern in $rules.HyperVPatterns) {
        if ($ComponentName -like $pattern) {
            return $true
        }
    }
    
    # 5. 检查WSL组件
    foreach ($pattern in $rules.WSLPatterns) {
        if ($ComponentName -like $pattern) {
            return $true
        }
    }
    
    return $false
}

function Optimize-WinSxS {
    <#
    .SYNOPSIS
    优化WinSxS目录，保留必需组件
    #>
    param(
        [string]$MountPath,
        [string]$ScratchDir,
        [string]$AdminGroup,
        [bool]$NoConfirm = $false
    )
    
    Write-Host "=== WinSxS 目录优化开始 ===" -ForegroundColor Cyan
    Write-Host "挂载目录: $MountPath" -ForegroundColor Yellow
    Write-Host "临时目录: $ScratchDir" -ForegroundColor Yellow
    
    # 获取WinSxS路径
    $winSxSPath = Join-Path -Path $MountPath -ChildPath "Windows\WinSxS"
    $winSxSNew = Join-Path -Path $ScratchDir -ChildPath "WinSxS_new"
    
    # 检查WinSxS目录是否存在
    if (-not (Test-Path $winSxSPath)) {
        Write-Error "WinSxS目录不存在: $winSxSPath"
        return $false
    }
    
    # 获取所有权和权限
    Write-Host "获取WinSxS目录所有权..." -ForegroundColor Yellow
    try {
        & takeown.exe /F $winSxSPath /R /D Y 2>&1 | Out-Null
        & icacls.exe $winSxSPath /grant "${AdminGroup}:(F)" /T /C 2>&1 | Out-Null
    } catch {
        Write-Warning "获取所有权时出错: $_，但继续执行..."
    }
    
    # 创建新的WinSxS目录
    Write-Host "创建新的WinSxS目录..." -ForegroundColor Yellow
    if (Test-Path $winSxSNew) {
        Remove-Item -Path $winSxSNew -Recurse -Force -ErrorAction SilentlyContinue
    }
    New-Item -ItemType Directory -Path $winSxSNew -Force | Out-Null
    
    # 获取所有WinSxS组件
    $allComponents = Get-ChildItem -Path $winSxSPath -Directory
    $totalComponents = $allComponents.Count
    $retainedCount = 0
    
    Write-Host "分析 $totalComponents 个WinSxS组件..." -ForegroundColor Cyan
    
    # 处理每个组件
    $processed = 0
    foreach ($component in $allComponents) {
        $processed++
        $componentName = $component.Name
        
        # 显示进度
        if ($processed % 100 -eq 0 -or $processed -eq $totalComponents) {
            $percent = [math]::Round(($processed / $totalComponents) * 100, 1)
            Write-Progress -Activity "处理WinSxS组件" -Status "已处理 $processed/$totalComponents ($percent%)" -PercentComplete $percent
        }
        
        # 判断是否需要保留
        if (Should-RetainComponent -ComponentName $componentName) {
            $retainedCount++
            
            # 复制组件到新目录
            $sourcePath = $component.FullName
            $destPath = Join-Path -Path $winSxSNew -ChildPath $componentName
            
            try {
                # 复制目录
                Copy-Item -Path $sourcePath -Destination $destPath -Recurse -Force -ErrorAction Stop
                
                # 复制manifest文件到Manifests目录
                $manifestFiles = Get-ChildItem -Path $sourcePath -Filter "*.manifest" -File -ErrorAction SilentlyContinue
                if ($manifestFiles) {
                    $manifestDestDir = Join-Path -Path $winSxSNew -ChildPath "Manifests"
                    if (-not (Test-Path $manifestDestDir)) {
                        New-Item -ItemType Directory -Path $manifestDestDir -Force | Out-Null
                    }
                    foreach ($manifest in $manifestFiles) {
                        $manifestDest = Join-Path -Path $manifestDestDir -ChildPath $manifest.Name
                        Copy-Item -Path $manifest.FullName -Destination $manifestDest -Force -ErrorAction SilentlyContinue
                    }
                }
            } catch {
                Write-Warning "复制组件失败 '$componentName': $_"
            }
        }
    }
    
    Write-Progress -Activity "处理WinSxS组件" -Completed
    
    # 确保特殊目录存在并复制内容
    Write-Host "处理特殊元数据目录..." -ForegroundColor Cyan
    $rules = Get-WinSxSRetentionRules
    foreach ($dir in $rules.SpecialDirectories) {
        $specialDirPath = Join-Path -Path $winSxSPath -ChildPath $dir
        $specialDirNew = Join-Path -Path $winSxSNew -ChildPath $dir
        
        if (Test-Path $specialDirPath) {
            Write-Host "复制目录: $dir" -ForegroundColor Cyan
            Copy-Item -Path "$specialDirPath\*" -Destination $specialDirNew -Recurse -Force -ErrorAction SilentlyContinue
        } else {
            # 创建空目录
            New-Item -ItemType Directory -Path $specialDirNew -Force | Out-Null
        }
    }
    
    # 计算大小对比
    try {
        $originalSize = (Get-ChildItem -Path $winSxSPath -Recurse -File -ErrorAction SilentlyContinue | 
                        Measure-Object -Property Length -Sum).Sum / 1GB
        $newSize = (Get-ChildItem -Path $winSxSNew -Recurse -File -ErrorAction SilentlyContinue | 
                    Measure-Object -Property Length -Sum).Sum / 1GB
    } catch {
        $originalSize = 0
        $newSize = 0
        Write-Warning "无法计算目录大小"
    }
    
    Write-Host "`n=== 优化统计 ===" -ForegroundColor Green
    Write-Host "原始组件总数: $totalComponents" -ForegroundColor Yellow
    Write-Host "保留组件数: $retainedCount" -ForegroundColor Green
    Write-Host "精简比例: $([math]::Round((($totalComponents - $retainedCount) / $totalComponents) * 100, 1))%" -ForegroundColor Cyan
    
    if ($originalSize -gt 0 -and $newSize -gt 0) {
        Write-Host "原始大小: $([math]::Round($originalSize, 2)) GB" -ForegroundColor Yellow
        Write-Host "精简后大小: $([math]::Round($newSize, 2)) GB" -ForegroundColor Green
        Write-Host "空间节省: $([math]::Round($originalSize - $newSize, 2)) GB" -ForegroundColor Cyan
    }
    
    # 替换WinSxS目录
    if (-not $NoConfirm) {
        Write-Host "`n=== 替换WinSxS目录 ===" -ForegroundColor Magenta
        Write-Host "是否继续并替换原始WinSxS目录？(y/N)" -ForegroundColor Yellow
        $confirm = Read-Host
        
        if ($confirm -ne 'y') {
            Write-Host "取消替换操作。" -ForegroundColor Yellow
            Write-Host "新的WinSxS目录位于: $winSxSNew" -ForegroundColor Cyan
            return $false
        }
    }
    
    Write-Host "开始替换WinSxS目录..." -ForegroundColor Yellow
    
    try {
        # 1. 删除原始WinSxS目录
        Write-Host "删除原始WinSxS目录..." -ForegroundColor Yellow
        Remove-Item -Path $winSxSPath -Recurse -Force -ErrorAction Stop
        
        # 2. 移动新的WinSxS到目标位置
        Write-Host "移动新的WinSxS目录..." -ForegroundColor Yellow
        Move-Item -Path $winSxSNew -Destination $winSxSPath -Force -ErrorAction Stop
        
        Write-Host "替换完成！" -ForegroundColor Green
        return $true
    } catch {
        Write-Error "替换WinSxS目录失败: $_"
        Write-Host "新的WinSxS目录保留在: $winSxSNew" -ForegroundColor Cyan
        return $false
    }
}

function Mount-WindowsImage {
    <#
    .SYNOPSIS
    使用dism挂载Windows镜像
    #>
    param(
        [string]$WimPath,
        [int]$Index,
        [string]$MountPath
    )
    
    Write-Host "正在挂载Windows镜像..." -ForegroundColor Green
    Write-Host "WIM文件: $WimPath" -ForegroundColor Yellow
    Write-Host "镜像索引: $Index" -ForegroundColor Yellow
    Write-Host "挂载目录: $MountPath" -ForegroundColor Yellow
    
    # 检查WIM文件是否存在
    if (-not (Test-Path $WimPath)) {
        Write-Error "WIM文件不存在: $WimPath"
        return $false
    }
    
    # 检查挂载目录是否存在，不存在则创建
    if (-not (Test-Path $MountPath)) {
        New-Item -ItemType Directory -Path $MountPath -Force | Out-Null
    }
    
    # 检查目录是否已挂载
    $mountInfo = dism /Get-MountedImageInfo 2>$null
    if ($mountInfo -match [regex]::Escape($MountPath)) {
        Write-Warning "目录已被挂载: $MountPath"
        Write-Host "尝试卸载现有挂载..." -ForegroundColor Yellow
        $unmountResult = Dismount-WindowsImage -MountPath $MountPath -Discard
        if (-not $unmountResult) {
            Write-Error "无法卸载现有挂载"
            return $false
        }
    }
    
    # 使用dism挂载镜像
    try {
        Write-Host "执行挂载命令..." -ForegroundColor Cyan
        $dismOutput = dism /Mount-Image /ImageFile:"$WimPath" /Index:$Index /MountDir:"$MountPath" 2>&1
        
        # 检查dism命令是否成功
        if ($LASTEXITCODE -eq 0) {
            Write-Host "镜像挂载成功！" -ForegroundColor Green
            return $true
        } else {
            Write-Error "镜像挂载失败，错误代码: $LASTEXITCODE"
            Write-Host "DISM输出: $dismOutput" -ForegroundColor Red
            return $false
        }
    } catch {
        Write-Error "挂载过程中发生错误: $_"
        return $false
    }
}

function Dismount-WindowsImage {
    <#
    .SYNOPSIS
    使用dism卸载Windows镜像
    #>
    param(
        [string]$MountPath,
        [switch]$Discard  # 是否丢弃更改
    )
    
    Write-Host "正在卸载Windows镜像..." -ForegroundColor Yellow
    Write-Host "挂载目录: $MountPath" -ForegroundColor Yellow
    
    # 检查目录是否已挂载
    $mountInfo = dism /Get-MountedImageInfo 2>$null
    if (-not ($mountInfo -match [regex]::Escape($MountPath))) {
        Write-Warning "目录未被挂载: $MountPath"
        return $true
    }
    
    # 使用dism卸载镜像
    try {
        if ($Discard) {
            Write-Host "丢弃更改并卸载..." -ForegroundColor Yellow
            $dismOutput = dism /Unmount-Image /MountDir:"$MountPath" /Discard 2>&1
        } else {
            Write-Host "提交更改并卸载..." -ForegroundColor Yellow
            $dismOutput = dism /Unmount-Image /MountDir:"$MountPath" /Commit 2>&1
        }
        
        # 检查dism命令是否成功
        if ($LASTEXITCODE -eq 0) {
            Write-Host "镜像卸载成功！" -ForegroundColor Green
            return $true
        } else {
            Write-Error "镜像卸载失败，错误代码: $LASTEXITCODE"
            Write-Host "DISM输出: $dismOutput" -ForegroundColor Red
            return $false
        }
    } catch {
        Write-Error "卸载过程中发生错误: $_"
        return $false
    }
}

function Get-WimInfo {
    <#
    .SYNOPSIS
    获取WIM文件信息
    #>
    param(
        [string]$WimPath
    )
    
    Write-Host "获取WIM文件信息..." -ForegroundColor Cyan
    try {
        $wimInfo = dism /Get-WimInfo /WimFile:"$WimPath"
        return $wimInfo
    } catch {
        Write-Error "获取WIM信息失败: $_"
        return $null
    }
}

# 主执行逻辑
function Main {
    # 检查管理员权限
    if (-not (Test-Administrator)) {
        Write-Error "请以管理员身份运行此脚本！"
        exit 1
    }
    
    Write-Host "=== WinSxS 精简工具 ===" -ForegroundColor Cyan
    Write-Host "基于nano11builder.ps1逻辑，重新设计WinSxS精简" -ForegroundColor Yellow
    
    # 获取管理员组
    $adminGroup = Get-AdminGroup
    Write-Host "管理员组: $adminGroup" -ForegroundColor Cyan
    
    $mountRequired = $false
    $mountSuccess = $false
    
    # 处理WIM挂载逻辑
    if ($WimPath -and $AutoMount) {
        Write-Host "`n=== 挂载镜像模式 ===" -ForegroundColor Green
        
        # 显示WIM文件信息
        $wimInfo = Get-WimInfo -WimPath $WimPath
        if ($wimInfo) {
            Write-Host "`nWIM文件包含以下镜像:" -ForegroundColor Cyan
            # 提取并显示索引信息
            $indexMatches = [regex]::Matches($wimInfo, "Index : (\d+)")
            $nameMatches = [regex]::Matches($wimInfo, "Name : (.+)")
            
            for ($i = 0; $i -lt $indexMatches.Count; $i++) {
                $idx = $indexMatches[$i].Groups[1].Value
                $name = if ($i -lt $nameMatches.Count) { $nameMatches[$i].Groups[1].Value } else { "未知" }
                Write-Host "  索引 $idx : $name" -ForegroundColor Yellow
            }
            
            if ($indexMatches.Count -gt 0) {
                Write-Host "`n使用索引: $Index" -ForegroundColor Green
            }
        }
        
        # 挂载镜像
        $mountSuccess = Mount-WindowsImage -WimPath $WimPath -Index $Index -MountPath $MountPath
        if (-not $mountSuccess) {
            Write-Error "镜像挂载失败，无法继续"
            exit 1
        }
        $mountRequired = $true
    }
    
    # 检查挂载目录是否存在
    if (-not (Test-Path $MountPath)) {
        Write-Error "挂载目录不存在: $MountPath"
        if ($mountRequired) {
            Write-Host "尝试清理..." -ForegroundColor Yellow
            Dismount-WindowsImage -MountPath $MountPath -Discard
        }
        exit 1
    }
    
    # 检查临时目录
    if (-not (Test-Path $ScratchDir)) {
        Write-Host "创建临时目录: $ScratchDir" -ForegroundColor Yellow
        New-Item -ItemType Directory -Path $ScratchDir -Force | Out-Null
    }
    
    # 执行WinSxS优化
    Write-Host "`n=== 开始WinSxS优化 ===" -ForegroundColor Green
    $result = Optimize-WinSxS -MountPath $MountPath -ScratchDir $ScratchDir -AdminGroup $adminGroup -NoConfirm:$NoConfirm
    
    # 处理卸载逻辑
    if ($mountRequired -and $AutoUnmount) {
        Write-Host "`n=== 卸载镜像 ===" -ForegroundColor Green
        if ($result) {
            # 优化成功，提交更改
            Dismount-WindowsImage -MountPath $MountPath
        } else {
            # 优化失败，丢弃更改
            Write-Host "优化失败，丢弃更改..." -ForegroundColor Red
            Dismount-WindowsImage -MountPath $MountPath -Discard
        }
    } elseif ($mountRequired) {
        Write-Host "`n=== 镜像保持挂载 ===" -ForegroundColor Yellow
        Write-Host "挂载目录: $MountPath" -ForegroundColor Cyan
        Write-Host "如需卸载，请手动运行: dism /Unmount-Image /MountDir:`"$MountPath`" /Commit" -ForegroundColor Yellow
    }
    
    if ($result) {
        Write-Host "`n=== WinSxS优化成功完成！ ===" -ForegroundColor Green
    } else {
        Write-Host "`n=== WinSxS优化未完成或已取消 ===" -ForegroundColor Yellow
    }
    
    # 清理临时目录（保留ScratchDir，只删除内部内容）
    try {
        $tempItems = Get-ChildItem -Path $ScratchDir -Exclude "WinSxS_new" -ErrorAction SilentlyContinue
        foreach ($item in $tempItems) {
            Remove-Item -Path $item.FullName -Recurse -Force -ErrorAction SilentlyContinue
        }
    } catch {
        # 忽略清理错误
    }
}

# 运行主函数
try {
    Main
} catch {
    Write-Error "执行过程中发生错误: $_"
    Write-Host "错误详细信息:" -ForegroundColor Red
    Write-Host $_.Exception -ForegroundColor Red
    Write-Host "脚本栈追踪:" -ForegroundColor Red
    Write-Host $_.ScriptStackTrace -ForegroundColor Red
    exit 1
}