# WebAssembly Python 解释器

一个用 Python 重构的 WebAssembly MVP（Minimum Viable Product）解释器，完整支持 178 条 Wasm 指令。

## 项目结构

```
wasm解释器/
├── __init__.py          # 包初始化
├── __main__.py          # 入口模块
├── 枚举定义.py          # 操作码、值类型等枚举定义
├── 数据结构.py          # 核心数据结构（模块、栈帧、控制块等）
├── 工具函数.py          # LEB128编解码、类型转换等工具函数
├── 模块加载器.py        # Wasm二进制模块解析器
├── 解释器核心.py        # 指令执行引擎
├── 命令行接口.py        # CLI界面
└── 测试用例.py          # 单元测试
```

## 与原C项目的对应关系

| Python 文件 | 原C文件 | 说明 |
|------------|---------|------|
| 枚举定义.py | opcode.h | 178条指令操作码枚举 |
| 数据结构.py | module.h | 模块、栈、帧等数据结构 |
| 工具函数.py | utils.c/utils.h | LEB128、类型转换工具 |
| 模块加载器.py | module.c | Wasm二进制解析 |
| 解释器核心.py | interpreter.c | 指令执行循环 |
| 命令行接口.py | cli.c | 命令行界面 |

## 支持的指令

### 控制流指令 (15条)
- `unreachable`, `nop`, `block`, `loop`, `if`, `else`, `end`
- `br`, `br_if`, `br_table`, `return`

### 参数指令 (2条)
- `drop`, `select`

### 变量访问指令 (5条)
- `local.get`, `local.set`, `local.tee`
- `global.get`, `global.set`

### 内存指令 (25条)
- `i32.load`, `i64.load`, `f32.load`, `f64.load`
- `i32.load8_s`, `i32.load8_u`, `i32.load16_s`, `i32.load16_u`
- `i64.load8_s`, `i64.load8_u`, `i64.load16_s`, `i64.load16_u`, `i64.load32_s`, `i64.load32_u`
- `i32.store`, `i64.store`, `f32.store`, `f64.store`
- `i32.store8`, `i32.store16`, `i64.store8`, `i64.store16`, `i64.store32`
- `memory.size`, `memory.grow`

### 常量指令 (4条)
- `i32.const`, `i64.const`, `f32.const`, `f64.const`

### 整数比较指令 (20条)
- `i32.eqz`, `i32.eq`, `i32.ne`, `i32.lt_s`, `i32.lt_u`, `i32.gt_s`, `i32.gt_u`, `i32.le_s`, `i32.le_u`, `i32.ge_s`, `i32.ge_u`
- `i64.eqz`, `i64.eq`, `i64.ne`, `i64.lt_s`, `i64.lt_u`, `i64.gt_s`, `i64.gt_u`, `i64.le_s`, `i64.le_u`, `i64.ge_s`, `i64.ge_u`

### 整数运算指令 (28条)
- `i32.add`, `i32.sub`, `i32.mul`, `i32.div_s`, `i32.div_u`, `i32.rem_s`, `i32.rem_u`
- `i32.and`, `i32.or`, `i32.xor`, `i32.shl`, `i32.shr_s`, `i32.shr_u`, `i32.rotl`, `i32.rotr`
- `i64.add`, `i64.sub`, `i64.mul`, `i64.div_s`, `i64.div_u`, `i64.rem_s`, `i64.rem_u`
- `i64.and`, `i64.or`, `i64.xor`, `i64.shl`, `i64.shr_s`, `i64.shr_u`, `i64.rotl`, `i64.rotr`

### 浮点比较指令 (12条)
- `f32.eq`, `f32.ne`, `f32.lt`, `f32.gt`, `f32.le`, `f32.ge`
- `f64.eq`, `f64.ne`, `f64.lt`, `f64.gt`, `f64.le`, `f64.ge`

### 浮点运算指令 (24条)
- `f32.abs`, `f32.neg`, `f32.ceil`, `f32.floor`, `f32.trunc`, `f32.nearest`, `f32.sqrt`
- `f32.add`, `f32.sub`, `f32.mul`, `f32.div`, `f32.min`, `f32.max`, `f32.copysign`
- `f64.abs`, `f64.neg`, `f64.ceil`, `f64.floor`, `f64.trunc`, `f64.nearest`, `f64.sqrt`
- `f64.add`, `f64.sub`, `f64.mul`, `f64.div`, `f64.min`, `f64.max`, `f64.copysign`

### 类型转换指令 (43条)
- `i32.wrap_i64`, `i32.trunc_f32_s`, `i32.trunc_f32_u`, `i32.trunc_f64_s`, `i32.trunc_f64_u`
- `i64.extend_i32_s`, `i64.extend_i32_u`, `i64.trunc_f32_s`, `i64.trunc_f32_u`, `i64.trunc_f64_s`, `i64.trunc_f64_u`
- `f32.convert_i32_s`, `f32.convert_i32_u`, `f32.convert_i64_s`, `f32.convert_i64_u`, `f32.demote_f64`
- `f64.convert_i32_s`, `f64.convert_i32_u`, `f64.convert_i64_s`, `f64.convert_i64_u`, `f64.promote_f32`
- `i32.reinterpret_f32`, `i64.reinterpret_f64`, `f32.reinterpret_i32`, `f64.reinterpret_i64`
- `i32.extend8_s`, `i32.extend16_s`, `i64.extend8_s`, `i64.extend16_s`, `i64.extend32_s`
- `i32.trunc_sat_f32_s`, `i32.trunc_sat_f32_u`, `i32.trunc_sat_f64_s`, `i32.trunc_sat_f64_u`
- `i64.trunc_sat_f32_s`, `i64.trunc_sat_f32_u`, `i64.trunc_sat_f64_s`, `i64.trunc_sat_f64_u`

## 使用方法

### 命令行使用

```bash
# 运行 Wasm 文件
python -m wasm解释器 run example.wasm

# 转储模块信息
python -m wasm解释器 dump example.wasm

# 调用特定函数
python -m wasm解释器 invoke example.wasm add 1 2

# 交互式模式
python -m wasm解释器 repl
```

### 编程使用

```python
from wasm解释器 import 模块加载器, 解释器

# 加载 Wasm 模块
with open('example.wasm', 'rb') as f:
    字节码 = f.read()

加载器 = 模块加载器()
模块实例 = 加载器.加载模块(字节码)

# 执行
解释器实例 = 解释器(模块实例)
解释器实例.解释()
```

## 命名规范

本项目采用中文标识符命名规范：
- 类名使用中文（如：`模块`, `解释器`, `栈帧`）
- 函数名使用中文（如：`加载模块`, `压入操作数`, `执行返回`）
- 变量名使用中文（如：`程序计数器`, `操作数栈指针`）
- 常量名使用中文（如：`栈溢出`, `除零错误`）

## 测试

```bash
cd wasm解释器
python 测试用例.py
```

## 技术特点

1. **完整指令支持**：实现全部 178 条 Wasm MVP 指令
2. **模块化设计**：清晰的模块划分，易于理解和扩展
3. **中文命名**：使用中文标识符，提高代码可读性
4. **类型安全**：使用 Python 类型注解
5. **完整测试**：包含单元测试确保正确性

## 与原C项目的区别

1. **内存管理**：Python 自动内存管理，无需手动分配/释放
2. **类型系统**：Python 动态类型，使用类型注解辅助
3. **错误处理**：使用异常机制替代错误码
4. **数据结构**：使用 Python 高级数据结构（list, dict等）
5. **字符串处理**：Python 原生 Unicode 支持

## 许可证

重构项目，仅供学习参考
