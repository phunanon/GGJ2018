#include <string.h> //For memset()

#include "map.hpp"

//eeeeeeee eeeeeeee     llaf ffssssbb
uint32_t map[MAP_W][MAP_H]; //00000000 00000000 00000000 00000000 - 0000000000000000 entity map id, 0000 RESERVED, 00 luminosity, 0 animated, 000 frame, 0000 sprite, 00 biome
uint64_t game_time = 100;

bool inBounds (uint16_t x, uint16_t y)
{
    return x > 0 && x < MAP_W && y > 0 && y < MAP_H;
}

uint8_t getBiome (uint16_t x, uint16_t y) { if (inBounds(x, y)) {
    return map[x][y] & 0x03;
} }
void setBiome (uint16_t x, uint16_t y, uint8_t b) { if (inBounds(x, y)) {
    map[x][y] = (map[x][y] & 0xFFFFFFFC) | b;
} }
uint8_t getSprite (uint16_t x, uint16_t y) { if (inBounds(x, y)) {
    return (map[x][y] & 0x3C) >> 2;
} }
void setSprite (uint16_t x, uint16_t y, uint8_t s) { if (inBounds(x, y)) {
    map[x][y] = (map[x][y] & 0xFFFFFFC3) | (s << 2);
} }
uint8_t getFrame (uint16_t x, uint16_t y) {  if (inBounds(x, y)) {
    return (map[x][y] & 0x1C0) >> 6;
} }
void setFrame (uint16_t x, uint16_t y, uint8_t f) { if (inBounds(x, y)) {
    map[x][y] = (map[x][y] & 0xFFFFFE3F) | (f << 6);
} }
bool getAnimated (uint16_t x, uint16_t y) {  if (inBounds(x, y)) {
    return (map[x][y] & 0x200) >> 9;
} }
void setAnimated (uint16_t x, uint16_t y, bool a) { if (inBounds(x, y)) {
    map[x][y] = (map[x][y] & 0xFFFFFDFF) | (a << 9);
} }
uint8_t getLux (uint16_t x, uint16_t y) {  if (inBounds(x, y)) {
    return (map[x][y] & 0xC00) >> 10;
} }
void setLux (uint16_t x, uint16_t y, uint8_t l, bool append) { if (inBounds(x, y)) {
    if (append) { if (getLux(x, y) > l) { return; } }
    map[x][y] = (map[x][y] & 0xFFFFF3FF) | (l << 10);
} }
uint16_t getMapEntity (uint16_t x, uint16_t y) { if (inBounds(x, y)) {
    return (map[x][y] & 0xFFFF0000) >> 16;
} }
void setMapEntity (uint16_t x, uint16_t y, uint16_t e) { if (inBounds(x, y)) {
    map[x][y] = (map[x][y] & 0x0000FFFF) | (e << 16);
} }

bool isFoliage (uint8_t sprite_code)
{
    switch (sprite_code) {
        case 4: case 5: return true;
        default: return false;
    }
}
bool isCompletelySolid (uint8_t sprite_code)
{
    switch (sprite_code) {
        case S_WALL: case S_CRATE: return true;
        default: return false;
    }
}
bool isntWalkable (uint8_t sprite_code)
{
    switch (sprite_code) {
        case S_WALL: case S_CRATE: case S_CAMPFIRE: return true;
        default: return false;
    }
}



void tryPushCrate (uint16_t x, uint16_t y, float dx, float dy)
{
    if (fabs(dx) > fabs(dy)) { dx = (dx > 0 ? 1 : -1); dy = 0; } else { dx = 0; dy = (dy > 0 ? 1 : -1); }
    uint16_t px = x + int16_t(dx);
    uint16_t py = y + int16_t(dy);
    if (getSprite(x, y) == 1 && !getSprite(px, py) && getBiome(px, py) == B_STONE) {
        setSprite(x, y, 0);
        setSprite(px, py, 1);
    }
}


void growMap (uint16_t grow_speed, uint16_t death_speed);

