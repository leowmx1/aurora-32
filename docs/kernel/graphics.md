# 图形系统调用：矩形、双色位图、透明位图（轮询版）

当前硬件已移除屏幕完成中断。内核不注册 0x03000003，不生成图形完成事件，
不保留依赖 IRQ 才清除的在途标记。仅根据设备 0x09900000 的 busy 状态判断完成。

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

屏幕 480×240，左上 (0,0)，矩形覆盖 [x,x+w)、[y,y+h)。
宽高非零，整个矩形须在屏幕内，不自动裁剪。负数转 unsigned 后也会拒绝；
采用减法验证边界以避免加法溢出。用户封装先验证，内核独立验证原始调用。

颜色原样写设备，不解释为 RGB、不做调色板范围转换；已验证 2 红、3 绿、4 蓝。
填充不自动清屏、不重画旧矩形，其他区域保持不变。

| r0 请求 | r1 | r2 | r3 | 返回 r0 |
|---|---|---|---|---|
| 19 GRAPHICS_FILL_RECT (旧 ABI) | x+256*y | w+256*h | color | 0 接受，-1 非法，-5 忙 |
| 37 GRAPHICS_RECT16 | x+65536*y | w+65536*h | color | 0 接受，-1 非法，-5 忙 |
| 38 GRAPHICS_IMAGE | 八字图像描述符 | 1 双色 / 2 透明 | 必须 0 | 同 28 |
| 20 GRAPHICS_BUSY | 0 | 0 | 0 | 0 空闲，1 忙 |
| 28 GRAPHICS_BITMAP | 八字描述符地址 | 1 双色 / 2 透明 | 必须 0 | 0 接受，-1 非法，-4 地址错误，-5 忙 |

新 FillRect 使用调用 37，两个打包字各含 unsigned 16 位字段，支持整个 480×240 屏幕。
旧调用 19 保留 unsigned 8 位打包格式（每个参数字 <=65535），供已有 ABI-3 文件使用。
调用 28 的两字内联位图也保留；旧程序不自动缩放，重新编译才获得新布局。
内核解包显式使用 AND/DIV，规避现有后端对 16 位常量移位的窄加载合法化问题；
本次没有修改通用编译器后端。

### 位图描述符与位排列

`aurora_graphics_bitmap` 是八个连续 32 位字，顺序为
`x,y,width,height,color,background,data_low,data_high`。必须放在 static/global 存储中，
不能传用户栈局部变量。内核只接受完整落在同一 UROM 或 URAM 段内的八字描述符；
用户栈、内核区、MMIO、跨段或越界地址均返回 EFAULT。
编译器可能把 static const 数据初始化到 URAM，不应依赖 const 一定放在 UROM。

宽高非零、整个区域在 480×240 内，且 `width*height <= 64`。没有缩放、重复平铺或自动裁剪。
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
| 0x09100000 | y |
| 0x09200000 | width |
| 0x09300000 | height |
| 0x09400000 | color |
| 0x09500000 | background color |
| 0x09600000+n | data RAM[n]，每字 32 个连续像素位 |
| 0x09700000 | 保留，不使用 |
| 0x09800000 | mode：0 矩形、1 双色位图、2 透明背景位图 |
| 0x09900000 | 读 busy；写 0 停止、写 1 启动；完成后硬件自动清零 |

图形设备占用 0x09 段，地址 [23:20] 选择寄存器，低 20 位为内部地址。
data RAM[0/1/2] 分别为 0x09600000/0x09600001/0x09600002。
CPU 使用字地址，不能乘 4。原先低 4 位译码和 data_high 硬件协议不再兼容，
必须加载配套的新内核。MIDI 保持 0x0A000000+N。

内核在 INT 清 IE 的短临界区内验证参数、读取状态；忙则立即返回 EBUSY，不写任何
参数。矩形空闲才依次写 stop、x/y/w/h/color、mode=0、start（八次写入），
旧位图写 stop、六个参数、data RAM 两字、mode、start（11 次写入）；
新 Image 则写六个参数和 N 个连续数据字，共 N+9 次写入。
每次矩形显式重设 mode，避免沿用之前的位图模式；随后 IRET，不等待像素填充。
查询 Busy 只读状态，无写入或应答副作用。无需观察到 busy=1 才能认为 0 表示完成，
因此极快或极小矩形也可以立即完成。

