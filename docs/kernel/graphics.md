# 图形系统调用：矩形、双色位图、透明位图（轮询版）

当前硬件已移除屏幕完成中断。内核不注册 0x03000003，不生成图形完成事件，
不保留依赖 IRQ 才清除的在途标记。仅根据设备 0x09000009 的 busy 状态判断完成。

## 用户接口

- `system.graphics.FillRect(x,y,width,height,color)`：异步提交；0 表示已启动，
  -5（EBUSY）表示设备忙，-1（EINVAL）表示矩形非法。成功不代表画完。
- `system.graphics.Busy()`：0 空闲、1 忙；用户需处理负的系统调用错误。
- `system.graphics.Bitmap(&bitmap)`：位为 1 写前景色，为 0 写背景色。
- `system.graphics.BitmapTransparent(&bitmap)`：位为 1 写前景色，为 0 保留原像素。
  两者均异步提交；0 接受、EINVAL=-1 参数错误、EFAULT=-4 描述符地址错误、EBUSY=-5 忙。
- 图形 Subscribe 接口已删除。旧调用号 21 保留不用，直接调用返回 ENOSYS=-3。
  ABI_VERSION 仍为 3；必须使用重新构建的配套 ROM/UROM。

等待时查询 Busy，并在两次查询间 Yield。不要用 WaitEvent 等图形完成：
屏幕不会发出事件，无法唤醒等待进程。键盘/Telnet 的回调和 WaitEvent 功能不变。

```c
aurora_status_t result = system.graphics.FillRect(40, 25, 80, 50, 4);
if (result != AURORA_OK) system.Exit(1);
aurora_word_t start = system.TimeMs();
for (;;) {
  aurora_status_t busy = system.graphics.Busy();
  if (busy < 0) system.Exit(1);
  if (busy == 0) break;
  if (system.TimeMs() - start >= 10000u) system.Exit(1);
  system.Yield();
}
```

Yield 让出 CPU 后用户仍可运行，不是阻塞睡眠；内核空闲时可能很快切回用户。
实际轮询发生在用户程序，每次系统调用只查询一次，不在关中断区等待整个填充。
硬件绘制期间 tick、键盘和 Telnet 仍可运行。超时只结束等待，不会取消硬件任务。
最后一个矩形必须画完再 Exit，避免系统停机时硬件尚未写完。

## 参数和寄存器 ABI

屏幕 160×100，左上 (0,0)，矩形覆盖 [x,x+w)、[y,y+h)。
宽高非零，整个矩形须在屏幕内，不自动裁剪。负数转 unsigned 后也会拒绝；
采用减法验证边界以避免加法溢出。用户封装先验证，内核独立验证原始调用。

颜色原样写设备，不解释为 RGB、不做调色板范围转换；已验证 2 红、3 绿、4 蓝。
填充不自动清屏、不重画旧矩形，其他区域保持不变。

| r0 请求 | r1 | r2 | r3 | 返回 r0 |
|---|---|---|---|---|
| 19 GRAPHICS_FILL_RECT | x+256*y | w+256*h | color | 0 接受，-1 非法，-5 忙 |
| 20 GRAPHICS_BUSY | 0 | 0 | 0 | 0 空闲，1 忙 |
| 28 GRAPHICS_BITMAP | 八字描述符地址 | 1 双色 / 2 透明 | 必须 0 | 0 接受，-1 非法，-4 地址错误，-5 忙 |

矩形接口的两个打包字必须 <=65535，每字段 unsigned 8 位，不传用户栈描述符或共享临时参数
缓冲区。分辨率超出字段范围时需扩展 ABI，不能截断。
内核低字节解包使用显式 AND，规避现有编译器窄整数合法化可能漏掉常量掩码的问题；
本轮未修改或声称修复通用编译器后端。

### 位图描述符与位排列

`aurora_graphics_bitmap` 是八个连续 32 位字，顺序为
`x,y,width,height,color,background,data_low,data_high`。必须放在 static/global 存储中，
不能传用户栈局部变量。内核只接受完整落在同一 UROM 或 URAM 段内的八字描述符；
用户栈、内核区、MMIO、跨段或越界地址均返回 EFAULT。
编译器可能把 static const 数据初始化到 URAM，不应依赖 const 一定放在 UROM。

