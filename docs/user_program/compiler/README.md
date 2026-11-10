# Tiny C：机内原生编译器与自举

`compiler` 是 Aurora-32 用户态程序。它通过系统调用读取 ASCII 源文件，在 Aurora CPU 上
完成词法分析、递归下降解析和机器码生成，再创建一个新的 AEXE 可执行文件。
它不是解释器，不调用宿主机 Clang，也不通过 Telnet 把源程序交给外部编译。

## 使用

更新配套 `kernel/build/kernel.hex` 和 `storage/build/storage.hex`。镜像重新加载会覆盖
模拟 EEPROM 的运行时修改；请先导出保存。UROM、URAM、UStack 的物理内部地址均为 24 位，
每进程使用一个 22 位银行。生成的函数入口以 `0x823fff00` 检查栈上限并预留尾部空间，
不再使用旧的 `0x82ffff00`；越界在分配函数栈帧前报错退出。

```text
aurora:/ > ls tinyc
aurora:/ > run compiler
Source > tinyc/arrays.c
Output > sorted
Run in background? [Y/n] > N
Compiler running in foreground.
Compiling...
  Function: sort
  Function: main
Saving executable...
Compiled ... code words. Run: sorted
aurora:/ > run sorted
```

输入/输出路径都从根目录解析。输出必须是**尚不存在**的名字；不会覆盖源码、已有程序或正在
运行的编译器。父目录必须存在。输入 `q` 取消。前台语法错误显示行号，不安装半成品。
输入被内核派发给用户回调，内置输入运行库暂存字符，`getchar()` 无数据时通过 WaitEvent 休眠。
解析、代码生成和文件操作都在主执行流，不在键盘回调里执行。

输入完 Output 后询问 `Run in background? [Y/n] >`：Y/y 或回车转后台，N/n 留前台，
q/Q 取消且不安装产物；其他回答会重新询问。CRLF 只作为一个换行，单独 LF 也支持默认选择。
选择后台时通过系统调用 36（`system.Background()`）主动转后台，原槽位、银行和执行
现场保持不变。系统提示符会提前出现，可以执行 `ps` 或启动另一个前台程序。
后台所有用户终端输出由内核静默丢弃，包括进度和详细错误；不会干扰前台交互。
必须等带 compiler 程序名/槽位号的退出通知，且退出码为 0，才运行产物。
要查看函数进度或错误详情，请选择 N 前台编译；后台不会保存输出供稍后查看。
如果该终端已有后台任务，则提示 `Background unavailable; continuing in foreground.` 并继续
前台编译，不抢占现有后台。不要用 `compiler &` 启动，因为最初仍需要前台输入文件名。
所有 FS 请求（Open/Info/Read/Install）若碰到另一终端执行 ls 等文件命令、暂时占用内核
工作区，会在 EBUSY(-5) 时 Yield 重试；前台读取源码期间也适用，不仅仅在后台保存时。
其他错误照常报告，不取消内核的文件系统互斥保护。
以上行为在同源生成的 compiler2/3 中也成立，Tiny C 源码使用 `sys(36,0,0,0)` 调用。

前台编译过程中，每开始处理一个函数定义就输出 `Function: 函数名`；仅有原型的声明不输出。
这是当前正在编译的函数，不表示该函数已成功完成。语法错误仍会输出行号和原因。
仅按函数输出一次，不逐 token/逐行打印；大函数内部仍可能等待一段时间。
种子编译器和机内自举生成的 compiler2/3 均支持此进度输出。

用 `editor` 修改源码：`open tinyc/arrays.c`，再使用 `set N TEXT`、`append TEXT`、`save`。
也可先在系统终端 `touch tinyc/my.c`，进入 editor 编写。touch 默认容量仍是 2048 字；
文件不能自动扩大。镜像中的 `tinyc/compiler.c` 单独预留 65536 字，供机内修改自身。

## 自举操作

```text
run compiler
Source > tinyc/compiler.c
Output > compiler2

run compiler2
Source > tinyc/compiler.c
Output > compiler3
```

`tinyc/compiler.c` 直接来自 `user-programs/programs/compiler/compiler.c`，打包时不改写源码。
首次使用 LLVM 编译得到种子 compiler；compiler2 和 compiler3 的编译过程都由 Aurora
机器码完成。编译结果先保存在 storage，返回系统终端后才重新装载 UROM，不覆盖正在执行的代码。

源码唯一的 `#include "bootstrap.h"` 是明确约定的种子运行库桥接：宿主构建使用该头文件提供
入口、INT 包装和键盘收取；机内编译器识别这一行，生成等效原生运行库。它不包含任何词法、
解析或代码生成逻辑，不是隐藏的第二个编译器。其他预处理指令会报错。

验收脚本 `tests/compiler/run_selfhost.py` 运行真实内核与用户 BIN，比较 compiler2/3 的完整
AEXE 和代码字序列，并用生成的编译器再编译和执行数组示例。测试使用理想 ISA/设备模型，
**不是 Digital 电路时序模拟**。生成产物和 SHA-256 报告在 `tests/compiler/build/`。

## 语言子集

必须有 `int main()`，无参数，返回退出码。不再支持旧版的文件顶层可执行语句。