四用户槽之间显存独占，首次成功绘图的进程拥有显存直到退出/kill；其他进程的图形调用返回
EPERM。系统绘制图标时临时保留设备，返回 EBUSY；有用户所有者时不覆盖其画面。
没有绘图命令队列、任务 ID 或用户取消接口。Busy 是全局设备状态，
不是某个调用者的完成凭据。若前台和键盘回调同时绘图，须自行明确顺序，
不能把 Busy==0 当作下次提交必然成功；必须检查 FillRect 的返回值。
各程序需自行串行化绘图：例如贪吃蛇只在前台画图，五子棋/迷宫在非嵌套键盘回调内画图。

## 构建与验证

正式图形示例为 snake-screen、maze-screen、gomoku-screen 和 tetris。
执行 `./build-tools/build.ps1` 后加载配套 kernel.hex 与 storage.hex，从系统终端启动。
原 graphics-demo 项目已移除，矩形/位图参考画面的最小测试源保留于
`tests/graphics-syscall/render_fixture.c`，不打包成用户示例。
固定 INT 跳板位于 ROM 字 2，内部函数布局变化不改变这个地址。

tests/graphics-syscall 执行实际 BIN，并在完全不产生图形 IRQ 的设备模型中验证
立即/延迟完成、连续绘制、边界、忙时拒绝、键盘回调和 tick、卡忙超时。
测试还断言内核未注册图形向量，图形等待不调用 WaitEvent。
这是理想指令/设备模型，不是 Digital 电路时序验证。

`tests/graphics-syscall/run_bitmap_syscall.py` 另外验证真实 BIN 的位图描述符检查、
64 位数据顺序、bit31/32、5×7 行宽、透明零位保留、矩形模式复位、数据快照、
立即/延迟完成、13 次键盘回调现场恢复、tick 和卡忙超时。

## 大位图接口（调用 38）

`aurora_graphics_image` 的八个连续字为
`x,y,width,height,color,background,data,words`。
`system.graphics.Image(&image)` 为双色；`ImageTransparent(&image)` 为透明。
`data` 指向 static/global 的 32 位字数组；`words` 必须恰好等于
`(width*height+31)/32`，不是容量上限。最大全屏需要 3600 字。

第 i 个像素读取 data[i/32] 的 bit(i%32)，i=dy*width+dx。
行末不补齐字；例如宽 65 时，第二行从第 65 位继续。每个命令从 data RAM[0] 读取。
描述符及完整数据区都必须位于同一用户银行的 UROM 或 URAM 范围；
栈、内核、MMIO、跨银行末尾地址返回 EFAULT。内核按当前进程银行分别转换两个指针。
宽高/坐标/字数错误返回 EINVAL。忙或权限错误时不写任何图形参数。

INT 内先完成全部验证，再在关中断区上传数据并启动；返回后硬件异步画图。
上传期间不能抢占，以防银行切换或其他进程改写共享 data RAM。
全屏 3600 字上传会增加中断延迟，建议大文本/图片按小块或条带提交。
这不是 DMA，也不在内核等待像素绘制完成。
成功返回后调用者可复用描述符和原数组；下一次图形命令仍须先等 Busy。

## 480×240 排版与字号

- HUD、坐标和一般提示：12×16 字符格，常规细字形。
- Tetris 标题：18×24；系统 Aurora-32 图标：24×32，居中显示。
- 字模离线栅格化并预打包为 1bpp，不放大旧 3×5 字形、不在 CPU 上逐像素缩放。
  参考生成器为 tests/graphics-syscall/rasterize_font.py；目标端不需要字体文件或 Pillow。
- 贪吃蛇、迷宫、五子棋保留 160×100 逻辑坐标，按 12/5 等比例扩展到
  居中的 384×240 视口。矩形两端分别取整，避免拼接缝。
- Tetris 独立整数布局：11×11 格，10×10 填色，1 像素间隔；110×220 游戏区。
- Digital 窗口应保持内容区 2:1，才能让 480×240 的逻辑像素显示为正方形。
  拉伸窗口导致的物理变形不属于图形寻址错误。
- 大区域比旧分辨率需要更多像素写入；仍保持局部重绘，不承诺实际绘图时间不增长。

新增 run_image_syscall.py 检查跨 32 位/跨行的大位图、透明零位保留、
描述符/数据快照与拒绝非法范围；run_layout.py 检查正式游戏 BIN 的分级字模和退出。
