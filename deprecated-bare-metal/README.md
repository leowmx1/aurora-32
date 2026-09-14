# DEPRECATED / UNSUPPORTED — 裸机程序已弃用、不受支持

> **此目录不是可运行示例集，不属于当前 Aurora-32 的支持范围。**
> 仅保留历史源码供阅读和移植参考；不提供构建、运行、微码兼容或问题修复保证。
> 现行示例请使用 [user-programs](../docs/user_program/README.md)，
> 现行系统请使用 [kernel](../docs/kernel/README.md)。

## 为什么弃用

这些程序源自没有完整用户/内核状态隔离、没有现行系统调用 ABI 或 tick 调度的早期设计。
它们可能直接访问终端、键盘、IVT 和硬件栈，依赖旧地址、旧微码或旧 customasm 规则。
部分文件后来补过空中断处理，但这不表示它们已经适配现行 CPU。

在当前硬件上直接运行，可能出现未注册中断、错误返回现场、错误访存、PC 跑飞或停机。
**不要把它们作为 UROM 用户程序加载，也不要据此更改现行系统的地址和 ABI。**
“源码仍在”仅表示保留历史参考，不代表维护承诺。

## 保留内容

| 目录 | 历史用途 |
|---|---|
| `asm/bubble-sort/` | 汇编冒泡排序 |
| `asm/calculator/` | 汇编计算器 |
| `asm/fibonacci/` | 斐波那契数列 |
| `asm/guess-number/` | 裸机汇编猜数字 |
| `asm/sequential-fill/` | 顺序填充 |
| `asm/telnet-chat/` | 早期 Telnet 双向通信 |
| `c/bubble-sort/` | 裸机 C 排序 |
| `c/calculator/` | 裸机 C 计算器 |
| `c/character-output/` | 早期字符输出实验 |
| `asm/customasm_rule.asm` | 历史规则，不是当前指令集的权威定义 |

旧 `.hex/.bin/.o` 及 C 编译生成的 `.s` 已从项目中清理，避免误加载。
手写 `.asm` 和 `.c` 源文件完整保留。项目内 `build-tools/build.ps1` 不构建此目录；
本目录没有受支持的重建命令，也不保证当前编译器可重现旧二进制。

## 如需移植

应在 `user-programs/programs/` 新建用户程序，显式声明 `AURORA_USER_PROGRAM`，
按现行启动约定初始化用户栈，通过 `system.io` 访问终端，订阅内核键盘回调，
使用软件数组维护计算数据栈，并经 `system.Exit` 退出。
不能只改代码基址，就假定直接 MMIO 和特权指令能在用户态工作。
参考 [Aurora 编程约定](../docs/aurora/README.md) 和现行用户示例。
