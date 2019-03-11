# cis407-507-project-Blockgame

A block-dropping puzzle game inspired by tetris and beatmania. My first attempt at creating an actual, standalone game.

### Requirements
- SFML - https://www.sfml-dev.org/index.php (written with ver 2.5.1)

SFML configuration for project on Visual Studio 2017:
- Add `...\SFML-2.5.1\include` to C++ 'Additional Include Directories' in project properties
- Add `...\SFML-2.5.1\lib` to Linker 'Additional Library Directories' in project properties
- For 'debug,' add `sfml-graphics-d.lib`, sfml-window-d.lib`` and `sfml-system-d.lib` to 'Additional Dependencies' in Linker/Input in project properties
  - For 'release', instead use `sfml-graphics.lib`, `sfml-window.lib` and `sfml-system.lib`
- Copy the SFML corresponding .dlls for those libraries to the working directory
- Build and enjoy

### Planned features
For the second half of the project I would like to add:
  - ✅ Timer and auto-drop/strike/some sort of penalty when taking too long to drop piece
  - Mascot character that animates idly and reacts when player clears lines
  - ✅ Scoring system that rewards more difficult play (double line clears, back-to-back line clears)
  - Non-placeholder graphics
  - Visual effects when pieces drop and lines clear
  - ✅ A ranking system with increasing difficulty as you play longer
  
...but I may not get to all of it.

### Attribution
Programming/Art/Design - myself

Sansation font - Bernd Montag

SFML - https://www.sfml-dev.org/index.php
