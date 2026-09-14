param(
  [Parameter(Mandatory = $true, Position = 0)][string]$SourceFile,
  [Parameter(Mandatory = $true, Position = 1)][string]$HexFile,
  [ValidateSet('kernel', 'user')][string]$Mode = 'kernel',
  [string[]]$ExtraArguments = @()
)
$ErrorActionPreference = 'Stop'
$source = [IO.Path]::GetFullPath($SourceFile)
$hex = [IO.Path]::GetFullPath($HexFile)
if (-not (Test-Path -LiteralPath $source -PathType Leaf)) { throw "Source not found: $source" }
if ([IO.Path]::GetExtension($hex) -ine '.hex') { throw 'Output must end in .hex' }
$bin = Join-Path $PSScriptRoot 'bin'
$clang = Join-Path $bin 'clang.exe'
$objcopy = Join-Path $bin 'llvm-objcopy.exe'
$nm = Join-Path $bin 'llvm-nm.exe'
$readobj = Join-Path $bin 'llvm-readobj.exe'
$resource = Join-Path $PSScriptRoot 'lib\clang\24'
foreach ($path in @($clang, $objcopy, $nm, $readobj, "$resource\include\aurora.h")) {
  if (-not (Test-Path -LiteralPath $path)) { throw "Bundled tool/input missing: $path" }
}
New-Item -ItemType Directory -Force -Path ([IO.Path]::GetDirectoryName($hex)) | Out-Null
$assembly = [IO.Path]::ChangeExtension($hex, '.s')
$object = [IO.Path]::ChangeExtension($hex, '.o')
$binary = [IO.Path]::ChangeExtension($hex, '.bin')
$arguments = @('-target', 'aurora', '-x', 'c', '-std=c11', '-ffreestanding',
  '-fomit-frame-pointer', '-O2', '-resource-dir', $resource) +
  $ExtraArguments + @('-I', (Join-Path $PSScriptRoot 'include'))
if ($Mode -eq 'user') { $arguments += '-DAURORA_USER_API_ONLY=1' }
Write-Output "Compiling $Mode image: $source"
& $clang @arguments -S $source -o $assembly
if ($LASTEXITCODE -ne 0) { throw "Assembly generation failed: $source" }
if ($Mode -eq 'user') {
  $privileged = Select-String -LiteralPath $assembly -Pattern '^\s*(ei|di|halt|iret|pushu|popu)(\s|$)'
  if ($privileged) { throw "User image contains privileged instructions: $($privileged[0])" }
}
& $clang @arguments -c $source -o $object
if ($LASTEXITCODE -ne 0) { throw "Object generation failed: $source" }
$undefined = @(& $nm --undefined-only $object)
if ($LASTEXITCODE -ne 0 -or $undefined.Count -ne 0) { throw "Unresolved symbols in $object : $undefined" }
$relocations = (& $readobj --relocations $object) -join "`n"
if ($LASTEXITCODE -ne 0 -or $relocations -notmatch 'Relocations\s*\[\s*\]') {
  throw "Unresolved relocations in $object; raw extraction would be unsafe"
}
& $objcopy -O binary --only-section=.text $object $binary
if ($LASTEXITCODE -ne 0) { throw "Binary extraction failed: $source" }
$bytes = [IO.File]::ReadAllBytes($binary)
if ($bytes.Length -lt 8 -or ($bytes.Length % 4) -ne 0 -or
    [BitConverter]::ToUInt32($bytes, 0) -ne 0x30) { throw "Invalid ROM entry in $binary" }
$base = [uint64]0
if ($Mode -eq 'user') { $base = [uint64]2147483648 }
$entry = [uint64][BitConverter]::ToUInt32($bytes, 4)
if ($entry -lt $base -or $entry -ge ($base + [uint64]16777216) -or
    (($entry - $base) * 4) -ge $bytes.Length) { throw "Entry outside generated $Mode image" }
& (Join-Path $PSScriptRoot 'bin-to-logisim.ps1') -InputFile $binary -OutputFile $hex | Out-Null
if ((Get-Content -LiteralPath $hex -TotalCount 1) -ne 'v2.0 raw') { throw 'Bad HEX header' }
Write-Output "Built: $hex"
