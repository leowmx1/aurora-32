# 项目内构建工具

`build-tools/` 是本项目所用 Aurora 定制 Clang/LLVM 的 Windows x64 可执行快照，
并包含头文件、运行 DLL 和 HEX 转换脚本。目的是在移动项目目录后直接重建内核
与当前用户 C 示例，不需要原 `llvm-project` 的源码树、编译目录或环境变量。

## 工具清单

| 路径（相对 build-tools） | 用途 |
|---|---|
| `bin/clang.exe` | C 前端、驱动和内置 Aurora 后端/集成汇编器；日常编译入口 |
| `bin/llc.exe` | 独立 LLVM IR → Aurora 汇编/对象的后端入口 |
| `bin/opt.exe` | IR 检查与优化，诊断用 |
| `bin/llvm-mc.exe` | LLVM Aurora 汇编器/编码检查 |
| `bin/llvm-objcopy.exe` | 提取 `.text` 为裸二进制 |
| `bin/llvm-nm.exe` | 查询内核系统调用符号和未定义符号 |
| `bin/llvm-readobj.exe` | 检查重定位、对象头和 PE DLL 依赖 |
| `bin/llvm-objdump.exe` | 反汇编/查看对象 |
| `bin/msvcp140.dll`、`vcruntime140.dll`、`vcruntime140_1.dll` | 这些工具实际导入的应用本地 VC 运行库 |
| `lib/clang/24/include/` | 整套 Clang resource headers，包含定制 `aurora.h` |
| `include/` | 用户 ABI、`system.io.buffer` 缓冲输出和 `kbqueue` 键盘队列头文件的打包快照 |
| `build.ps1` | 总构建入口，先内核再用户程序，可用 -Program 选择目标 |
| `build-user.ps1` | 任意单个用户 C 文件的一行编译入口，自动确定产物目录和系统调用地址 |
| `compile.ps1` | 公共 C→汇编/对象→bin→HEX 构建流程及校验 |
| `bin-to-logisim.ps1` | 每 4 个小端字节转换为一个 32 位十六进制字 |
| `toolchain-manifest.json` | 工具/头文件/脚本的 SHA-256、大小和来源版本信息 |
| `LICENSE-LLVM.txt` | LLVM 项目许可证 |

Clang 的前端和后端已集成在同一个 `clang.exe`，无需另一个所谓 `clang-cc1.exe`。
单独提供 llc/opt/llvm-mc 是为了可观察和分阶段验证。
当前构建没有通用链接步骤：后端为单个完整模块安排入口、全局初始化和绝对地址，
构建脚本确认对象没有未定义符号/重定位后才提取 `.text`。这不是任意多对象链接方案。

## 运行环境

支持范围：Windows 10/11 x64、PowerShell 5.1 或更高，项目路径允许空格和中文。
工具为 Windows PE x64，不是 Linux/macOS 可执行文件。
核心 VC DLL 随包提供，UCRT 和 Win32 系统 DLL 由 Windows 提供。
不需要把 bin 加到 PATH；脚本按 `$PSScriptRoot` 定位每个工具。
不需要 Visual Studio、Windows SDK、CMake、Ninja、Python、customasm。
工具 DLL 和 Microsoft 系统运行库不属于 Aurora 用户态 libc，不能装到 CPU 的 ROM。

## 完整构建命令

从项目根目录：

```powershell
.\build-tools\build.ps1
.\build-tools\build.ps1 -Program guess-number
.\build-tools\build.ps1 -Program calculator-trace
```

根构建总会先编译内核。`-Program all` 为默认，还接受 calculator、calculator-trace、
tic-tac-toe、guess-number、snake、maze、mini-terminal。只构建内核或使用现有内核对象：

```powershell
.\kernel\build.ps1
.\user-programs\build.ps1 -Program calculator
.\user-programs\build.ps1 -KernelObject .\kernel\build\kernel.o -Program tic-tac-toe
```

单独编译一个符合入口约定的内核 C 文件：

```powershell
.\build-tools\compile.ps1 .\kernel\kernel.c .\kernel\build\kernel.hex -Mode kernel
```

新用户文件使用 build-tools/build-user.ps1，已有示例也可使用 user-programs/build.ps1 获取实际 INT 地址；不要把下面的诊断命令
输出直接当成已经正确链接系统调用的用户镜像。

## 一行编译自己的用户程序

```powershell
# 从项目根目录调用；源码可以在任何位置
.\build-tools\build-user.ps1 "C:\my-project\main.c"

# 从任意目录调用；源码也可以使用相对当前目录的路径
& "F:\aurora-32\build-tools\build-user.ps1" ".\main.c"
```

第一个位置参数是 C 源文件路径，支持空格、中文和文件名中的方括号；必须是现存
`.c` 文件。输出自动位于源码**所在文件夹**的 `build/`，而非工具目录或当前终端目录。
例如 `C:/my-project/main.c` 生成 `C:/my-project/build/main.s/.o/.bin/.hex`。
不需要更改 `ValidateSet` 或现有示例清单，输出文件按源文件去掉 `.c` 的名称命名。

默认每次先重建本项目内核，再从新 `kernel.o` 提取系统调用地址，随后调用公共
`compile.ps1` 进行用户态构建与检查。内核产物仍在项目的 `kernel/build/`。
运行时应加载这次生成的配套内核，不要继续搭配另一份旧 ROM。

