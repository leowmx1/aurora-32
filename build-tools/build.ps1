param(
  [ValidateSet('all', 'calculator', 'calculator-trace', 'tic-tac-toe', 'guess-number')]
  [string]$Program = 'all'
)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
& (Join-Path $projectRoot 'kernel\build.ps1')
& (Join-Path $projectRoot 'user-programs\build.ps1') -Program $Program
Write-Output 'Load kernel/build/kernel.hex into ROM and one user HEX into UROM.'
