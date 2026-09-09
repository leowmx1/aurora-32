param(
  [string]$KernelObject =
      (Join-Path $PSScriptRoot '..\系统内核\build\kernel.o')
)

$ErrorActionPreference = 'Stop'
$toolDirectory = 'F:\llvm-project\llvm\lib\Target\Aurora-32\_build-tools\bin'
$clang = Join-Path $toolDirectory 'clang.exe'
$objcopy = Join-Path $toolDirectory 'llvm-objcopy.exe'
$nm = Join-Path $toolDirectory 'llvm-nm.exe'
$hexConverter =
    'F:\llvm-project\llvm\lib\Target\Aurora-32\examples\bin-to-logisim.ps1'

foreach ($required in @($clang, $objcopy, $nm, $hexConverter, $KernelObject)) {
  if (-not (Test-Path -LiteralPath $required)) {
    throw "Required build input/tool not found: $required"
  }
}

$symbolLine = (& $nm -n $KernelObject |
  Select-String -Pattern '\saurora_syscall_entry$' |
  Select-Object -First 1).ToString().Trim()
if (-not $symbolLine) {
  throw 'aurora_syscall_entry is missing from kernel.o'
}
$symbolParts = $symbolLine -split '\s+'
$entryByteAddress = [Convert]::ToUInt64($symbolParts[0], 16)
if (($entryByteAddress % 4) -ne 0) {
  throw "aurora_syscall_entry is not word aligned: 0x$($symbolParts[0])"
}
$entryWordAddress = $entryByteAddress / 4
$entryDefine = '-DAURORA_SYSCALL_ENTRY_ADDRESS=0x{0:X8}' -f $entryWordAddress

$commonArguments = @(
  '-target', 'aurora', '-x', 'c', '-std=c11', '-ffreestanding',
  '-fomit-frame-pointer', '-O2', '-I', (Join-Path $PSScriptRoot 'include'),
  '-DAURORA_USER_API_ONLY=1', $entryDefine
)

function Build-UserProgram([string]$DirectoryName, [string]$SourceName,
                           [string]$OutputName,
                           [string[]]$ExtraArguments = @()) {
  $programDirectory = Join-Path $PSScriptRoot "程序\$DirectoryName"
  $outputDirectory = Join-Path $programDirectory 'build'
  $source = Join-Path $programDirectory $SourceName
  $assembly = Join-Path $outputDirectory "$OutputName.s"
  $object = Join-Path $outputDirectory "$OutputName.o"
  $binary = Join-Path $outputDirectory "$OutputName.bin"
  $hex = Join-Path $outputDirectory "$OutputName.hex"
  if (-not (Test-Path -LiteralPath $source)) {
    Write-Warning "Skipping missing user program source: $source"
    return
  }
  New-Item -ItemType Directory -Force -Path $outputDirectory | Out-Null

  & $clang @commonArguments @ExtraArguments -S $source -o $assembly
  if ($LASTEXITCODE -ne 0) { throw "$OutputName assembly generation failed" }

  $privilegedInstructions = Select-String -LiteralPath $assembly -Pattern `
    '^\s*(ei|di|halt|iret|pushu|popu)(\s|$)'
  if ($privilegedInstructions) {
    $first = $privilegedInstructions | Select-Object -First 1
    throw ("$OutputName contains a kernel-only instruction at assembly " +
      "line $($first.LineNumber): $($first.Line.Trim())")
  }

  & $clang @commonArguments @ExtraArguments -c $source -o $object
  if ($LASTEXITCODE -ne 0) { throw "$OutputName object generation failed" }
  & $objcopy -O binary --only-section=.text $object $binary
  if ($LASTEXITCODE -ne 0) { throw "$OutputName machine-code extraction failed" }

  $imageBytes = [IO.File]::ReadAllBytes($binary)
  if ($imageBytes.Length -lt 8 -or $imageBytes[0] -ne 0x30) {
    throw "$OutputName does not start with the required UROM JMP entry"
  }
  $uromEntry = [BitConverter]::ToUInt32($imageBytes, 4)
  $uromEntryUnsigned = [uint64]$uromEntry
  $uromBegin = [uint64]2147483648  # 0x80000000
  $uromEnd = [uint64]2164260864    # 0x81000000
  if ($uromEntryUnsigned -lt $uromBegin -or
      $uromEntryUnsigned -ge $uromEnd) {
    throw ('{0} has an invalid UROM entry target: 0x{1:X8}' -f `
      $OutputName, $uromEntry)
  }

  & $hexConverter -InputFile $binary -OutputFile $hex | Out-Null
  if ($LASTEXITCODE -ne 0) { throw "$OutputName HEX conversion failed" }
}

Build-UserProgram 'keyboard-client' 'keyboard_client.c' 'keyboard-client'
Build-UserProgram 'hello-loop' 'hello_loop.c' 'hello-loop'
Build-UserProgram 'system-io-demo' 'system_io_demo.c' 'system-io-demo'
Build-UserProgram 'calculator' 'calculator.c' 'calculator'
Build-UserProgram 'calculator' 'calculator.c' 'calculator-trace' @(
  '-DAURORA_CALCULATOR_TRACE=1'
)

Write-Output ('System-call ROM word address: 0x{0:X8}' -f $entryWordAddress)
Write-Output 'Each user image is in its program-specific build directory.'