如果明确需要搭配现有内核对象，可以跳过内核重建：

```powershell
.\build-tools\build-user.ps1 "C:\my-project\main.c" -KernelObject .\kernel\build\kernel.o
```

源码仍需包含 `<aurora.h>`、`<aurora_system.h>`，声明 `AURORA_USER_PROGRAM;`，
定义 `_start`，并遵守用户栈初始化和系统 IO 约定。脚本不会修改源码或自动插入
缺失的入口声明。当前支持单个完整 C 翻译单元，不是通用多对象链接器。
源码同目录的本地头文件可以照常用 `#include "header.h"`。

## 构建链路与失败检查

通用参数为 `-target aurora -x c -std=c11 -ffreestanding -fomit-frame-pointer -O2`，
resource dir 显式指定为本包的 `lib/clang/24`。用户模式另定义
`AURORA_USER_API_ONLY=1` 与根据内核计算的 `AURORA_SYSCALL_ENTRY_ADDRESS`。
源文件的 `AURORA_USER_PROGRAM` 声明才是后端地址分配标记，`-Mode user` 是构建校验模式，
不能用它替代源码声明。

1. clang `-S` 生成可检查的汇编。
2. 用户汇编若包含 EI/DI/HALT/IRET/PUSHU/POPU 则拒绝构建。
3. clang `-c` 生成对象；nm 检查未定义符号，readobj 检查未解决重定位。
4. objcopy 提取 `.text`。检查长度按 4 字节对齐、首字为 JMP，以及入口落在生成镜像内。
5. bin-to-logisim 输出无 BOM 的 `v2.0 raw` HEX，每个单元为一个 32 位字。

`.s` 是审查文件，`.o` 是对象和符号依据，`.bin` 是小端机器字字节序列，`.hex` 是
加载到电路存储器的文本。四者不应互换；尤其不能把 ELF `.o` 直接加载进 ROM。
任何一步失败都应先修复，不使用前次残留 HEX 冒充本次成功结果。

## 独立查看前端/后端

以下为内核诊断命令，在根目录执行，不依赖外部工具：

```powershell
.\build-tools\bin\clang.exe -target aurora -ffreestanding -O2 -S -emit-llvm `
  -resource-dir .\build-tools\lib\clang\24 .\kernel\kernel.c -o .\kernel\build\kernel.ll
.\build-tools\bin\opt.exe -passes=verify -disable-output .\kernel\build\kernel.ll
.\build-tools\bin\llc.exe -mtriple=aurora -verify-machineinstrs `
  .\kernel\build\kernel.ll -o .\kernel\build\kernel-from-ir.s
.\build-tools\bin\llvm-nm.exe -n .\kernel\build\kernel.o
.\build-tools\bin\llvm-objdump.exe -d --triple=aurora .\kernel\build\kernel.o
```

用户 IR 诊断还必须传入正确的项目 include、用户 API 宏和系统调用地址。
Clang 可用 `-print-resource-dir` 查看默认头文件路径，`--version`/`-print-targets`
确认版本和 Aurora 目标。普通发行版 LLVM 没有此自定义目标，不能随意替换。

## 头文件来源与更新

`user-programs/include/` 是项目 ABI 的维护源；`build-tools/include/` 是复制快照。

当前编译器包含 Aurora 常量函数表的早期解析优化，使 `system.io.buffer.*`、`kbqueue.*`
等点号 API 能参与常规内联。语法与 ABI 不变；每次重建仍须使用同批次的内核和用户镜像。
性能对照与测试限制见 [验证记录](verification.md)。
用户构建显式加入项目 include，内核用相对 include，共同使用维护源。
修改 ABI 后要同步这份快照，重新构建内核和所有用户镜像，并更新 manifest。
`aurora.h` 属于定制编译器 resource headers，应与工具版本一起升级。

更新工具时，先完整重建 Aurora 目标表和相关 LLVM/Clang 工具，再整体替换本包。
不要混合不同 LLVM IR 版本、不同 TableGen 生成枚举或只替换部分过期对象。
目录快照不包含编译器源码构建系统，重建 LLVM 自身仍需维护中的 LLVM 仓库。

## 验收方法与已知限制

迁移时应做两次验证：新项目根目录完整构建，以及把源码和本包复制到另一条含空格
的路径、清除开发工具 PATH 后再构建，并比较生成二进制。检查所有文档相对链接
和脚本是否仍引用旧项目/LLVM 绝对路径。具体结果见 [验证记录](verification.md)。

内核/六个现存用户示例是此包验证的构建目标。它不包含旧 customasm、不承诺支持
历史裸机汇编方言、任意 C 标准库、动态链接或所有 C 类型。当前可写 char 数组
仍可能触发窄存储指令选择错误；使用字缓冲并检查汇编，不把它解释为缺失头文件。
硬件稳定性和中断时序仍须在电路中测试。

## 许可证与校验

LLVM 使用 Apache-2.0 WITH LLVM-exception，完整内容随包放于 LICENSE-LLVM.txt。
VC DLL 来自本机 Visual Studio 的 x64 redistributable CRT，归 Microsoft 所有，
不是 LLVM 许可证覆盖的文件。此快照用于本项目构建；向外分发时应同时保留上游声明。
manifest 记录复制来源与 SHA-256，便于追踪；校验值证明文件一致，不替代编译和运行测试。
