# Screen Maze regression

```powershell
.\build-tools\build.ps1 -Program maze-screen
python .\tests\maze-screen\run_maze_screen.py
```

Executes actual kernel/user BINs with ideal ISA/rectangle hardware, not Digital.
Checks connected room/corridor layout, boundary walls, player and start restoration,
dirty-cell movement, blocked movement with no draw, restart without regeneration,
several distinct new mazes, automatic path traversal to the exit, Q exit, and
full callback context restoration with normal tick scheduling. No graphics IRQ.
Also checks one startup help message, terminal silence during movement/restart/new
maze, and exactly one steps/time report on either successful escape or Q exit.
