# Aurora-32

Aurora-32 是一个自定义 32 位、字寻址、微程序控制的 CPU 项目。项目包含电路、
主/用户/中断微码、早期裸机示例、双进程内核、用户态终端程序，以及配套的
Windows x64 Clang/LLVM 工具快照。本文档整理于 2026-09-14。

> **不受支持、已弃用：** [deprecated-bare-metal/](deprecated-bare-metal/README.md)
> 仅保留早期裸机源码供历史参考，不参与当前构建，不保证兼容现有 CPU、微码和内核。
> 请使用 `user-programs/` 中的现行示例，不要将旧裸机程序加载到 UROM。

## 快速开始

在 Windows 的 PowerShell 中，从项目根目录执行：

```powershell
.\build-tools\build.ps1
```

脚本先编译内核，再读取内核实际系统调用入口，编译全部现存用户程序。

编译自己的用户 C 文件只需一行，文件可以位于项目外：

```powershell
.\build-tools\build-user.ps1 "C:\my-project\main.c"
```

脚本自动构建配套内核并取得系统调用地址，生成 `C:\my-project\build\main.hex`
及同名 `.s/.o/.bin`，不用登记到示例清单。

不需要安装 LLVM、Visual Studio、CMake、Ninja 或 customasm。
需要 Windows 10/11 x64 和 PowerShell；终端执行策略如果禁止本地脚本，
可以用 `powershell -NoProfile -ExecutionPolicy Bypass -File .\build-tools\build.ps1`。

1. 在电路中把 `kernel/build/kernel.hex` 加载到内核 ROM，复位 PC 为 `0`。
2. 从 `user-programs/programs/<name>/build/` 选择**一个**用户 HEX 加载到 UROM。
3. UROM 的 CPU 字地址基址为 `0x80000000`，HEX 文件从该存储器的本地偏移 0 开始加载。
4. 将 `micro-code/kernel/micro_code.hex`、`micro-code/user/micro_code_user.hex`、
   `micro-code/hard-int/micro_code_int.hex` 分别用于对应控制单元。
5. 复位运行，内核完成启动日志后，由 tick 调度进入用户程序。

更改内核后必须重新编译用户程序：`INT` 中写的是内核函数的实际 ROM 地址，
不是固定的 IVT 表项。旧用户 HEX 与新内核不保证兼容。

## 目录

| 路径 | 内容 |
|---|---|
| `32位计算机.dig` | 主电路，保留原文件名和电路内容 |
| `micro-code/kernel/`、`micro-code/user/`、`micro-code/hard-int/` | 主/用户/硬中断微码项目及导出文件 |
| `kernel/` | 内核源文件、构建入口、内核自己的 `build/` |
| `user-programs/include/` | 系统调用 ABI、用户回调封装、`system.io` |
| `user-programs/programs/` | 计算器、五子棋、猜数字、贪吃蛇、迷宫、迷你终端及各自 `build/` |
| `deprecated-bare-metal/` | **不受支持、已弃用**的裸机源码归档；旧编译产物已清理 |
| `build-tools/` | 可直接用于本项目的编译工具、头文件、运行库、脚本 |
| `docs/user_program/` | 每个用户示例的详细 README |
| `docs/kernel/` | 内核结构、调度、回调、异常、系统调用 |
| `docs/aurora/` | 架构、地址、ABI、指令、编译器约定和移植注意事项 |
| `docs/build-tools/` | 工具清单、构建链路、更新与验证方法 |

## 文档入口

- [用户程序总览](docs/user_program/README.md)
- [计算器](docs/user_program/calculator/README.md)
- [计算器跟踪版本](docs/user_program/calculator-trace/README.md)
- [5×5 五子棋](docs/user_program/tic-tac-toe/README.md)
- [猜数字](docs/user_program/guess-number/README.md)
- [终端贪吃蛇与批量输出](docs/user_program/snake/README.md)
- [迷宫探险](docs/user_program/maze/README.md)
- [迷你命令终端与 Telnet](docs/user_program/mini-terminal/README.md)
- [内核说明](docs/kernel/README.md)
- [Aurora 架构和编程约定](docs/aurora/README.md)
- [指令速查](docs/aurora/instruction-set.md)
- [自包含构建工具](docs/build-tools/README.md)
- [英文目录迁移对照](docs/aurora/directory-migration.md)

## 范围和限制

目前运行的是一个系统进程和一个用户进程，没有文件系统、动态装载器、多个用户
进程、通用 libc 或完整 shell；迷你终端仅执行内置命令。用户退出/异常时内核会打印原因并停机。
`deprecated-bare-metal/` 已弃用，里面的程序是历史内核态裸机程序，不能当作 UROM 用户程序运行。
本文档描述软件当前需要的硬件契约，不表示所有异常电路时序均已由软件测试覆盖。
