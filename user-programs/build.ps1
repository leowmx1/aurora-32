param(
  [string]$KernelObject = (Join-Path $PSScriptRoot '..\kernel\build\kernel.o'),
  [ValidateSet('all', 'calculator', 'calculator-trace', 'tic-tac-toe', 'guess-number', 'snake', 'maze', 'mini-terminal')]
  [string]$Program = 'all'
)
$ErrorActionPreference = 'Stop'
$tools = Join-Path $PSScriptRoot '..\build-tools'
$nm = Join-Path $tools 'bin\llvm-nm.exe'
if (-not (Test-Path -LiteralPath $KernelObject -PathType Leaf)) {
  throw "Build the kernel first; missing $KernelObject"
}
$symbols = @(& $nm -n $KernelObject)
if ($LASTEXITCODE -ne 0) { throw 'Cannot read kernel symbols' }
$entryLines = @($symbols | Where-Object { $_ -match '\s[Tt]\s+aurora_syscall_entry$' })
if ($entryLines.Count -ne 1) { throw 'Expected one defined aurora_syscall_entry in kernel.o' }
$entryBytes = [Convert]::ToUInt64(($entryLines[0].Trim() -split '\s+')[0], 16)
if (($entryBytes % 4) -ne 0 -or $entryBytes -ge 67108864) { throw 'Invalid kernel syscall entry' }
$entryWord = [uint64]($entryBytes / 4)
$entryDefine = '-DAURORA_SYSCALL_ENTRY_ADDRESS=0x{0:X8}' -f $entryWord
$programs = @(
  @{ Name='calculator'; Directory='calculator'; Source='calculator.c'; Extra=@() },
  @{ Name='calculator-trace'; Directory='calculator'; Source='calculator.c'; Extra=@('-DAURORA_CALCULATOR_TRACE=1') },
  @{ Name='tic-tac-toe'; Directory='tic-tac-toe'; Source='tic_tac_toe.c'; Extra=@() },
  @{ Name='guess-number'; Directory='guess-number'; Source='guess_number.c'; Extra=@() },
  @{ Name='snake'; Directory='snake'; Source='snake.c'; Extra=@() },
  @{ Name='maze'; Directory='maze'; Source='maze.c'; Extra=@() },
  @{ Name='mini-terminal'; Directory='mini-terminal'; Source='mini_terminal.c'; Extra=@() }
)
foreach ($item in $programs) {
  if ($Program -ne 'all' -and $Program -ne $item.Name) { continue }
  $directory = Join-Path $PSScriptRoot ("programs\" + $item.Directory)
  $extra = @('-I', (Join-Path $PSScriptRoot 'include'), $entryDefine) + $item.Extra
  & (Join-Path $tools 'compile.ps1') -SourceFile (Join-Path $directory $item.Source) `
    -HexFile (Join-Path $directory ("build\" + $item.Name + '.hex')) `
    -Mode user -ExtraArguments $extra
}
Write-Output ('System-call ROM word address: 0x{0:X8}' -f $entryWord)
