#include <SFML/Graphics.hpp> //http://www.sfml-dev.org/tutorials/2.4
#include <iostream> //Terminal

#include "protag.cpp"
#include "display.cpp"


int main ()
{
  //Create the window
    sf::RenderWindow window(sf::VideoMode(WINDOW_W, WINDOW_H), "Infection");
  //Display rendering loading screen


  //Declare asset thingies
    sf::Font fnt_arial;
    sf::Image biomeTexImg;
    sf::Texture biomeTexture;
    sf::Sprite biomeTile;
    sf::Image spriteTexImg;
    sf::Texture spriteTexture;
    sf::Sprite spriteTile;
    sf::Image entityTexImg;
    sf::Texture entityTexture;
    sf::Sprite entityTile;
  //Load textures/fonts
    if (!fnt_arial.loadFromFile("Assets/arial.ttf"))
    {
      std::cout << "Couldn't load Assets/arial.ttf" << std::endl;
    }
    if (!biomeTexImg.loadFromFile("Assets/biomes.png")) {
        std::cout << "Couldn't load Assets/biomes.png" << std::endl;
    }
    if (!spriteTexImg.loadFromFile("Assets/sprites.png")) {
        std::cout << "Couldn't load Assets/sprites.png" << std::endl;
    }
    if (!entityTexImg.loadFromFile("Assets/entities.png")) {
        std::cout << "Couldn't load Assets/entities.png" << std::endl;
    }
    biomeTexture.loadFromImage(biomeTexImg);
    biomeTile.setTexture(biomeTexture);
    biomeTexture.setSmooth(false);
    spriteTexture.loadFromImage(spriteTexImg);
    spriteTile.setTexture(spriteTexture);
    spriteTexture.setSmooth(false);
    entityTexture.loadFromImage(entityTexImg);
    entityTile.setTexture(entityTexture);
    entityTexture.setSmooth(false);
  //Minimap
    mm_tex.create(MAP_W, MAP_H);
    minimap.scale(float(mm_size) / float(MAP_W), float(mm_size) / float(MAP_H));
    minimap.setOutlineThickness(1);
    minimap.setOutlineColor(sf::Color(0, 0, 0));
    minimap.setPosition(sf::Vector2f(0, mm_diag_width / 2));
    minimap.setRotation(-45);

  //Generate map
    genMap();


  //Start game-loop
    uint32_t game_time = 0;
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
        protag_rot = vecToAng((mouse_pos.x - float(WINDOW_W / 2)) / 2, mouse_pos.y - float(WINDOW_H / 2)) + 45;
        normaliseAng(protag_rot);
      //Check keyboard
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) { //Move protag forwards (NW)
            float dir_X, dir_Y;
            angToVec(protag_rot, dir_X, dir_Y);
            protag_X += dir_X; /// 10;
            protag_Y += dir_Y; /// 10;
        }

        doDISPLAY(game_time, window, biomeTile, spriteTile, entityTile, !(game_time % 100));

        sf::sleep(sf::milliseconds(10));
        ++game_time;
    }

    return 0; //End the program
}
