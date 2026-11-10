# Aurora-32

Aurora-32 是一个自定义 32 位、字寻址、微程序控制的 CPU 项目。项目包含电路、
主/用户/中断微码、早期裸机示例、四用户槽内核、用户态终端与图形程序，以及配套的
Windows x64 Clang/LLVM 工具快照，以及机内可自举的 Tiny C 编译器。更新于 2026-11-03。

> **不受支持、已弃用：** [deprecated-bare-metal/](deprecated-bare-metal/README.md)
> 仅保留早期裸机源码供历史参考，不参与当前构建，不保证兼容现有 CPU、微码和内核。
> 请使用 `user-programs/` 中的现行示例，不要将旧裸机程序加载到 UROM。

## 快速开始

在 Windows 的 PowerShell 中，从项目根目录执行：

```powershell
.\build-tools\build.ps1
```

脚本先编译内核和全部用户程序，再生成 EEPROM 程序存储镜像。

编译自己的用户 C 文件只需一行，文件可以位于项目外：

```powershell
.\build-tools\build-user.ps1 "C:\my-project\main.c"
```

脚本自动构建配套内核并取得系统调用地址，生成 `C:\my-project\build\main.hex`
及同名 `.s/.o/.bin/.program.json`，不用登记到示例编译清单。
要从系统终端启动它，再将 HEX 路径加入 `storage/programs.json` 并运行
`./build-tools/pack-storage.ps1`；详见 [存储和加载](docs/kernel/storage.md)。

不需要安装 LLVM、Visual Studio、CMake、Ninja 或 customasm。
需要 Windows 10/11 x64 和 PowerShell；终端执行策略如果禁止本地脚本，
可以用 `powershell -NoProfile -ExecutionPolicy Bypass -File .\build-tools\build.ps1`。

1. 在电路中把 `kernel/build/kernel.hex` 加载到内核 ROM，复位 PC 为 `0`。
2. 将 `storage/build/storage.hex` 加载到 `0x0B000000` 段 EEPROM 的本地偏移 0。
3. UROM 基址仍为 `0x80000000`，可以留空；接通**仅内核态允许**的写入。内核从 EEPROM 加载选中的程序。
   当前内核用 LDMA 搬运代码，需要接好 `0x0C000000..0x0C000004` 的 origin/target/len/stat/sum。
   普通 `run` 用 LDMA 的 UROM 读回 SUM 验证代码，不再由 CPU 扫描代码源和目标；`quickrun` 跳过校验和比较，详见 [装载行为](docs/kernel/storage.md)。
4. 将 `micro-code/kernel/micro_code.hex`、`micro-code/user/micro_code_user.hex`、
   `micro-code/hard-int/micro_code_int.hex` 分别用于对应控制单元。
5. 复位后出现 `aurora:/ > `。输入 `ls`、`run calculator`、`snake`、`maze`、`gomoku` 等。
6. 程序正常退出后回到系统终端；用户保护异常显示蓝屏并停机；`halt` 刷黑屏后停机。

新版 `INT 0x00000002` 指向 ROM 中固定的 JMP 跳板，不是 IVT。
只要 ABI/地址约定保持兼容，内核内部函数移动不再要求重编用户程序。
本次之前使用动态入口的旧用户 HEX 必须重新编译。

## 目录

| 路径 | 内容 |
|---|---|
| `aurora-32.dig` | 主电路 |
| `micro-code/kernel/`、`micro-code/user/`、`micro-code/hard-int/` | 主/用户/硬中断微码项目及导出文件 |
| `kernel/` | 内核源文件、构建入口、内核自己的 `build/` |
| `storage/` | 程序清单、EEPROM 镜像及布局报告 `build/storage.hex/.json` |
| `user-programs/include/` | 系统调用 ABI、用户回调封装、`system.io` |
| `user-programs/programs/` | 计算器、五子棋、猜数字、贪吃蛇、迷宫、俄罗斯方块、编辑器、编译器及各自 `build/` |
| `deprecated-bare-metal/` | **不受支持、已弃用**的裸机源码归档；旧编译产物已清理 |
| `build-tools/` | 可直接用于本项目的编译工具、头文件、运行库、脚本 |
| `docs/user_program/` | 每个用户示例的详细 README |
| `docs/kernel/` | 内核结构、调度、回调、异常、系统调用 |
| `docs/aurora/` | 架构、地址、ABI、指令、编译器约定和移植注意事项 |
| `docs/build-tools/` | 工具清单、构建链路、更新与验证方法 |