宽高非零、整个区域在 160×100 内，且 `width*height <= 64`。没有缩放、重复平铺或自动裁剪。
局部 `(dx,dy)` 对应 `i=dy*width+dx`：i<32 取 data_low 的 bit i，否则取 data_high
的 bit i-32。bit 0 是左上角；无须强制宽度为 8，超出有效面积的数据位被忽略。
背景色在透明模式下不参与写像素。所有颜色值原样传入硬件。

```c
static const aurora_graphics_bitmap letter_f = {
    56, 40, 8, 8,       // x, y, width, height
    2, 5,               // red foreground, yellow background
    0x1f01013fu, 0x00010101u
};

// Call after the previous operation completes; still check EBUSY on submit.
aurora_status_t result = system.graphics.Bitmap(&letter_f);
// Or: system.graphics.BitmapTransparent(&letter_f);
// Check result, then use the Busy/TimeMs/Yield loop above before next draw/Exit.
```

内核在返回前复制并提交全部数据，不保存用户指针，因此成功返回后可复用/修改描述符。
硬件此时可能仍在绘制，下一次提交仍需等 Busy。内核验证失败或设备忙时不写任何图形寄存器。
若主循环和回调共用可变描述符，应自行用 DeferEvents/ResumeEvents 保护构造过程，或各用独立描述符；
内核只能保证 INT 内的读取/提交原子性，不能保护用户尚未填完的数据。

## 设备协议与并发

| CPU 字地址 | 用途 |
|---|---|
| 0x09000000 | x |
| 0x09000001 | y |
| 0x09000002 | width |
| 0x09000003 | height |
| 0x09000004 | color |
| 0x09000005 | background color |
| 0x09000006 | data_low，bit 0..31 |
| 0x09000007 | data_high，bit 32..63 |
| 0x09000008 | mode：0 矩形、1 双色位图、2 透明背景位图 |
| 0x09000009 | 读 busy；写 0 停止、写 1 启动；完成后硬件自动清零 |

图形设备只占 `0x09` 段，低 4 位选择十个寄存器。以上均为 CPU 字地址，偏移不乘 4。
不访问偏移 A..F。旧版 status=+7 的协议已不兼容；现在 +7 是位图高字。
MIDI 已迁往 `0x0A000000 + N`，不能再把该地址当图形 Y 寄存器。
旧图形 0x0A..0x0E 和旧 MIDI 0x0F 地址不兼容，必须同时更新硬件译码和配套镜像。

内核在 INT 清 IE 的短临界区内验证参数、读取状态；忙则立即返回 EBUSY，不写任何
参数。矩形空闲才依次写 stop、x/y/w/h/color、mode=0、start（八次写入），
位图依次写 stop、八字描述符、mode=1/2、start（十一次写入）。
每次矩形显式重设 mode，避免沿用之前的位图模式；随后 IRET，不等待像素填充。
查询 Busy 只读状态，无写入或应答副作用。无需观察到 busy=1 才能认为 0 表示完成，
因此极快或极小矩形也可以立即完成。

只有一个用户进程，没有绘图命令队列、任务 ID 或取消接口。Busy 是全局设备状态，
不是某个调用者的完成凭据。若前台和键盘回调同时绘图，须自行明确顺序，
不能把 Busy==0 当作下次提交必然成功；必须检查 FillRect 的返回值。
各程序需自行串行化绘图：例如贪吃蛇只在前台画图，五子棋/迷宫在非嵌套键盘回调内画图。

## 构建与验证

`./build-tools/build.ps1 -Program graphics-demo`。
加载 kernel/build/kernel.hex 到 ROM，user-programs/programs/graphics-demo/build/graphics-demo.hex
到 UROM。内核变化会改变 INT 的直接入口地址，其他用户镜像也必须重新构建。

示例不清屏，保持蓝色中央、红色左上、绿色右下三个矩形，中央增加双色/透明红色 F，
最后在左 F 下画绿色短条验证切回矩形模式，完成后退出。
旧显存内容可能保留，可先清空/复位显存。

tests/graphics-syscall 执行实际 BIN，并在完全不产生图形 IRQ 的设备模型中验证
立即/延迟完成、连续绘制、边界、忙时拒绝、键盘回调和 tick、卡忙超时。
测试还断言内核未注册图形向量，图形等待不调用 WaitEvent。
这是理想指令/设备模型，不是 Digital 电路时序验证。

`tests/graphics-syscall/run_bitmap_syscall.py` 另外验证真实 BIN 的位图描述符检查、
64 位数据顺序、bit31/32、5×7 行宽、透明零位保留、矩形模式复位、数据快照、
立即/延迟完成、13 次键盘回调现场恢复、tick 和卡忙超时。
