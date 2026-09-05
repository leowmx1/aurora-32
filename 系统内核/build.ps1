param(
  [string]$OutputDirectory = (Join-Path $PSScriptRoot 'build')
)

$ErrorActionPreference = 'Stop'
$builder = 'F:\llvm-project\llvm\lib\Target\Aurora-32\aurora_c_build.ps1'
if (-not (Test-Path -LiteralPath $builder)) {
  throw "Aurora C builder not found: $builder"
}

New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null
& $builder (Join-Path $PSScriptRoot 'kernel.c') `
  (Join-Path $OutputDirectory 'kernel.hex')
if ($LASTEXITCODE -ne 0) {
  throw "Kernel build failed with exit code $LASTEXITCODE"
}

Write-Output 'Load build\kernel.hex into ROM.'
