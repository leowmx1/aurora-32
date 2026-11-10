param([string]$Python='python')
$ErrorActionPreference='Stop'
$root=Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
$compile=Join-Path $root 'build-tools/compile.ps1'
$clang=Join-Path $root 'build-tools/bin/clang.exe'
$resource=Join-Path $root 'build-tools/lib/clang/24'
& $compile "$PSScriptRoot/contract.c" "$PSScriptRoot/build/contract.hex"
foreach($opcode in @('lusp','lufp','ousp','oufp')) {
  $rejected=$false
  try {
    & $compile "$PSScriptRoot/privileged_user.c" "$PSScriptRoot/build/reject-$opcode.hex" `
      -Mode user -ExtraArguments @('-DAURORA_SYSCALL_ENTRY_ADDRESS=2', "-DTEST_OPCODE=$opcode")
  } catch {
    if($_.Exception.Message -notmatch 'User image contains privileged instructions') { throw }
    $rejected=$true
  }
  if(-not $rejected) { throw "User build accepted privileged instruction: $opcode" }
  Write-Output "PASS: user build rejects $opcode"
}
foreach($words in @(1,4194304)) {
  $output="$PSScriptRoot/build/layout-$words.s"
  & $clang -target aurora -ffreestanding -O2 -resource-dir $resource -S `
    "$PSScriptRoot/capacity.c" "-DTEST_WORDS=$words" -o $output
  if($LASTEXITCODE -ne 0) { throw "Valid user allocation rejected: $words words" }
  $expected=if($words -eq 1){'0x813fffff'}else{'0x81000000'}
  if(-not (Select-String -LiteralPath $output -SimpleMatch $expected)) {
    throw "Wrong user global address: expected $expected"
  }
}
$diagnostics=(& $clang -target aurora -ffreestanding -O2 -resource-dir $resource `
  -fno-crash-diagnostics -S "$PSScriptRoot/capacity.c" -DTEST_WORDS=4194305 `
  -o "$PSScriptRoot/build/too-large.s" 2>&1) -join "`n"
if($LASTEXITCODE -eq 0 -or $diagnostics -notmatch 'global variables exceed Aurora RAM capacity') {
  throw "Oversize user globals were not correctly rejected: $diagnostics"
}
& $clang -target aurora -ffreestanding -O2 -resource-dir $resource -S `
  "$PSScriptRoot/capacity.c" -DTEST_WORDS=4194305 -DTEST_KERNEL `
  -o "$PSScriptRoot/build/kernel-large.s"
if($LASTEXITCODE -ne 0) { throw 'Kernel RAM was incorrectly restricted to a user bank' }
Write-Output 'PASS: user globals fit exactly one 2^22-word bank; kernel retains 2^24 words.'
& $Python "$PSScriptRoot/run_context.py"
if($LASTEXITCODE -ne 0) { throw 'User-pointer regression failed' }
