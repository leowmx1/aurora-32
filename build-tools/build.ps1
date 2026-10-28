param(
  [ValidateSet('all', 'calculator', 'calculator-trace', 'gomoku-screen', 'guess-number', 'snake-screen', 'maze-screen', 'mini-terminal', 'graphics-demo')]
  [string]$Program = 'all'
)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
& (Join-Path $projectRoot 'kernel\build.ps1')
& (Join-Path $projectRoot 'user-programs\build.ps1') -Program $Program
Write-Output 'Load kernel/build/kernel.hex into ROM and one user HEX into UROM.'
