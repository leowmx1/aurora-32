# Aurora-32 用户程序

本目录保存用户态库、程序源码和每个程序自己的构建产物。

## 目录

```text
include/
  aurora_syscall.h       内核/用户共享 ABI
  aurora_user.h          INT 封装、键盘订阅与回调返回跳板
  aurora_system.h        system.io 高层接口

程序/
  calculator/            中断回调驱动的表达式求值器
  hello-loop/            循环输出和调度测试
  keyboard-client/       键盘硬件中断回调测试
  system-io-demo/        system.io 接口示例
```

每个程序的 `.s/.o/.bin/.hex` 均位于它自己的 `build/` 子目录。计算器还会
生成带 IRQ 序号跟踪的 `calculator-trace.hex`。

## 构建

先构建同级的系统内核，再在本目录执行：

```powershell
.\build.ps1
```

脚本从 `../系统内核/build/kernel.o` 查找 `aurora_syscall_entry`，把实际 ROM
字地址编译进所有用户程序的 `INT` 指令。脚本同时验证每个用户二进制都
以 UROM 入口跳转开始，且目标落在 `0x08000000-0x08ffffff`。用户构建只
暴露非特权 `aurora` API，并拒绝任何含有 `EI`、`DI`、`HALT`、`IRET`、
`PUSHU` 或 `POPU` 的生成汇编。系统调用只需执行 `INT`；该指令在进入
内核时由硬件自动清除 IE。
