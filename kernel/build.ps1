param([string]$OutputDirectory = (Join-Path $PSScriptRoot 'build'))
$ErrorActionPreference = 'Stop'
& (Join-Path $PSScriptRoot '..\build-tools\compile.ps1') `
  -SourceFile (Join-Path $PSScriptRoot 'kernel.c') `
  -HexFile (Join-Path $OutputDirectory 'kernel.hex') -Mode kernel
