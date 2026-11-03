param(
  [string]$Manifest = (Join-Path $PSScriptRoot '../storage/programs.json'),
  [string]$OutputFile,
  [uint32]$CapacityWords = 16777216
)
$ErrorActionPreference = 'Stop'
$manifestPath = (Get-Item -LiteralPath $Manifest).FullName
$root = Split-Path -Parent $manifestPath
if (-not $OutputFile) { $OutputFile = Join-Path $root 'build/storage.hex' }
$output = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath($OutputFile)
$config = Get-Content -LiteralPath $manifestPath -Raw | ConvertFrom-Json
if ($config.Version -ne 2 -or $config.Files.Count -gt 64 -or $config.Files.Count -lt 1) {
  throw 'Expected manifest version 2 and 1..64 files'
}
if ($CapacityWords -gt 16777216 -or $CapacityWords -lt 16) { throw 'Invalid device capacity' }
[uint64]$dataOffset = $config.DataOffsetWords
if ($dataOffset -lt (16 + 40 * 64) -or $dataOffset -gt $CapacityWords) {
  throw 'DataOffsetWords overlaps directory or exceeds device capacity'
}
$image = New-Object 'System.Collections.Generic.List[uint32]'
for ($i=0; $i -lt $dataOffset; ++$i) { $image.Add(0) }
$listing = @()
 $entries = New-Object 'System.Collections.Generic.List[object]'
 $paths = @{}
 foreach ($file in $config.Files) {
   $path = [string]$file.Name
   if ($path.Length -gt 127 -or $path -cnotmatch '^[a-z0-9_.-]+(/[a-z0-9_.-]+)*$') { throw "Invalid path: $path" }
   $parts = $path.Split('/'); $prefix = ''; $parent = 0
   for ($p=0; $p -lt $parts.Length; ++$p) {
     $part = $parts[$p]
     if ($part.Length -gt 31 -or $part -in @('.','..')) { throw "Invalid component: $part" }
     $prefix = if ($prefix) { "$prefix/$part" } else { $part }
     if ($paths.ContainsKey($prefix)) {
       if ($p -eq $parts.Length-1 -or $entries[$paths[$prefix]-1].Kind -ne 'Directory') { throw "Duplicate/conflicting path: $prefix" }
     } else {
       $kind = if ($p -lt $parts.Length-1) { 'Directory' } elseif ($file.Type -eq 'Text') { 'Text' } elseif ($file.Hex -and (!$file.Type -or $file.Type -eq 'Executable')) { 'Executable' } else { throw "Unknown type: $path" }
       $entries.Add([pscustomobject]@{Name=$part; Path=$prefix; Parent=$parent; Kind=$kind; Source=$file.Source; Hex=$file.Hex; ReserveWords=$file.ReserveWords})
       $paths[$prefix] = $entries.Count
     }
     $parent = $paths[$prefix]
   }
 }
 if ($entries.Count -gt 64) { throw 'At most 64 directory slots, including directories' }
