# User programs

使用方法、源码结构和当前限制集中在 [docs/user_program/](../docs/user_program/README.md)。
本目录维护用户 API 与各程序源码，产物位于 `programs/<name>/build/`。
每个程序目录只保留 `.c` 源码、Markdown 说明和 `build/` 产物；公共 API 头文件统一位于 `include/`。
测试脚本、测试替身和临时诊断产物已于 2026-10-19 清理，不参与正常构建。
从项目根目录执行 `./build-tools/build.ps1`，或在内核没有改变时运行本目录 `./build.ps1`。
