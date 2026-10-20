# Terminal Snake

用户态终端贪吃蛇。完整玩法、优化原理、构建与测试说明见
[docs/user_program/snake/README.md](../../../docs/user_program/snake/README.md)。

在项目根目录运行：

```powershell
.\build-tools\build.ps1 -Program snake
```

加载配套的 `kernel/build/kernel.hex` 到 ROM，`build/snake.hex` 到 UROM。
按空格开始，WASD 转向，P 暂停，+/- 调速，Q 退出。
