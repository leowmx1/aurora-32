# 用户态图形示例

源码、构建与预期画面见 [graphics-demo](../../../user-programs/programs/graphics-demo/README.md)。
当前使用 Busy/Yield 轮询完成，不需要屏幕完成中断、不使用图形回调或 WaitEvent。
系统调用、错误码、并发与未来完成中断约定见 [graphics](../../kernel/graphics.md)。

这是常规内核 + UROM 用户程序，不是裸机测试。依次绘制中央蓝色、左上红色、
右下绿色矩形，不清除其他区域；中央绘制黄色背景红 F 和透明背景红 F，
再画绿色短条验证切回矩形模式。所有绘制完成后打印成功提示并以退出码 0 结束。
