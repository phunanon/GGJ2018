# TODO

## Must do
- Fix Sprite/Entity overlapping in display
- Sprite/Entity enums used exclusively over numbers
- Sandardise and improve collision detection so Entities can't pass through gaps between Sprites
- Employ display.hpp and Entity.hpp
- Enable deletion of Entities (don't rely on index_in_array - a vestige of GGJ)
- Investigate bias NW loiter (note: testing ri() showed no particular bias)

## Should do
- Background music
- Zombie bite animation
- Write CREDITS.md
- Blood animation when attacked by zombie
- Enable protag zombie mode
- Fix Villager animations looking backwards at certain angles
- Mouse crosshair
- Improve crate sprite
- Delete the faded out dead
- Improve apparent protag gun accuracy

## Could do
- Animated foliage
- Protag torch
- Fog
- Crate destruction
- Stop shooting/movement unless mouse is in window
- Add granade which can be thrown over Sprites
- More variation in sounds
- Change protag indicator to above-head arrow
- Pause
- Zombies start in one part of the map and spread, protag near this (tested, is nice, but bias loiter is blocking)
- Slide instead of stop on Entity/Sprite collision
- Test protag random gun accuracy
- -Wall compliance and -O3 testing

## Won't do
- Crate push pressure
