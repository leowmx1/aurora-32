# Scale：单次上传大字母，硬件抽样缩小

把 [build/scale_test.hex](build/scale_test.hex) 单独加载到 **ROM**，复位从 PC=0 开始。
不需要正式 kernel.hex、storage 或用户程序。全程内核态，先初始化 KSP/KFP，再 DI；
不启用 tick/键盘中断。结束后 NOP 空转保留画面，不停机、不重复绘制。

## 协议

- 480×240，原图形地址不变；原 high 地址 **0x09700000** 现在是 scale。
- scale=0 等效 1，正整数 s 表示水平/垂直均每隔 s 个源像素抽样。
- **Grw/Grh 写输出尺寸**。源行宽为 `Grw*max(scale,1)`。
- 本测试源为 48×72，所有测试缩放因子都整除宽高，不假设无法整除时的边缘规则。
- 对输出 `(dx,dy)`，源位索引为 `(dy*s)*(Grw*s)+dx*s`，s=max(scale,1)。
- data RAM 从 0x09600000 开始，一字 32 位、bit0 在先、行间无补齐。
- mode1：0 位写背景、1 位写前景；mode2：0 位不写、1 位写前景。
- stat=0x09900000，写 1 开始、读非零忙、完成读 0；忙时不改寄存器或 RAM。

## 预期画面

从左到右六列，scale 为 **0、1、2、3、4、6**：

| scale | 输出宽×高 |
|---|---|
| 0 / 1 | 48×72（两列必须完全相同） |
| 2 | 24×36 |
| 3 | 16×24 |
| 4 | 12×18 |
| 6 | 8×12 |

上排 y=24：白底黑字，mode1。下排 y=136：红字叠在绿/黄双色底板，mode2。
下排字母的孔和外部背景应保留底板色，不应被背景寄存器颜色覆盖。
参考图在 build/preview.png（2 倍显示）、build/preview-native.png（原生像素）。

只上传一份最大的 A，共 108 个数据字，12 次字母绘制重复使用同一 RAM。
CPU 不进行缩放，没有小字号数据表，不在列之间重新上传字模。
源字形离线生成；硬件抽样不是抗锯齿，极小尺寸时细笔画可能变细或丢失，这也是观察项。
终端输出列顺序与完成提示，屏幕没有另外上传标签字模。

## 安全和构建

每次绘制前后等待设备空闲；10 秒超时后只输出错误并空转，不强制取消或继续写入。
启动时若已有设备任务也会等待，不能把此测试当作热替换正在运行内核的方式。
正常完成后把 scale 写回 1；正式内核/库本次未修改，测试确认后再考虑接入缩放 API。

在 F:\aurora-32 执行：

```powershell
.\build-tools\compile.ps1 tests/graphics-scale/scale_test.c tests/graphics-scale/build/scale_test.hex -Mode kernel
python tests/graphics-scale/run_scale.py --preview
```

run_scale.py 执行实际编译 BIN，检查一次上传、完整像素、0=1、两种模式、数据步长、
立即/延迟完成、timer 回绕、忙超时和完成后不重画。它是理想 ISA/设备模型，
**不能代替 Digital 微码/电路时序测试**。
generate_glyph.py 可重新输出字模头文件（--header）或独立像素参考 JSON；
使用本机 Consolas Regular 和 Pillow 离线生成，目标机不需要字体文件。
