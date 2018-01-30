#include <SFML/Graphics.hpp> //http://www.sfml-dev.org/tutorials/2.4
#include <iostream> //Terminal

#include "display.cpp"
#include "math.hpp"

const uint16_t RELOAD_TIME = 10;


int main ()
{
  //Create the window
    sf::RenderWindow window(sf::VideoMode(WINDOW_W, WINDOW_H), "Infection");
  //Display rendering loading screen


  //Declare asset thingies
    sf::Font fnt_arial;
    sf::Image biomeTexImg;
    sf::Texture biomeTexture;
    sf::Image spriteTexImg;
    sf::Texture spriteTexture;
    sf::Image entityTexImg;
    sf::Texture entityTexture;
    sf::Image villagerTexImg;
    sf::Texture villagerTexture;
    sf::Image zombieTexImg;
    sf::Texture zombieTexture;
  //Load textures/fonts
    fnt_arial.loadFromFile("assets/arial.ttf");
    biomeTexImg.loadFromFile("assets/biomes.png");
    spriteTexImg.loadFromFile("assets/sprites.png");
    villagerTexImg.loadFromFile("assets/villager.png");
    zombieTexImg.loadFromFile("assets/zombie.png");


    txt_float.setFont(fnt_arial);
    txt_float.setCharacterSize(8);
    txt_float.setFillColor(sf::Color::Black);
    txt_HUD.setFont(fnt_arial);
    txt_HUD.setCharacterSize(20);
    txt_HUD.setPosition(sf::Vector2f(4, WINDOW_H - 24));
    biomeTexture.loadFromImage(biomeTexImg);
    biomeTile.setTexture(biomeTexture);
    biomeTexture.setSmooth(false);
    spriteTexture.loadFromImage(spriteTexImg);
    spriteTile.setTexture(spriteTexture);
    spriteTexture.setSmooth(false);
    villagerTexture.loadFromImage(villagerTexImg);
    villagerTile.setTexture(villagerTexture);
    villagerTexture.setSmooth(false);
    zombieTexture.loadFromImage(zombieTexImg);
    zombieTile.setTexture(zombieTexture);
    zombieTexture.setSmooth(false);
    projectileTile.setFillColor(sf::Color::Red);
  //Init Minimap
    mm_tex.create(MAP_W, MAP_H);
    minimap.scale(float(mm_size) / float(MAP_W), float(mm_size) / float(MAP_H));
    minimap.setOutlineThickness(1);
    minimap.setOutlineColor(sf::Color(0, 0, 0));
    minimap.setPosition(sf::Vector2f(0, mm_diag_width / 2));
    minimap.setRotation(-45);

  //Init sound
    initSound();

  //Generate map
    genMap();

  //Dummy Entity
    entity.push_back(new Entity());
  //Spawn Villagers
    for (uint16_t v = 0; v < GEN_VILLAGERS; ++v) {
        uint16_t x, y;
        do {
            x = ri(0, MAP_W);
            y = ri(0, MAP_H);
        } while (getBiome(x, y) != B_STONE || getSprite(x, y));
        entity.push_back(new Entity(0, x, y));
    }
    prot = entity[1];
  //Spawn Zombies
    for (uint16_t z = 0; z < GEN_ZOMBIES; ++z) {
        uint16_t x, y;
        do {
            x = ri(0, MAP_W);
            y = ri(0, MAP_H);
        } while (getBiome(x, y) == B_STONE || getBiome(x, y) == B_WATER || getSprite(x, y));
        entity.push_back(new Entity(1, x, y));
    }
  //Move player to suitable location (stone)
    do {
        prot->pos_X = ri(0, MAP_W);
        prot->pos_Y = ri(0, MAP_H);
    } while (getBiome(prot->pos_X, prot->pos_Y) != B_STONE || getSprite(prot->pos_X, prot->pos_Y));

    uint64_t prev_shot = 0;
  //Start game-loop
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
          if (event.type == sf::Event::Closed)
              window.close();
        }

      //Check mouse
        sf::Vector2f mouse_pos = sf::Vector2f(sf::Mouse::getPosition(window));
		mouse_pos.x = (float(WINDOW_W) / float(window.getSize().x)) * float(mouse_pos.x);  //
		mouse_pos.y = (float(WINDOW_H) / float(window.getSize().y)) * float(mouse_pos.y); // Adjust for scaled window
        prot->rot = normaliseAng(vecToAng((mouse_pos.x - float(WINDOW_W / 2)) / 2, mouse_pos.y - float(WINDOW_H / 2)) + 45);
      //Check keyboard and mouse
        float dir_X = 0, dir_Y = 0;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { //Move protag foward (NW)
            angToVec(prot->rot, dir_X, dir_Y);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) { //Move protag right (NE)
            angToVec(prot->rot + 90, dir_X, dir_Y);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) { //Move protag backward (SE)
            angToVec(prot->rot + 180, dir_X, dir_Y);
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) { //Move protag left (SW)
            angToVec(prot->rot + 270, dir_X, dir_Y);
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && prev_shot + RELOAD_TIME < game_time) {
            prot->shootDir();
            prev_shot = game_time;
        }
      //Speed based on mouse distance from the center
        if (mouse_pos.x > WINDOW_W*.75) { mouse_pos.x = WINDOW_W*.75; }
        if (mouse_pos.x < WINDOW_W*.25) { mouse_pos.x = WINDOW_W*.25; }
        float dist = eD_approx(WINDOW_W/2, WINDOW_H/2, mouse_pos.x, mouse_pos.y) / (WINDOW_W/2);
        dir_X *= dist;
        dir_Y *= dist;
      //Set protag data
        if (fabs(dir_X) + fabs(dir_Y) > .1) {
            prot->had_moved = true;
            prot->speed = dist / 8;
        }
        double new_X = prot->pos_X + dir_X/6;
        double new_Y = prot->pos_Y + dir_Y/6;
        if (prot->tryDir(dir_X, dir_Y)) {
            prot->pos_X = new_X;
            prot->pos_Y = new_Y;
            setMapEntity(prot->pos_X, prot->pos_Y, 1);
        }
        prot->rot = prot->rot;

      //DISPLAY
        doDISPLAY(game_time, window, !(game_time % 50));

      //Entity & Projectile stuff
      //Entities
        bool is_nighttime = sky_darkness < .4;
        uint16_t ents = 0, humans = 0, zombies = 0;
        for (uint16_t e = 2, elen = entity.size(); e < elen; ++e) {
            Entity* ent = entity[e];
            ent->animate();
            if (ent->is_dead) { continue; }
            if (rb(0.02)) { ent->think(is_nighttime); }
            ent->move();

            ++ents;
            if (ent->type == E_VILLAGER) { ++humans; }
             else if (ent->type == E_ZOMBIE) { ++zombies; }
        }
        //Heal ALL Villagers if they're near a campfire
        for (uint16_t e = 1, elen = entity.size(); e < elen; ++e) {
            if (entity[e]->type != E_VILLAGER) { continue; }
            float heal_amount = getLux(entity[e]->pos_X, entity[e]->pos_Y) * LUX_HEAL;
            if (heal_amount > 0) { entity[e]->heal(heal_amount); }
        }
      //Projectiles
        for (uint16_t p = 0, plen = projectile.size(); p < plen; ++p) {
            Projectile* proj = projectile[p];
            proj->opacity -= .01;
            if (proj->opacity <= 0) {
                projectile.erase(projectile.begin() + p);
                --p;
                continue;
            }
            if (!proj->had_hit) { proj->move(); }
        }
        prot->animate();


      //HUD
        txt_HUD.setString("Human: "+ std::to_string(humans) +", zombie: "+ std::to_string(zombies) +"; "+ to_string_with_precision(float(zombies)/ents*100) +"% infected");
        sf::sleep(sf::milliseconds(10));
        ++game_time;
    }

    return 0; //End the program
}