foreach ($item in $entries) {
  $name = [string]$item.Name
  if ($item.Parent -eq 0 -and $name -in @('ls','programs','run','quickrun','tree','cat','mkdir','touch','rm','mv','rename','stat','verify','df','mount','help','halt','about','version',
                  'uptime','ps','mem','devices','selftest','echo','send','netstat','history','clear','all')) {
    throw "Reserved shell name: $name"
  }
  $exe = New-Object 'System.Collections.Generic.List[uint32]'
  [uint32]$type = 1; [uint32]$reserve = 0
  if ($item.Kind -eq 'Executable') {
  $hex = [IO.Path]::GetFullPath((Join-Path $root $item.Hex))
  $meta = Get-Content -LiteralPath ([IO.Path]::ChangeExtension($hex, '.program.json')) -Raw | ConvertFrom-Json
  if ($meta.Format -ne 'AuroraExecutableSource' -or $meta.Version -ne 1 -or
      $meta.ABI -ne 3 -or $meta.SyscallGateway -ne 2 -or $meta.Startup -ne 'compiler-init' -or
      $meta.HexSHA256 -ne (Get-FileHash -LiteralPath $hex -Algorithm SHA256).Hash) {
    throw "Unverified/stale executable metadata; rebuild $hex"
  }
  $lines = [IO.File]::ReadAllLines($hex)
  if ($lines.Length -lt 2 -or $lines[0].Trim() -ne 'v2.0 raw') { throw "Unsupported HEX: $hex" }
  $code = New-Object 'System.Collections.Generic.List[uint32]'
  foreach ($line in $lines[1..($lines.Length-1)]) {
    $body = ($line -split '#', 2)[0].Trim()
    if (-not $body) { continue }
    foreach ($token in ($body -split '\s+')) {
      if ($token -notmatch '^(?:(\d+)\*)?([0-9a-fA-F]{1,8})$') { throw "Invalid HEX word: $token" }
      [uint64]$repeat = 1
      if ($Matches[1]) { $repeat = [uint64]$Matches[1] }
      [uint32]$value = [Convert]::ToUInt32($Matches[2], 16)
      if ($repeat -lt 1 -or $repeat -gt $CapacityWords -or $code.Count + $repeat -gt $CapacityWords) {
        throw 'HEX expansion exceeds device capacity'
      }
      for ($i=0; $i -lt $repeat; ++$i) { $code.Add($value) }
    }
  }
  if ($code.Count -ne $meta.Words -or $code.Count -lt 2 -or $code[0] -ne 0x30 -or
      $code[1] -lt 2147483648 -or [uint64]$code[1] -ge (2147483648 + [uint64]$code.Count)) {
    throw "Invalid user entry or image length: $hex"
  }
  # AEXE has its own version/header/segment table, independent of ASFS.
  foreach ($v in @(0x41455845,1,16,(22+$code.Count),2147483648,3,2,1,16,6,1,0,0,0,0,0,
                  2147483648,22,$code.Count,$code.Count,1,0)) { $exe.Add([uint32]$v) }
  $exe.AddRange($code)
  $reserve = $exe.Count
  } elseif ($item.Kind -eq 'Text') {
    $type = 2
    $source = [IO.Path]::GetFullPath((Join-Path $root $item.Source))
    $content = [IO.File]::ReadAllText($source).Replace("`r`n","`n").Replace("`r","`n")
    foreach ($ch in $content.ToCharArray()) {
      $v = [uint32][char]$ch
      if ($v -ne 9 -and $v -ne 10 -and ($v -lt 32 -or $v -gt 126)) { throw "Text must be ASCII (TAB/LF allowed): $source" }
      $exe.Add($v)
    }
    $reserve = if ($item.ReserveWords) { [uint32]$item.ReserveWords } else { 2048 }
    if ($reserve -lt $exe.Count -or $reserve -gt 4096) { throw "Text capacity must fit text and be <=4096: $source" }
  } else { $type = 3 }
  if ([uint64]$image.Count + $reserve -gt $CapacityWords) { throw 'Storage image exceeds device capacity' }
  [uint64]$sum = 0
  foreach ($word in $exe) { $sum = ($sum + $word) -band [uint64]4294967295 }
  $index = 16 + 40 * $listing.Count
  $offset = if ($type -eq 3) { 0 } else { $image.Count }
  $image[$index] = $type; $image[$index+1] = $offset
  $image[$index+2] = $exe.Count; $image[$index+3] = [uint32]$sum
  for ($i=0; $i -lt $name.Length; ++$i) { $image[$index+4+$i] = [uint32][char]$name[$i] }
  $image[$index+36] = $item.Parent; $image[$index+37] = $reserve
  $listing += [ordered]@{ Name=$item.Path; Type=$item.Kind; ID=($listing.Count+1); Parent=$item.Parent; OffsetWords=$offset; Words=$exe.Count; CapacityWords=$reserve; Checksum=('{0:X8}' -f $sum) }
  $image.AddRange($exe)
  for ($i=$exe.Count; $i -lt $reserve; ++$i) { $image.Add(0) }
}
$header = @(0x41534653,2,16,$image.Count,$listing.Count,16,40,$dataOffset,3)
for ($i=0; $i -lt $header.Count; ++$i) { $image[$i] = [uint32]$header[$i] }
New-Item -ItemType Directory -Force -Path (Split-Path -Parent $output) | Out-Null
$text = New-Object Text.StringBuilder
[void]$text.Append("v2.0 raw`n")
for ($i=0; $i -lt $image.Count; ++$i) {
  [void]$text.Append($image[$i].ToString('X8'))
  [void]$text.Append($(if (($i % 16) -eq 15 -or $i -eq $image.Count-1) { "`n" } else { ' ' }))
}
$utf8 = New-Object Text.UTF8Encoding($false)
[IO.File]::WriteAllText($output, $text.ToString(), $utf8)
$report = [ordered]@{ Format='ASFS'; Version=2; BaseAddress='0x0B000000'; Words=$image.Count;
  SHA256=(Get-FileHash -LiteralPath $output -Algorithm SHA256).Hash; Files=$listing }
[IO.File]::WriteAllText([IO.Path]::ChangeExtension($output, '.json'), ($report | ConvertTo-Json -Depth 5) + "`n", $utf8)
Write-Output "Built storage: $output ($($image.Count) words, $($listing.Count) entries)"
