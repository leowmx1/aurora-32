# Screen Maze

160×100 屏幕迷宫，构建目标 `maze-screen`。原终端迷宫 `maze` 已移除。

```powershell
.\build-tools\build.ps1 -Program maze-screen
```

ROM 加载配套 `kernel/build/kernel.hex`，UROM 加载 `build/maze-screen.hex`。
不需要屏幕完成 IRQ，设备 busy 完成后自动清零即可。
文字使用预计算 6×10 双色位图，每字符一次提交并覆盖旧字符背景；地图仍按矩形局部更新。
需使用支持位图请求 28 的配套内核，原有字体尺寸、字距和颜色不变。

深青灰墙、米色通道，珊瑚色实心块是玩家、空心框是出口，深青灰小点是起点。
WASD 移动，R 回到起点，N 新随机地图，Q 退出。顶部 S 是步数，T 是秒数，均显示末三位。
启动时终端仅打印一次操作帮助；游戏期间（包括 N/R）不输出。
通关或 Q 退出时显示结果、当前这局的完整步数和用时；错误退出保留诊断。

完整说明见 [屏幕版迷宫](../../../docs/user_program/maze-screen/README.md)。
