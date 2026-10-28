# Screen Gomoku regression

```powershell
.\build-tools\build.ps1 -Program gomoku-screen
python .\tests\gomoku-screen\run_gomoku_screen.py
```

Executes actual kernel/user BINs using the ideal ISA/rectangle/bitmap model. Not Digital.
Checks pixel-level wood/black/white palette, solid disks against an independent
circle equation, two 10x5 transparent masks/80 written pixels per disk, grid preservation,
updates restricted to the new piece/player digit, four-line row/column selection highlight
and its restoration without damaging pieces, both coordinate
orders/cases, invalid/occupied cells, backspace/Esc, CR/LF, Q, both players,
one startup terminal help message, absence of the on-screen quit hint,
row/column/both diagonal wins and a full-board draw. Normal play is terminal-silent.
Winning lines get exactly five red 4x4 center marks; unrelated pieces and grid remain
unchanged. Covers bottom/right boundaries and simultaneous row/column wins (first
matching line in the existing scan wins marking priority). Draw/quit add no marks.
Normal second-coordinate processing uses seven submissions, including one opaque
player digit bitmap; original font reference checks remain active.
Uses real tick scheduling and queued keyboard IRQs, checks full callback restoration,
slow drawing and graphics timeout without issuing commands while busy.