void genMap ()
{
  //Set map to water
    memset(map, B_WATER, sizeof map);
  //Generate islands
  //Generated by randomly placing differently sized circles (blobs) in a group
    for (uint8_t i = 0; i < GEN_ISLANDS; ++i) { //For each island
      //Calc island size and pos
        const uint16_t island_radius = ri(GEN_ISLAND_RAD_MIN, GEN_ISLAND_RAD_MAX);
        uint16_t island_X, island_Y;
        do {
            random_coord(MAP_W, MAP_H, island_X, island_Y);
        } while (getBiome(island_X, island_Y) == B_WATER
            && island_X < island_radius * 2.2 || island_Y < island_radius * 2.2 || island_X > MAP_W - (island_radius * 2.2) || island_Y > MAP_H - (island_radius * 2.2));
      //Calc blob size and pos
        uint16_t blob_X = island_X + ri(island_radius / 2, island_radius);
        uint16_t blob_Y = island_Y + ri(island_radius / 2, island_radius);

        for (uint8_t b = 0; b < GEN_ISLAND_RES; ++b) { //For each blob in the island
            uint16_t blob_X = island_X + ri(-island_radius, island_radius);
            uint16_t blob_Y = island_Y + ri(-island_radius, island_radius);
          //Go through all angles from 0 to 2 * PI radians, in an ever-smaller circle (size), to fill the blob
            float size = 1.0;
            const float step = .005;
            while (size > step) {
                float r = size * island_radius;
                for (float ang = 0; ang < 6.28; ang += .01) {
                    uint16_t x = blob_X + r * sinf(ang);
                    uint16_t y = blob_Y + r * cosf(ang);
                  //Place some land there
                    setBiome(x, y, B_GRASS);
                }
                size -= step;
            }
          //Go through all angles and add a coast of sand
            for (float ang = 0; ang < 6.28; ang += .01) {
                uint16_t x = blob_X + island_radius * sinf(ang);
                uint16_t y = blob_Y + island_radius * cosf(ang);
              //Place some land there
                setBiome(x, y, B_SAND);
            }
        }
    }
  //Generate villages
    for (uint8_t v = 0; v < GEN_VILLAGES; ++v) { //For each village
      //Calc island size and pos
        const uint16_t village_radius = ri(GEN_VILLAGE_RAD_MIN, GEN_VILLAGE_RAD_MAX);
        uint16_t village_X, village_Y;
        do {
            random_coord(MAP_W, MAP_H, village_X, village_Y);
        } while (getBiome(village_X, village_Y) != B_GRASS);
      //Go through all angles from 0 to 2 * PI radians, in an ever-smaller circle (size), to fill the village biome
        float size = 1.0;
        const float step = .005;
        while (size > step) {
            float r = size * village_radius;
            for (float ang = 0; ang < 6.28; ang += .01) {
                uint16_t x = village_X + r * sinf(ang);
                uint16_t y = village_Y + r * cosf(ang);
                setBiome(x, y, B_STONE);
            }
            size -= step;
        }
      //Go through all angles and add a brick wall
        for (float ang = 0; ang < 6.28; ang += .075) { //wide angle to cause some gaps
            uint16_t x = village_X + village_radius * sinf(ang);
            uint16_t y = village_Y + village_radius * cosf(ang);
            setSprite(x, y, 2);
        }
    }
  //En-masse
    for (uint16_t y = 1; y < MAP_H - 1; ++y) {
        for (uint16_t x = 1; x < MAP_W - 1; ++x) {
          //Set animation properties (idk why I even have to do this)
            setFrame(x, y, 0);
            setAnimated(x, y, false);
          //Prune strays
            //Remove all brick walls surrounded by stone biome (where village blobs have overlapped)
            if (getSprite(x, y) == 2 && getBiome(x+1, y) == B_STONE && getBiome(x-1, y) == B_STONE && getBiome(x, y+1) == B_STONE && getBiome(x, y-1) == B_STONE) {
                setSprite(x, y, 0);
            }
            //Remove all sand not touching water (where island blobs have overlapped)
            if (getBiome(x, y) == B_SAND) {
                bool wet = false;
                wet |= getBiome(x+1, y) == B_WATER;
                wet |= getBiome(x-1, y) == B_WATER;
                wet |= getBiome(x, y+1) == B_WATER;
                wet |= getBiome(x, y-1) == B_WATER;
                if (!wet) {
                    setBiome(x, y, B_GRASS);
                }
            }
            uint8_t biome_code = getBiome(x, y);
            if (biome_code == B_STONE && rb(.05) && !getSprite(x, y)) {
              //Add random fireplace and luminosity
                setSprite(x, y, 3);
                setAnimated(x, y, true);
                setFrame(x, y, ri(0, 6));
                setLux(x+0,y+0,3, true);
                setLux(x+1,y+0,2, true);
                setLux(x+2,y+0,1, true);
                setLux(x+3,y+0,1, true);
                setLux(x-1,y+0,2, true);
                setLux(x-2,y+0,1, true);
                setLux(x-3,y+0,1, true);
                setLux(x+0,y+1,2, true);
                setLux(x+1,y+1,2, true);
                setLux(x+2,y+1,1, true);
                setLux(x-1,y+1,2, true);
                setLux(x-2,y+1,1, true);
                setLux(x+0,y+2,1, true);
                setLux(x+1,y+2,1, true);
                setLux(x-1,y+2,1, true);
                setLux(x+0,y+3,1, true);
                setLux(x+0,y-1,2, true);
                setLux(x+1,y-1,2, true);
                setLux(x+2,y-1,1, true);
                setLux(x-1,y-1,2, true);
                setLux(x-2,y-1,1, true);
                setLux(x+0,y-2,1, true);
                setLux(x+0,y-3,1, true);
                setLux(x+1,y-2,1, true);
                setLux(x-1,y-2,1, true);
            }
            if (biome_code == B_STONE && rb(.05) && !getSprite(x, y)) {
                setSprite(x, y, 1); //Random crate
            }
          //Add random foliage
            else if (biome_code == B_GRASS) {
                if (rb(0.02)) {
                    setSprite(x, y, S_TREE);
                    //setAnimated(x, y, true);
                } else if (rb(0.01)) {
                    setSprite(x, y, S_BUSH);
                    //setAnimated(x, y, true);
                }
            }
          //Add random brick
            if (rb(0.005) && getBiome(x, y) != B_WATER && !getSprite(x, y)) {
                setSprite(x, y, 2);
            }
        }
    }
  //Grow the map naturally for a bit
    for (uint16_t g = 0; g < GEN_GROW_MAP; ++g) {
        growMap();
    }
}



