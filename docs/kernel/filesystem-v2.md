# ASFS2：目录、文本、文件接口与 quickrun

更新：2026-10-30。ASFS2 支持最多 64 个目录槽（包含目录），可执行文件仍使用 AEXE1，
现有 ABI3 调用不变，增加 30–34 号系统调用。旧 ASFS1 镜像仍可只读挂载和运行。

## 硬件与构建

storage 为 `0x0B000000`–`0x0BFFFFFF`，2^24 个 32 位字（64 MiB）。
内核可读写，用户态不可直接访问；写入必须具有 RAM 一样的总线完成语义。
每次写入立即读回检查，未接 WE 或读回不符则返回 EIO 并使挂载失效，需要 `mount` 重试。
这不是 EEPROM 延时编程驱动，没有 busy/ready 握手或写入重试。

`.\build-tools\build.ps1` 重建内核、九个用户构建目标以及存储镜像。
ROM 加载 `kernel/build/kernel.hex`，EEPROM 加载 `storage/build/storage.hex`。
**第一次从旧版升级需要同时更新内核和存储镜像。** 不需要手动加载 UROM。
当前内核超过 32,768 字，ROM 至少配置 65,536 字；当前示例 UROM 至少 16,384 字。
重新打包前应备份 Digital 中的 EEPROM，打包器不会保留运行时编辑。

## 装载策略

| 命令 | 结构、范围、ABI、入口检查 | 全文件校验和比较 | 拷贝 | 目标验证 |
|---|---|---|---|---|
| `run PATH` / 直接输入可执行路径 | 是 | 是 | LDMA 代码 / CPU 数据 | UROM 读回 SUM；URAM 逐字读回 |
| `quickrun PATH` | 是 | **跳过** | LDMA 代码 / CPU 数据 | **跳过结果检查** |
| `verify PATH` | 可执行文件检查全部结构 | 是 | 否 | 否 |