## 文档入口

- [用户程序总览](docs/user_program/README.md)
- [计算器](docs/user_program/calculator/README.md)
- [屏幕版 5×5 五子棋](docs/user_program/gomoku-screen/README.md)
- [猜数字](docs/user_program/guess-number/README.md)
- [俄罗斯方块](docs/user_program/tetris/README.md)
- [文本查看与编辑](docs/user_program/editor/README.md)
- [机内 Tiny C 编译器与自举](docs/user_program/compiler/README.md)
- [本地/远程系统终端与 kill](docs/kernel/remote-console.md)
- [屏幕版贪吃蛇](docs/user_program/snake-screen/README.md)
- [屏幕版迷宫](docs/user_program/maze-screen/README.md)
- [内核说明](docs/kernel/README.md)
- [Aurora 架构和编程约定](docs/aurora/README.md)
- [指令速查](docs/aurora/instruction-set.md)
- [自包含构建工具](docs/build-tools/README.md)
- [英文目录迁移对照](docs/aurora/directory-migration.md)

完整软件回归：`powershell -NoProfile -File tests/run-regression.ps1`（另需 Python）。
它执行编译产物的指令/设备模型测试，不替代 Digital 电路时序验证；详见
[验证记录](docs/build-tools/verification.md)。正常构建不需要 Python。

## 在 Aurora-32 上编写与编译程序

`run editor` 编辑文本源码；`run compiler` 输入 `tinyc/arrays.c` 和新输出名 `sorted`，
返回系统终端后 `run sorted`。`tinyc/` 中提供未编译的入门、键盘、递归、数组和指针示例。
编译 `tinyc/compiler.c` 得到 `compiler2`，再运行 compiler2 编译同一源码得到 compiler3，
即完成机内自举。它们都是用户态原生程序，不是解释执行；不能把完整 ISO C 或现有游戏源码
直接交给 Tiny C。源文件、容量与运行库限制见 [详细说明](docs/user_program/compiler/README.md)。

自举回归：`pypy3 tests/compiler/run_selfhost.py`（也可用 Python，但耗时更长）。
全套测试可用 `tests/run-regression.ps1 -Python pypy3 -SelfHost`。

## 范围和限制

目前 SYSTEM 同时服务本地/远程两个终端，各支持一个前台和一个后台用户程序（共四槽）。
后台用 `NAME &` 或 `run NAME &`；`ps` 查看槽位、银行、名称与状态，`kill SLOT` 终止指定槽。
显存与 MIDI 各由一个进程独占。支持 ASFS2 子目录、文本编辑和银行隔离的固定逻辑地址程序加载。
硬件要求和限制见 [多进程与用户银行](docs/kernel/user-banks.md)。
`quickrun` 可跳过校验和结果比较（硬件仍读回），`editor` 提供文本查看及行编辑；
`mv`/`rm` 只修改目录元数据，创建文件复用空洞。详见 [ASFS2](docs/kernel/filesystem-v2.md)。
没有文件系统日志、断电事务保证、多个用户进程、MMU、重定位或通用 libc。
程序正常退出回到所属端的 SYSTEM；另一端可执行系统命令或 kill，但不能启动第二个用户程序。
用户保护异常及系统异常蓝屏停机。UROM 地址段现使用断电归零的 RAM，B 段仍为存储 EEPROM。
`deprecated-bare-metal/` 已弃用，里面的程序是历史内核态裸机程序，不能当作 UROM 用户程序运行。
本文档描述软件当前需要的硬件契约，不表示所有异常电路时序均已由软件测试覆盖。