void growMap (uint16_t grow_speed, uint16_t death_speed)
{
  //Giveuth
    for (uint16_t g = 0; g < grow_speed; ++g) {
      //Select a random position on the map
        uint16_t grow_X, grow_Y;
        random_coord(MAP_W, MAP_H, grow_X, grow_Y);
        uint8_t sprite_code = getSprite(grow_X, grow_Y);
      //If it's growable, grow it
        uint8_t spread = 0;
        switch (sprite_code) {
            case S_TREE: spread = 1 * rb(.3); break;
            case S_BUSH: spread = 2.5 * rb(.8); break;
        }
        if (spread) {
            grow_X += ri(-spread, spread);
            grow_Y += ri(-spread, spread);
            if (!getSprite(grow_X, grow_Y) && getBiome(grow_X, grow_Y) == B_GRASS) {
                setSprite(grow_X, grow_Y, sprite_code);
            }
        }
    }
  //Takeuth awae
    for (uint16_t d = 0; d < death_speed; ++d) {
      //Select a random position on the map
        uint16_t grow_X, grow_Y;
        random_coord(MAP_W, MAP_H, grow_X, grow_Y);
        uint8_t sprite_code = getSprite(grow_X, grow_Y);
      //If it's killable, kill it
        bool to_kill = 0;
        switch (sprite_code) {
            case 4: to_kill = rb(.3); break;
            case 5: to_kill = rb(.8); break;
        }
        if (to_kill) {
            setSprite(grow_X, grow_Y, 0);
        }
    }
}


//http://lodev.org/cgtutor/raycasting.html
//Returns blocking Sprite
uint8_t raycastBlocking (float pos_X, float pos_Y, float targ_X, float targ_Y, float dist, bool (*blockTester)(uint8_t))
{
    float dir_X, dir_Y;
    targToVec(pos_X, pos_Y, targ_X, targ_Y, dir_X, dir_Y);

    //Length of ray from current position to next x- or y-side
    float side_dist_X = 0, side_dist_Y = 0;

    //Length of ray from one x- or y-side to next x- or y-side
    const float delta_X = sqrt_approx(1 + (dir_Y * dir_Y) / (dir_X * dir_X));
    const float delta_Y = sqrt_approx(1 + (dir_X * dir_X) / (dir_Y * dir_Y));
    float perpWallDist;

    // what direction to step in x or y-direction (either +1 or -1)
    int8_t step_X = 0, step_Y = 0;
    //Which box of the map we're in
    uint16_t check_X = pos_X;
    uint16_t check_Y = pos_Y;

    if (dir_X < 0) {
        step_X = -1;
        side_dist_X = (pos_X - check_X) * delta_X;
    } else {
        step_X = 1;
        side_dist_X = (check_X + 1.0 - pos_X) * delta_X;
    }

    if (dir_Y < 0) {
        step_Y = -1;
        side_dist_Y = (pos_Y - check_Y) * delta_Y;
    } else {
        step_Y = 1;
        side_dist_Y = (check_Y + 1.0 - pos_Y) * delta_Y;
    }

  //Perform DDA
    uint8_t check_sprite = 0;
    while (true) {
      //Check if ray has reached its max requested range
        if (dist < eD_approx(pos_X, pos_Y, check_X, check_Y)) { return 0; }
      //Jump to next map square in x- or y-direction
        if (side_dist_X < side_dist_Y){
            side_dist_X += delta_X;
            check_X += step_X;
        } else {
            side_dist_Y += delta_Y;
            check_Y += step_Y;
        }
      //Check if ray has hit a solid
        check_sprite = getSprite(check_X, check_Y);
        if ((*blockTester)(check_sprite)) { break; }
    }
    return check_sprite;
}
