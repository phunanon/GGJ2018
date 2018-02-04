#ifndef HPP_MAP_INCLUDE
#define HPP_MAP_INCLUDE

#include <string.h> //For memset()

#include "math.hpp"
#include "config.hpp"

#define B_WATER 0
#define B_STONE 1
#define B_GRASS 2
#define B_SAND  3

#define S_CRATE    1
#define S_WALL     2
#define S_CAMPFIRE 3
#define S_TREE     4
#define S_BUSH     5

const uint32_t MAP_A = MAP_W * MAP_H;
                            //eeeeeeee eeeeeeee     llaf ffssssbb
extern uint32_t map[MAP_W][MAP_H]; //00000000 00000000 00000000 00000000 - 0000000000000000 entity map id, 0000 RESERVED, 00 luminosity, 0 animated, 000 frame, 0000 sprite, 00 biome
extern uint64_t game_time;

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
bool isCompletelySolid (uint8_t sprite_code);
bool isntWalkable (uint8_t sprite_code);


void tryPushCrate (uint16_t x, uint16_t y, float dx, float dy);

void growMap (uint16_t grow_speed = MAP_GROW_SPEED, uint16_t death_speed = MAP_DEATH_SPEED);

void genMap ();

uint8_t raycastBlocking (float pos_X, float pos_Y, float targ_X, float targ_Y, float dist, bool (*blockTester)(uint8_t));


#endif
