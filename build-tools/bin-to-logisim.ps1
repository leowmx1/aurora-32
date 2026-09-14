param(
  [Parameter(Mandatory = $true)]
  [string]$InputFile,

  [string]$OutputFile
)

$inputPath = [IO.Path]::GetFullPath($InputFile)
if (-not $OutputFile) {
  $OutputFile = [IO.Path]::ChangeExtension($inputPath, '.hex')
}
$outputPath = [IO.Path]::GetFullPath($OutputFile)

[byte[]]$bytes = [IO.File]::ReadAllBytes($inputPath)
if (($bytes.Length % 4) -ne 0) {
  throw "Aurora binary size must be a multiple of four bytes: $inputPath"
}

$words = New-Object 'System.Collections.Generic.List[string]'
for ($i = 0; $i -lt $bytes.Length; $i += 4) {
  [uint32]$word = [uint32]$bytes[$i] -bor
                   ([uint32]$bytes[$i + 1] -shl 8) -bor
                   ([uint32]$bytes[$i + 2] -shl 16) -bor
                   ([uint32]$bytes[$i + 3] -shl 24)
  $words.Add($word.ToString('X8'))
}

$lines = New-Object 'System.Collections.Generic.List[string]'
$lines.Add('v2.0 raw')
for ($i = 0; $i -lt $words.Count; $i += 16) {
  $count = [Math]::Min(16, $words.Count - $i)
  $lines.Add([string]::Join(' ', $words.GetRange($i, $count)))
}

# Logisim expects the header to be the first bytes in the file, so omit a BOM.
$utf8NoBom = New-Object Text.UTF8Encoding($false)
[IO.File]::WriteAllText($outputPath,
                        ([string]::Join("`n", $lines) + "`n"),
                        $utf8NoBom)
Write-Output $outputPath