`quickrun` 用于已经确认正确、可写 UROM 正常的可信镜像，进度只显示 Copy。
LDMA 的硬件读回仍执行，但不比较 SUM；损坏内容或 UROM 写入失败可能导致用户程序崩溃。
它不能跳过地址边界，否则目录损坏会让加载器写坏内核。文本和目录一律不能作为程序运行。
2026-11-01 起普通 run 不再执行独立 Check 和 CPU UROM 扫描；详细 SUM 契约见 [装载行为](storage.md#ldma-装载行为)。

## 系统终端文件管理

| 命令 | 行为 |
|---|---|
| `ls [PATH]` / `programs [PATH]` | 列出指定目录的直接子项，显示 NAME、TYPE、SIZE |
| `tree` | 从根目录迭代遍历，显示缩进目录树 |
| `cat PATH` | 校验后显示文本，不输出二进制可执行内容 |
| `stat PATH` | 类型、地址、长度、预留容量；可执行项另显示 AEXE 字段 |
| `mkdir PATH` | 创建一个目录；父目录必须存在，不隐式创建父目录 |
| `touch PATH` | 创建空文本，预留 2048 字；同名已存在时报错，不截断文件 |
| `rename PATH NAME` | 同目录改名，NAME 不含 `/` |
| `mv PATH DEST` | DEST 为现有目录时移入并保留名字，否则为新的完整路径 |
| `rm PATH` | 删除文件或空目录，不允许删除根目录或非空目录 |
| `df` | 设备容量、镜像高水位、实际已分配量、可分配总字数 |
| `verify PATH` / `verify all` | 校验内容；all 跳过目录和失效槽，不写 UROM |
| `mount` | 重新读取并验证整个目录 |

没有 `cd`，路径始终从根目录解析，开头 `/` 可省略；不解释 `.`、`..`、重复分隔符。
分量最长 31 个字符：小写字母、数字、`_-.`，大小写敏感；系统调用路径最长 127 字。
系统终端命令行仍限 63 字，因此较长路径可由用户程序调用接口访问。
默认不覆盖目标。目录不能移入自身或后代；允许移动整个目录，但不改变后代目录项。
shell 可管理可执行文件元数据，用户正文写入接口仅支持文本。

## 减少写入与内容搬移

- `mv` 只改父 ID 和实际变化的名字字符，最多 33 字；保留原名的跨目录移动仅写父 ID **1 字**。
  不复制正文、不改 ID/偏移/容量。
- `rename` 只改名字字段中实际变化的字，最多 32 字；目录中孩子的父 ID 不变。
- `rm` 只把类型设为 0，**一次元数据写入**；不清零旧正文、不移动后面的文件。
- 创建文件扫描至多 64 个现有区间，first-fit 使用删除留下的空洞；找不到合适空洞再使用尾部。
  空文件只记录长度 0，不必清零整块预留区。读取绝不超过逻辑长度，避免读出旧内容。
- 文本保存只写新正文、校验和和长度，不移动邻接文件。超出预留容量返回 ENOSPC，
  不自动搬移扩容。4096 字是目前单文本容量/单次保存上限。
- 没有在线压缩/碎片整理；总空闲足够但无足够连续区间，或目录槽耗尽，仍可能 ENOSPC。
- 镜像高水位不会随删除下降，`Outside image` 不等于全部空闲；`Free words` 包含内部空洞。

删除不是安全擦除，没有进程间保密隔离。没有日志或断电事务保证；逐字读回检查只发现写入失败，
无法保证断电恢复一致性。电路复位/关闭前应保存 EEPROM 镜像。正常软退出/重新运行不会丢失内容。

## ASFS2 格式

保留 ASFS1 的 16 字超级块和默认 40 字目录项，字段 1=版本 2，字段 8=格式标志 3。
头字段 3=已使用范围的高水位（含空洞/预留区），字段 4=槽高水位（含 tombstone），最多 64。
目录起点、步长、内容起点仍为字段 5、6、7。默认内容起点 4096，预留完整 64 槽目录空间。
其他头保留字段必须 0。

| 目录项偏移 | 内容 |
|---|---|
| 0 | 0=失效/已删除，1=EXE，2=TXT，3=DIR |
| 1 | 正文起点（相对 storage 的字偏移）；目录为 0 |
| 2 | 逻辑长度；目录为 0 |
| 3 | 正文前 length 个字相加模 2^32；目录为 0 |
| 4..35 | 名字，1 字 1 ASCII 字符，零结尾并补零 |
| 36 | 父目录 ID，0=根目录 |
| 37 | 预留容量；EXE=length；TXT 为 1..4096；DIR=0 |
| 38..stride-1 | 保留零 |

ID=槽索引+1，0 为虚拟根目录，根不占槽。挂载验证名字、同级重名、父项类型/范围、祖先循环、
有效正文区间的容量重叠、目录/正文边界；失效槽不参与分配。父目录可在任意槽，不要求先于子项。
删除后槽可能复用，因此 ID 不是跨会话永久标识。shell 仅在用户进程退出后管理目录，
不允许在用户运行时外部更换镜像；单次用户运行期间 ID 保持稳定。

## 打包清单

```json
{
  "Version": 2,
  "DataOffsetWords": 4096,
  "Files": [
    {"Name":"calculator","Hex":"../user-programs/programs/calculator/build/calculator.hex"},
    {"Name":"docs/notes.txt","Type":"Text","Source":"texts/notes.txt","ReserveWords":2048}
  ]
}
```

打包器自动生成路径需要的父目录，每个目录也占一个槽。路径相对清单位置。
可执行 HEX 仍需匹配 `.program.json`，与文本格式分开处理。
文本为 ASCII，每字符占一个字，不带隐式终止符；LF/TAB 允许，CRLF/CR 规范化为 LF。
文本预留空间默认 2048 字，最大 4096，不够时报错，不静默截断。内核保存也允许 CR。
目录根层保留 shell 命令名。不会把 64 MiB 全部填零，只输出到当前高水位。

## 用户系统调用和 C API

包括 `<aurora_system.h>`，使用 `system.fs`。每次调用 `r0=编号/返回值`，`r1..r3=参数`。
INT 跳板仍在 ROM 字地址 2；ABI3 扩展，不改变现有调用。

| 编号 | API | 参数与返回 |
|---|---|---|
| 30 | `List(directory, ordinal, &info)` | 直接子项序号从 0 开始；1=一项，0=结束，负数=错误 |
| 31 | `Open(path)` | 返回 ID，根为 0，负数=错误；不分配打开句柄，无需 Close |
| 32 | `Read(&request)` | packet=[id,offset,buffer,count]；count≤64，返回实际读取字数，EOF=0 |
| 33 | `Save(id, text, length)` | 替换已有 TXT 全文；length≤capacity≤4096，返回长度；长度 0 清空 |
| 34 | `Info(id, &info)` | 返回 0 成功或负数；读取类型、长度、容量、父 ID、名字 |

`aurora_fs_info` 是 37 字：id,type,words,capacity,parent,name[32]。
`aurora_fs_read_request` 是 4 字：id,offset,buffer,count，buffer 是 CPU 字地址。
所有输出缓冲区必须是 **静态/全局 URAM**；输入允许 UROM/URAM，禁止 USTACK、内核、MMIO。
先验证完整区间才输出，不接受溢出和跨设备区间。Read 可以读取 EXE 原始字，但拒绝目录。
Save 校验全部输入和容量后才开始写入；不能写 EXE 或 DIR。List/Read 分批避免长时间关中断。
Save 最多 4096 字，期间串行执行，硬件实时输入较多时仍可能造成延迟，这是第一版限制。

```c
static aurora_fs_info info;
static aurora_fs_read_request request;
static aurora_word_t buffer[64];
int id = system.fs.Open("docs/notes.txt");
if (id > 0 && system.fs.Info(id, &info) == AURORA_OK) {
    request.id = id;
    request.offset = 0;
    request.buffer = (aurora_word_t)(unsigned long)buffer;
    request.count = 64;
    int n = system.fs.Read(&request);
    if (n > 0) system.io.Write(buffer, n);
}
```

错误码：EINVAL=-1，EFAULT=-4，EBUSY=-5，ENOENT=-6，EIO=-7，ENOSPC=-8，EPERM=-9，EEXIST=-10。
这些接口不执行动态链接，不提供权限/用户账户，也不能用校验和抵御恶意软件。

双终端并发约定：SYSTEM 执行命令期间预留文件系统工作区，用户文件系统请求可能返回 EBUSY，
应稍后重试。Open/Info/List/Read/Save 暴露给当前用户的 ID 保留到进程退出或 kill；此时另一端
rm 相应条目会返回 EBUSY，避免删除后复用同 ID。rename/mv 不改变 ID，仍允许操作。
暂不提供 Close；`txt-viewer` 的 discard 仅丢弃草稿，不会解除内核的 ID 保留。
