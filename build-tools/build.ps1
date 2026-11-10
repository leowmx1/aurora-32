param(
  [ValidateSet('all', 'calculator', 'gomoku-screen', 'guess-number', 'snake-screen', 'maze-screen', 'editor', 'tetris', 'compiler', 'process-demo')]
  [string]$Program = 'all'
)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
& (Join-Path $projectRoot 'kernel\build.ps1')
& (Join-Path $projectRoot 'user-programs\build.ps1') -Program $Program
if ($Program -eq 'all') {
  & (Join-Path $PSScriptRoot 'pack-storage.ps1')
  Write-Output 'Load kernel/build/kernel.hex into ROM and storage/build/storage.hex into EEPROM B. Both storage and UROM require kernel-only writes. Back up runtime EEPROM edits before reloading.'
} else {
  Write-Output 'Selected program rebuilt. Repack storage with build-tools/pack-storage.ps1.'
}
