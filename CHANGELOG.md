Asset to use : Asset-BETA, available [here](https://drive.google.com/file/d/1DI9XeGJRRVNhXxdgb0sLzFiAnMjx-_gn/view?usp=sharing)


# 23/11/2020 - GOLD tag

Final, functional version of Mini Company!
Build accessible [here](https://drive.google.com/file/d/1sQUQ4cDS3Eyi7E8jnirtoJ_mVDrKmtHW/view?usp=sharing).

To edit Mini Company from Unreal Engine 4, **the assets must be downloaded from [here](https://drive.google.com/file/d/1qL_-rYyKfeAV9iPbi2r2JTjly_UFpkJr/view?usp=sharing)** (Content-GOLD.zip).
Extract the .zip to the root of the repo, so the folders `Content/Asset` and `Content/Movies` appear after extraction.

- Combat has been heavily balanced
- Formations now have improved, equal, or reduced damage against other formations
- Many visual and audio feedbacks have been added
- Refined HUD
- Improved menus, now has more options
- Introduction video and credits have been added
- All required sounds have been implemented
- Fixed many small or gamebreaking bugs
- Cleaned assets
- Polished animations

**/!\\** Known bugs:
- While attacking, getting hit by an enemy can *occasionally* prevent the player from attacking. Switch to circle formation, and raise your company's shields to come back to normal
- Spamming formation switch, melee attacks and special attacks will lock the line special attack


# 13/11/2020 - BETA tag

Playable version, smoothened and free of many previous bugs.
Build accessible [here](https://drive.google.com/file/d/1l5EYwzR6PGh_QrgGe2zR6XFVWOmQWmEo/view?usp=sharing).

- Added a playable level (Content/Level/LevelMap.umap)
- Proper game loop ("Victory" and "Game over" screens to come)
- Overhaul of health system of formations
- Intermediate checkpoint saves
- Combat system revised and balancing
- Chained melee attacks
- Menus (start menu to come) and in-game HUD
- First sounds and extra animations and animation blending/layering
- Improved controls
- Many bug fixes


# 16/10/2020 - ALPHA tag

Partial implentation of 3C, along some secondary features.

## Features
- The player controls a company of soldiers
- The company can assume different formations: circle, triangle, or line (through the gamepad's left and right triggers)
- The company is capable of moving as a group (left joystick)
- The company can attack and damage enemies (gamepad left face button)
- The company can block attacks by raising its shields, at the cost of reduced movement speed (note: still does not fully work) (gamepad right face button)
- The company can dash while in triangle formation (other special abilities coming for the other formations) (top button on the right part of the gamepad)
- Enemy companies are guided by a simple AI, which comes in range of the player, stays distant, waits, than repeats this cycle
- The camera can be controlled (right joystick)
- The camera can stay locked in a direction (gamepad right shoulder)