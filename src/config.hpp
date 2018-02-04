#ifndef HPP_CONFIG_INCLUDE
#define HPP_CONFIG_INCLUDE

const uint16_t MAP_W = 320, MAP_H = 320;
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
const uint16_t GEN_VILLAGERS = 1024;
const uint16_t GEN_ZOMBIES = 256;

const uint16_t WINDOW_W = 1024, WINDOW_H = 512;
const float ANI_INTERVAL = 2;
const uint8_t SOUND_INTERVAL = 40;
const uint8_t HURT_ANI_LEN = 10;
const float PROJECTILE_FADE_SPEED = .01;
const float CORPSE_FADE_SPEED = 0.005;

const uint8_t SHOOT_DISTANCE = 12;
const uint8_t ATTACK_DISTANCE = 6;
const uint8_t LASHOUT_INTERVAL = 20;
const uint8_t MAX_HEALTH = 255;
const float NORMAL_SPEED = .032, ATTACK_SPEED = .08;
const uint8_t PROJECTILE_DAMAGE = 10;
const uint8_t ATTACK_DAMAGE = 45;
const float PROJECTILE_SPEED = .32;
const float LUX_HEAL = .2;
const uint8_t KILL_HP_REWARD = 20;
const uint8_t CAMPFIRE_DAMAGE = 20;

const float THINK_CHANCE = 0.04;
const uint16_t PROT_RELOAD_TIME = 10;

#endif
