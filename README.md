

## BREAKOUT PROTOTYPE

Simple example of a breakout game, using SDL and Entt. It uses a pure ECS architecture, where systems are free functions.

# Building
Make sure to run git submodules to download EnTT, and use CMAKE. Point cmake to SDL 2.0.8 and SDL_Image 2.0.4 folders. The project has a CMakeSettingJson compatible with visual studio, you can change the paths there.
Only tested in visual studio.

# Architecture
* 2d math vectors in vmath, 
* bitmap font loader, which loads BMFont fonts in bitfont
* rendering functions on sdl_render
* gameplay code for the breakout game in components.h and main.cpp

You can follow the code from main.cpp, in the sdl iteration loop its calling each of the functions and sending the ecs registry, which holds the game state
The game uses a pure ECS aproach, so all game objects are created through composition. Systems are all called in order, and only read from specific component sets.
Ej: Renderer only needs SDL_RenderSprite to render, and movement system uses MovementComponent + SpriteLocation to move things.

Entities:
* Player: PlayerInput, SDL_RenderSprite, SpriteLocation,MovementComponent
* Ball: Ball, SDL_RenderSprite,SpriteLocation,MovementComponent
* Bricks: Brick, SDL_RenderSprite, SpriteLocation

Components
* PlayerInput: Holds directional input axis
* SDL_RenderSprite: Holds all the data needed to render a single sprite.
* SpriteLocation: Holds 2d location coordinates in game-space
* MovementComponent: Moves the entity according to velocity and acceleration
* Ball: Tag component, used for filtering the ball
* Brick: Used as tag component, also holds score points of the brick.



# Third Party
* SDL and SDL_Image for rendering: https://www.libsdl.org/
* EnTT for the ECS framework: https://github.com/skypjack/entt
* Project made from a CMake template with heavy modification: https://github.com/kigster/cmake-project-template
* Bitmap font generated from BMFont: http://www.angelcode.com/products/bmfont/
* 2d assets taken from Kenney: https://www.kenney.nl/