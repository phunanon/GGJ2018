#ifndef HPP_MAP_INCLUDE
#define HPP_MAP_INCLUDE

#include <string.h> //For memset()

#include "math.hpp"

#define B_WATER 0
#define B_STONE 1
#define B_GRASS 2
#define B_SAND  3

#define S_CAMPFIRE 3
#define S_TREE     4
#define S_BUSH     5

const uint16_t MAP_W = 320, MAP_H = 320;
const uint32_t MAP_A = MAP_W * MAP_H;
                            //eeeeeeee eeeeeeee     llaf ffssssbb
extern uint32_t map[MAP_W][MAP_H]; //00000000 00000000 00000000 00000000 - 0000000000000000 entity map id, 0000 RESERVED, 00 luminosity, 0 animated, 000 frame, 0000 sprite, 00 biome
extern uint64_t game_time;

//Constants
const uint8_t GEN_ISLANDS = 4;
const uint8_t GEN_ISLAND_RAD_MIN = 32;
const uint16_t GEN_ISLAND_RAD_MAX = 64;
const uint8_t GEN_ISLAND_RES = 4; //'resolution' of an island - how many blobs make it up
const uint8_t GEN_VILLAGES = 32;
const uint8_t GEN_VILLAGE_RAD_MIN = 8;
const uint16_t GEN_VILLAGE_RAD_MAX = 12;
const uint16_t GEN_GROW_MAP = 8192;
const uint16_t MAP_GROW_SPEED = 128;
const uint16_t MAP_DEATH_SPEED = 32;

bool inBounds (uint16_t x, uint16_t y);


uint8_t getBiome (uint16_t x, uint16_t y);
void setBiome (uint16_t x, uint16_t y, uint8_t b);
uint8_t getSprite (uint16_t x, uint16_t y);
void setSprite (uint16_t x, uint16_t y, uint8_t s);
uint8_t getFrame (uint16_t x, uint16_t y);
void setFrame (uint16_t x, uint16_t y, uint8_t f);
bool getAnimated (uint16_t x, uint16_t y);
void setAnimated (uint16_t x, uint16_t y, bool a);
uint8_t getLux (uint16_t x, uint16_t y);
void setLux (uint16_t x, uint16_t y, uint8_t l, bool append = false);
uint16_t getMapEntity (uint16_t x, uint16_t y);
void setMapEntity (uint16_t x, uint16_t y, uint16_t e);

bool isFoliage (uint8_t sprite_code);
bool isSolid (uint8_t sprite_code);


void pushCrate (uint16_t x, uint16_t y, double dx, double dy);

void growMap (uint16_t grow_speed = MAP_GROW_SPEED, uint16_t death_speed = MAP_DEATH_SPEED);

void genMap ();


#endif
