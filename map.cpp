#include <string.h> //For memset()

#include "math.cpp"


const uint16_t MAP_W = 1024, MAP_H = 1024;
const uint32_t MAP_A = MAP_W * MAP_H;
uint16_t map[MAP_W][MAP_H]; //00000000-00000000 - 0000 frame, 0000 sprite, 00 biome

//Consonants
const uint8_t GEN_ISLANDS = 16;
const uint16_t GEN_ISLAND_RAD_MIN = 64;
const uint16_t GEN_ISLAND_RAD_MAX = 128;
const uint8_t GEN_ISLAND_RES = 4; //'resolution' of an island - how many blobs make it up
const uint8_t GEN_VILLAGES = 32;
const uint16_t GEN_VILLAGE_RAD_MIN = 16;
const uint16_t GEN_VILLAGE_RAD_MAX = 32;

uint8_t getBiome (uint8_t m) { return m & 0x03; }
void setBiome (uint16_t x, uint16_t y, uint8_t b) { if (x > 0 && x < MAP_W && y > 0 && y < MAP_H) {
    map[x][y] = (map[x][y] & 0xFFFC) | b;
} }
uint8_t getSprite (uint8_t m) { return (m & 0x3C) >> 2; }
void setSprite (uint16_t x, uint16_t y, uint8_t s) { if (x > 0 && x < MAP_W && y > 0 && y < MAP_H) {
    map[x][y] = (map[x][y] & 0xFFC3) | (s << 2);
} }

void genMap ()
{
  //Set map to water
    memset(map, 3, sizeof map);
  //Generate islands
  //Generated by randomly placing differently sized circles (blobs) in a group
    for (uint8_t i = 0; i < GEN_ISLANDS; ++i) { //For each island
      //Calc island size and pos
        const uint16_t island_radius = ri(GEN_ISLAND_RAD_MIN, GEN_ISLAND_RAD_MAX);
        uint16_t island_X, island_Y;
        random_coord(MAP_W, MAP_H, island_X, island_Y);
      //Calc blob size and pos
        uint16_t blob_X = island_X + ri(island_radius / 2, island_radius);
        uint16_t blob_Y = island_Y + ri(island_radius / 2, island_radius);

        for (uint8_t b = 0; b < GEN_ISLAND_RES; ++b) { //For each blob in the island
            uint16_t blob_X = island_X + ri(0, island_radius * 2);
            uint16_t blob_Y = island_Y + ri(0, island_radius * 2);
          //Go through all angles from 0 to 2 * PI radians, in an ever-smaller circle (size), to fill the blob
            float size = 1.0;
            const float step = .005;
            while (size > step) {
                float r = size * island_radius;
                for (float ang = 0; ang < 6.28; ang += .01) {
                    uint16_t x = blob_X + r * sinf(ang);
                    uint16_t y = blob_Y + r * cosf(ang);
                  //Place some land there
                    setBiome(x, y, 1);
                }
                size -= step;
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
        } while (getBiome(map[village_X][village_Y]) == 3);
      //Go through all angles from 0 to 2 * PI radians, in an ever-smaller circle (size), to fill the village biome
        float size = 1.0;
        const float step = .005;
        while (size > step) {
            float r = size * village_radius;
            for (float ang = 0; ang < 6.28; ang += .01) {
                uint16_t x = village_X + r * sinf(ang);
                uint16_t y = village_Y + r * cosf(ang);
                setBiome(x, y, 0);
            }
            size -= step;
        }
      //Go through all angles and add wall of crates
        for (float ang = 0; ang < 6.28; ang += .01) {
            uint16_t x = village_X + village_radius * sinf(ang);
            uint16_t y = village_Y + village_radius * cosf(ang);
            setSprite(x, y, 1);
        }
        size -= step;
    }
}
