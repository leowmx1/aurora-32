param([switch]$SkipBuild)
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$priorKernel=$env:AURORA_TEST_KERNEL_DIR
Push-Location $root
try {
  if(-not $SkipBuild) { & ./build-tools/build.ps1 }
  foreach($source in @('storage/fs_contract','storage/console_contract','tetris/selftest',
      'kernel-events/event_contract','kernel-events/sleep_contract',
      'graphics-syscall/contract','graphics-syscall/bitmap_contract','graphics-syscall/render_fixture')) {
    $folder=Split-Path $source; $name=Split-Path $source -Leaf
    & ./build-tools/compile.ps1 "tests/$source.c" "tests/$folder/build/$name.hex" -Mode user `
      -ExtraArguments '-DAURORA_SYSCALL_ENTRY_ADDRESS=2'
  }
  Remove-Item Env:AURORA_TEST_KERNEL_DIR -ErrorAction SilentlyContinue
  foreach($script in @('storage/run_storage','storage/run_ldma','storage/run_files','storage/run_file_contract',
      'storage/run_metadata','storage/run_listing','storage/run_capacity',
      'storage/run_remote','storage/run_history','storage/run_shutdown','storage/run_performance','tetris/run_tetris')) {
    & python "tests/$script.py"
    if($LASTEXITCODE -ne 0) { throw "Regression failed: $script" }
  }
  & ./kernel/build.ps1 -DirectBoot -OutputDirectory tests/storage/build/direct-kernel
  $env:AURORA_TEST_KERNEL_DIR=Join-Path $root 'tests/storage/build/direct-kernel'
  foreach($script in @('kernel-events/run_core_examples','kernel-events/run_events','kernel-events/run_sleep',
      'graphics-syscall/run_graphics','graphics-syscall/run_bitmap_syscall',
      'gomoku-screen/run_gomoku_screen','maze-screen/run_maze_screen','snake-screen/run_snake_screen')) {
    & python "tests/$script.py"
    if($LASTEXITCODE -ne 0) { throw "Regression failed: $script" }
  }
  Write-Output 'All regressions passed (actual BIN / ideal ISA and devices; not Digital circuit timing).'
} finally {
  if($null -eq $priorKernel) { Remove-Item Env:AURORA_TEST_KERNEL_DIR -ErrorAction SilentlyContinue }
  else { $env:AURORA_TEST_KERNEL_DIR=$priorKernel }
  Pop-Location
}
