# 英文目录迁移对照

项目外层从 `F:\32位计算机 - 指令集扩展` 改为 `F:\aurora-32`。
构建脚本内部不依赖这个盘符或绝对位置，整个新目录还可以再次搬动。

| 原相对目录 | 新相对目录 |
|---|---|
| 系统内核 | kernel |
| 用户程序 | user-programs |
| 用户程序/程序 | user-programs/programs |
| 裸机程序 | deprecated-bare-metal |
| 裸机程序/asm/猜数字 | deprecated-bare-metal/asm/guess-number |
| 裸机程序/asm/斐波那契数列 | deprecated-bare-metal/asm/fibonacci |
| 裸机程序/asm/计算器 | deprecated-bare-metal/asm/calculator |
| 裸机程序/asm/简易telnet网络双向通信 | deprecated-bare-metal/asm/telnet-chat |
| 裸机程序/asm/冒泡排序 | deprecated-bare-metal/asm/bubble-sort |
| 裸机程序/asm/顺序填充 | deprecated-bare-metal/asm/sequential-fill |
| 裸机程序/c/计算器 | deprecated-bare-metal/c/calculator |
| 裸机程序/c/乱码打印 | deprecated-bare-metal/c/character-output |
| 裸机程序/c/冒泡排序升级 | deprecated-bare-metal/c/bubble-sort |

原有英文目录和 `.git/.vscode` 名称保留。部分旧汇编文件和主电路文件名仍是中文；
此次英文化对象是目录，不改变这些文件里的机器码或电路内容。
根目录旧 `指令集.md` 归档至 `docs/aurora/legacy-instruction-set.md`，有明确历史警告。
各程序原 README 改为指向集中说明的简短入口，详细内容在 `docs/` 维护。

外部快捷方式、编辑器收藏、脚本里自写的绝对路径需要改为新路径；项目自身的
构建入口和内核相对 include 已更新。主电路中原有 `F:\rom_data.hex` 外部引用保持原样，
它不是编译器依赖，也不是本次创建的 ROM 镜像路径；加载程序时应明确选择所需 HEX。
电路/微码里的实际器件数据不会因为目录改名而自动变成新构建产物。

`deprecated-bare-metal/asm` 仅保留旧 customasm 规则和历史源码，明确标为不受支持、已弃用；旧产物已清理。
新 bundled 工具链的保证范围是内核和当前用户 C 程序；不承诺直接替代所有历史
customasm 方言的汇编命令。裸机程序若在有 tick 的电路上运行，需要其自己的 ISR。
