#include <SFML/Audio.hpp>

#define SOUNDCHANNELS 12
#define EARSHOT 16

#define AUD_SHOOT         0
#define AUD_VILLAGER_HURT 1
#define AUD_ZOMBIE_HURT   2
#define AUD_ZOMBIE_DIE    3

//Create soundBuffer vector
std::vector<sf::SoundBuffer*> sound_buffers;
std::vector<sf::Sound*> sound_channels;

uint8_t curr_sch = 0;
void playSound (uint8_t sound, float pitch, double orgX, double orgY, double protag_X, double protag_Y) {
  //Load buffer
    sound_channels[curr_sch]->setBuffer(*sound_buffers[sound]);
  //Calculate volume, based on distance
    float dist = eD_approx(orgX, orgY, protag_X, protag_Y);
    if (dist > EARSHOT) { return; }
    uint8_t vol = ((pow(EARSHOT, 2) - pow(dist, 2)) / pow(EARSHOT, 2)) * 100;
    sound_channels[curr_sch]->setVolume(vol);
  //Unique pitch
    sound_channels[curr_sch]->setPitch(pitch);
  //Play
    sound_channels[curr_sch]->play();
  //Change channel
    ++curr_sch;
    if (curr_sch == SOUNDCHANNELS) { curr_sch = 0; }
}

void initSound ()
{
  //Init sound_buffers
    std::vector<std::string> sounds = {"gun_shot.ogg", "women_hurt.ogg", "zombiehurt.ogg", "zombiedie.ogg"};
    for (uint b = 0; b < sounds.size(); ++b) {
        sound_buffers.push_back(new sf::SoundBuffer);
        sound_buffers[b]->loadFromFile("assets/" + sounds[b]);
    }
    //Init sound_channels
    for (uint c = 0; c < SOUNDCHANNELS; ++c) {
        sound_channels.push_back(new sf::Sound);
    }
}