| 功能 | 支持范围 |
|---|---|
| 类型 | 有符号 32 位 `int`；`char` 同样存完整 32 位字；最多 8 层指针 |
| 数据 | 全局变量/一维定长数组、局部变量/数组、参数；全局及未初始化局部清零 |
| 数组 | 长度为 1..131072 的整数字面量；作为参数传指针；每项一个字 |
| 指针 | `&x`、`*p`、`p[i]`、指针加减、`int`/指针强制转换；不支持函数指针 |
| 函数 | 定义、带参数原型、递归/互递归、前向调用；先声明后使用；最多 8 个参数 |
| 控制流 | `{}`、`if/else`、`while`、`break`、`continue`、`return expression;` |
| 运算 | `+ - * / %`，比较，短路 `&& ||`，`! ~ & | ^`，右结合赋值 |
| 字面量 | 十进制、0x 十六进制、字符、字符串；字符串最多 255 个字符 |
| 转义 | `\n \r \t \b \0 \\ \' \"`；注释支持 `//` 和 `/* ... */` |

块结束后局部名字失效，兄弟块可重用名字；不允许声明遮蔽一个仍在作用域内的局部变量。
数组自动衰减为指针，不能整体赋值，不支持多维数组和初始化列表。全局定义不支持显式初始化器；
可在 main 中赋值。局部数组在执行到声明时清零。

为避免编译器启动时清空二十多万个工作区字，支持一个明确的扩展：
`int workspace[4096] __attribute__((section(".noinit")));`。
仅允许全局数组使用这个精确的属性形式；它不保证初值为零，必须先写后读。
普通全局和局部变量的清零规则不变。编译器自己的源码、代码及符号表缓冲区使用此属性，
按实际读取/生成的长度填充，每个新符号由 `name_symbol` 初始化。
LLVM 种子和自举版本都遵循此约定，不依赖开机 RAM 恰好为零。
`tests/compiler/run_startup.py` 检查带旧内存数据的启动、重复运行和普通全局清零，
`run_selfhost.py` 也在未写 URAM 返回脏值的模型上验证自举。

整型加减乘按 32 位回绕；除法向零截断，余数与被除数同号，INT_MIN/-1 回绕。
除零由运行库打印错误并退出，不继续执行随机结果。指针不提供数组边界检查；越权访问依赖硬件
用户态保护。指针/整型参数目前只做字宽传递与参数数量检查，不是完整 ISO C 类型检查器。

不支持 `for/do/switch`、`++/--`、复合赋值、三目、结构体、typedef、enum、float、动态分配、
一般头文件/宏、标准 C 库、变参、独立目标文件链接。已有完整 C 游戏仍使用宿主 LLVM 构建，
并不能直接交给 Tiny C。

## 内置运行库

| 调用 | 行为 |
|---|---|
| `print(expr)` | 整数按十进制、指针按零终止字符串输出，然后换行 |
| `putstr(pointer)` / `putint(value)` / `putchar(value)` | 不自动换行；对应系统输出接口 |
| `getchar()` | 有字符就返回；否则 WaitEvent；不会忙轮询设备 |
| `sleep(ms)` | 事件可提前唤醒的 Sleep；参数限 0..INT_MAX，不保证无事件时以外的完整延时 |
| `exit(code)` | 结束用户进程并返回系统终端 |
| `sys(number,a,b,c)` | 直接使用 ABI3 INT 网关；返回 r0，可调用文件/时间等系统服务 |

内核文件缓冲、描述符和字符串接口只接受 UROM/URAM，**不接受局部用户栈缓冲**。
与系统调用交互的可写数组应定义在全局。普通函数之间传递局部数组/指针不受此限制。
输入运行库通过回调收取低 8 位字符，255 字符环形暂存区满或内核报告丢事件时退出码 2，
不默默继续使用缺损输入。回调通过 INT CALLBACK_RETURN 恢复完整中断现场。

## 容量与运行时间

- 源码上限 65536 个 ASCII 字符，代码上限 131072 个字。
- 最多 511 个全局符号（含内置函数）、128 个同时在作用域内的局部符号。
- 表达式、语句和循环嵌套最多 96 层。
- 全局数据最多 1048576 字，单函数局部帧最多 65536 字。
- 私有调用约定使用 USP 和保存的 r15；函数入口检查栈上限。与 LLVM 编译的普通函数不能混合链接。
- 启动代码在任何 PUSH/CALL 前设置 USP/UFP，并清零全局数据；大程序启动需要更多时间。
- 编译自身显著慢于编译几十行的示例。当前是直接生成代码，没有寄存器分配、内联或高级优化。
- 文件读取每次最多 64 字；Install/Save 目前是有上限的同步系统调用，大文件保存可能暂时
  延迟 tick、键盘和另一终端响应。没有写入事务，保存期间不要复位。

宿主种子构建通过 `user-programs/build.ps1 -Program compiler`，使用 `-fwrapv` 保证词法分析
中的 32 位回绕。bootstrap.h 对一个短函数禁用优化，以避开当前 LLVM 后端未支持的 ROTL 合并。
所有其他程序保留原有构建选项。
