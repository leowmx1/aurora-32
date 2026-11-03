# Text Viewer / Editor

运行于用户态，通过 `system.fs` 访问 EEPROM；不直接访问 B 段。键盘采用内核事件队列派发的
回调，主循环使用 `system.WaitEvent()` 休眠。普通编辑只改变用户内存，只有 `save` 才写 EEPROM。

## 构建与启动

```powershell
.\build-tools\build.ps1
```

ROM 加载 `kernel/build/kernel.hex`，B 段加载 `storage/build/storage.hex`，
启动后执行 `run txt-viewer`，或对可信镜像执行 `quickrun txt-viewer`。
单独重建可用 `.\user-programs\build.ps1 -Program txt-viewer`，之后重新打包存储。

## 命令

| 命令 | 含义 |
|---|---|
| `ls [PATH]` | 列出目录；省略路径表示根目录 |
| `open PATH` | 读取并显示文本，同时建立可编辑草稿 |
| `show` | 显示当前草稿 |
| `append TEXT` | 在末尾追加一行；补齐必要的换行 |
| `set N TEXT` | 替换第 N 行，行号从 1 开始 |
| `delete N` | 删除第 N 行 |
| `clear` | 清空草稿，不立即改 EEPROM |
| `save` | 用草稿替换文件正文并更新长度、校验和 |
| `discard` | 放弃草稿并关闭当前文件 |
| `q` / `quit` | 返回系统终端；有未保存更改时拒绝退出 |
| `help` | 显示帮助 |

例如：

```text
open docs/notes.txt
append Hello, Aurora!
set 1 My notes
show
save
q
```

`txt* >` 表示有未保存修改。存在未保存修改时不能打开另一文件，需要先保存或放弃。
Backspace/Delete 删除输入的最后一个字符，Esc 放弃当前命令；输入支持 CR/LF/CRLF。
命令上限 159 字符，超长整条命令丢弃。文本最多 4096 字符，还受文件预留容量限制；
`touch` 新建文件和示例文本默认预留 2048 字。无法保存时保留草稿并打印错误码。
当前是 ASCII 行编辑器，不是全屏编辑器，不支持 Unicode、光标定位和撤销。
路径始终相对根目录，可加开头 `/`；没有 `cd`，不支持 `.`/`..`。

系统终端中的 `mkdir`、`touch`、`rm`、`rename`、`mv` 用于管理目录和文件。
可执行文件可以在系统终端移动/重命名/删除，但用户系统调用拒绝修改其正文。

## 保存的重要限制

- 保存会检查容量和字符，并逐字写入、读回检查，成功后更新校验和与长度。
- 没有日志、事务或断电恢复；写入被打断可能留下损坏文件。
- 修改的是 Digital 中加载的 EEPROM，不会自动修改宿主机的 `.txt` 或原始 `.hex`。
  需要用 Digital 导出/保存 EEPROM 内容才能保留；重新加载打包镜像会覆盖运行时编辑。
- 重新打包不会合并运行时修改。先备份导出的镜像，再更新存储镜像。

接口与存储格式见 [ASFS2](../../kernel/filesystem-v2.md)。

## 输出与双终端

正文、帮助和文件名均使用有界 `system.io.Write` 批次，最多 64 字，按键回显仍为单字符。
两个 4096 字工作区使用 `.noinit`，不在每次启动清零；仅在有效长度内读写，重新进入不会显示旧草稿。
程序从哪一端启动就在哪一端交互，另一端仍能使用系统终端。
文件 ID 在本用户进程生存期间被保留，另一端 rm 已引用文件会返回 EBUSY，避免删除后 ID 复用。
另一端执行文件命令时，文件系统调用可能返回 EBUSY；等待该命令结束后重试即可。
`kill` 强制退出不会保存草稿。详见 [双终端](../../kernel/remote-console.md)。
