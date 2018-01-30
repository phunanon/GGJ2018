#ifndef HPP_SOUND_INCLUDE
#define HPP_SOUND_INCLUDE

#include <SFML/Audio.hpp>
#include "math.hpp"

#define SOUNDCHANNELS 12
#define EARSHOT 16

#define AUD_SHOOT         0
#define AUD_VILLAGER_HURT 1
#define AUD_ZOMBIE_HURT   2
#define AUD_ZOMBIE_DIE    3

void playSound (uint8_t sound, float pitch, double orgX, double orgY, double protag_X, double protag_Y);

void initSound ();

#endif
