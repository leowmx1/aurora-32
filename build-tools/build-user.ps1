param(
  [Parameter(Mandatory = $true, Position = 0)]
  [ValidateNotNullOrEmpty()]
  [string]$SourceFile,

  # Optional: use an existing matching kernel instead of rebuilding this one.
  [string]$KernelObject
)

$ErrorActionPreference = 'Stop'

# Resolve against the caller's PowerShell location. Unlike Resolve-Path on
# Windows PowerShell 5.1, this API does not retain escaped [] in the directory.
$sourcePath = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($SourceFile)
$sourceItem = Get-Item -LiteralPath $sourcePath -ErrorAction Stop
if ($sourceItem -isnot [IO.FileInfo]) {
  throw "Expected a C source file: $SourceFile"
}
$source = $sourceItem.FullName
if ([IO.Path]::GetExtension($source) -ine '.c') {
  throw "Expected a .c source file: $source"
}

$projectRoot = Split-Path -Parent $PSScriptRoot
$compiler = Join-Path $PSScriptRoot 'compile.ps1'
$nm = Join-Path $PSScriptRoot 'bin\llvm-nm.exe'
foreach ($tool in @($compiler, $nm)) {
  if (-not (Test-Path -LiteralPath $tool -PathType Leaf)) {
    throw "Bundled build tool missing: $tool"
  }
}

if ($KernelObject) {
  $kernelPath = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($KernelObject)
  $kernelItem = Get-Item -LiteralPath $kernelPath -ErrorAction Stop
  if ($kernelItem -isnot [IO.FileInfo]) {
    throw "Expected a kernel object file: $KernelObject"
  }
  $kernel = $kernelItem.FullName
} else {
  # A fresh kernel image and its symbol table must describe the same build.
  $kernelBuilder = Join-Path $projectRoot 'kernel\build.ps1'
  if (-not (Test-Path -LiteralPath $kernelBuilder -PathType Leaf)) {
    throw 'Kernel sources are unavailable; supply -KernelObject with a matching kernel.o'
  }
  & $kernelBuilder
  $kernel = Join-Path $projectRoot 'kernel\build\kernel.o'
}

$symbols = @(& $nm -n $kernel)
if ($LASTEXITCODE -ne 0) { throw "Cannot read kernel symbols: $kernel" }
$entries = @($symbols | Where-Object { $_ -match '\s[Tt]\s+aurora_syscall_entry$' })
if ($entries.Count -ne 1) { throw 'Expected one defined aurora_syscall_entry in kernel object' }
$entryBytes = [Convert]::ToUInt64(($entries[0].Trim() -split '\s+')[0], 16)
if (($entryBytes % 4) -ne 0 -or $entryBytes -ge 67108864) {
  throw 'Kernel syscall entry is unaligned or outside ROM'
}
$entryWord = [uint64]($entryBytes / 4)
$entryDefine = '-DAURORA_SYSCALL_ENTRY_ADDRESS=0x{0:X8}' -f $entryWord

$sourceDirectory = [IO.Path]::GetDirectoryName($source)
$outputDirectory = Join-Path $sourceDirectory 'build'
$outputName = [IO.Path]::GetFileNameWithoutExtension($source) + '.hex'
$hex = Join-Path $outputDirectory $outputName
$extra = @($entryDefine)
$projectInclude = Join-Path $projectRoot 'user-programs\include'
if (Test-Path -LiteralPath $projectInclude -PathType Container) {
  $extra = @('-I', $projectInclude) + $extra
}

& $compiler -SourceFile $source -HexFile $hex -Mode user -ExtraArguments $extra
Write-Output ('System-call ROM word address: 0x{0:X8}' -f $entryWord)
Write-Output "User image: $hex"
Write-Output 'Load the matching kernel HEX into ROM and this user HEX into UROM.'
