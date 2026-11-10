param([switch]$SkipBuild, [string]$Python='python', [switch]$SelfHost)
$ErrorActionPreference='Stop'
$root=Split-Path -Parent $PSScriptRoot
$priorKernel=$env:AURORA_TEST_KERNEL_DIR
Push-Location $root
try {
  if(-not $SkipBuild) { & ./build-tools/build.ps1 }
  & ./tests/user-context/test.ps1 -Python $Python
  foreach($source in @('compiler/install_contract','storage/fs_contract','storage/console_contract','storage/multiprocess_contract','storage/background_contract','tetris/selftest',
      'kernel-events/event_contract','kernel-events/sleep_contract',
      'graphics-syscall/contract','graphics-syscall/bitmap_contract','graphics-syscall/image_contract','graphics-syscall/render_fixture')) {
    $folder=Split-Path $source; $name=Split-Path $source -Leaf
    & ./build-tools/compile.ps1 "tests/$source.c" "tests/$folder/build/$name.hex" -Mode user `
      -ExtraArguments '-DAURORA_SYSCALL_ENTRY_ADDRESS=2'
  }
  Remove-Item Env:AURORA_TEST_KERNEL_DIR -ErrorAction SilentlyContinue
  foreach($script in @('storage/run_storage','storage/run_ldma','storage/run_files','storage/run_file_contract',
      'storage/run_metadata','storage/run_listing','storage/run_capacity',
      'storage/run_remote','storage/run_multiprocess','storage/run_banked_io','storage/run_background','storage/run_history','storage/run_shutdown','storage/run_performance','tetris/run_tetris','graphics-syscall/run_layout',
      'compiler/run_compiler','compiler/run_install','compiler/run_tinyc_examples','compiler/run_choices','compiler/run_limits','compiler/run_startup')) {
    & $Python "tests/$script.py"
    if($LASTEXITCODE -ne 0) { throw "Regression failed: $script" }
  }
  if($SelfHost) {
    & $Python tests/compiler/run_selfhost.py
    if($LASTEXITCODE -ne 0) { throw 'Self-host regression failed' }
  }
  & ./kernel/build.ps1 -DirectBoot -OutputDirectory tests/storage/build/direct-kernel
  $env:AURORA_TEST_KERNEL_DIR=Join-Path $root 'tests/storage/build/direct-kernel'
  foreach($script in @('kernel-events/run_core_examples','kernel-events/run_events','kernel-events/run_sleep',
      'graphics-syscall/run_graphics','graphics-syscall/run_bitmap_syscall','graphics-syscall/run_image_syscall',
      'gomoku-screen/run_gomoku_screen','maze-screen/run_maze_screen','snake-screen/run_snake_screen')) {
    & $Python "tests/$script.py"
    if($LASTEXITCODE -ne 0) { throw "Regression failed: $script" }
  }
  Write-Output 'All regressions passed (actual BIN / ideal ISA and devices; not Digital circuit timing).'
} finally {
  if($null -eq $priorKernel) { Remove-Item Env:AURORA_TEST_KERNEL_DIR -ErrorAction SilentlyContinue }
  else { $env:AURORA_TEST_KERNEL_DIR=$priorKernel }
  Pop-Location
}
