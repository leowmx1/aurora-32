# Aurora build tools

完整说明见 [docs/build-tools/README.md](../docs/build-tools/README.md)。
从项目根目录运行 `./build-tools/build.ps1`，无需安装外部 LLVM。

编译任意单个用户 C 源文件：`./build-tools/build-user.ps1 "C:/my-project/main.c"`。
自动构建配套内核；用户产物保存到源码旁的 `build/`，无需修改示例清单。
