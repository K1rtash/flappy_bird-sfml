#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

struct Player{    
    bool jump = false;
    bool colision = false;
    float x = 560/2-108/2;
    float y = 0;
};

struct Pipe{
    float x = 560;
    int gapWidth = (rand() % (300 - 200 + 1) + 200);
    float gapStart = (rand() % ((852 - gapWidth) - 108 + 1) + 108);
    float speed = 100.0f;
    bool passed = false;
};

sf::Text getText(const sf::Font* font, const std::string& text, const sf::Color color, sf::Vector2<float> pos, sf::Vector2<float> scale, bool originCentered);
void handlePipe(sf::RenderWindow* window, float deltaTime, sf::Texture* pipeTex0, sf::Texture* pipeTex1, Player* player, Pipe* p, int* score, sf::Sprite* playerSprite, sf::Music* sound);

int main(){
    sf::RenderWindow window{sf::VideoMode({560, 960}), "Flappy Bird", sf::Style::Close | sf::Style::Titlebar, sf::State::Windowed};

    sf::Image icon;
    if (!icon.loadFromFile("assets/textures/bird.png")) return 1;
    window.setIcon(icon);

    float velocity = 0.0f;
    float gravity = 1000.0f;
    float jumpForce = -350.0f;
    float pipeSpawnTimer = 0.0f;
    bool gameOver = false;
    int score = 0;
    bool spaceKeyReleased = true;
    bool randomKeyPressed = false;
    int respawnTimer = 0;
    bool scoreBeaten = false;
    int highscore = 0;

    std::ifstream f("highscore.dat");
    f >> highscore;
    f.close();

    Player player;
    std::vector<Pipe*> pipes;

    sf::Texture playerTex;
    if (!playerTex.loadFromFile("assets/textures/bird.png")) return 1;
    sf::Sprite playerSprite{playerTex};

    sf::Texture pipeTex[2];
    if (!pipeTex[0].loadFromFile("assets/textures/pipe0.png")) return 1;
    if (!pipeTex[1].loadFromFile("assets/textures/pipe1.png")) return 1;

    const sf::Font arial_ttf("assets/fonts/arial.ttf");
    const sf::Font horrendo_ttf("assets/fonts/horrendo.ttf");
    sf::Music point_ogg{"assets/sounds/point.ogg"};
    sf::Music lose_ogg{"assets/sounds/lose.ogg"};

   sf::Clock clock{};
    srand(time(0));
    while (window.isOpen()) {
        float deltaTime = (clock.restart()).asSeconds();
        window.clear(sf::Color{112, 197, 206});

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) window.close();
            else if(const auto* k = event->getIf<sf::Event::KeyPressed>()) 
            {
                switch (k->scancode) { using key = sf::Keyboard::Scancode;

                    case key::Escape:
                        window.close();
                        break;
                    case key::Space:
                        if(spaceKeyReleased) {
                            player.jump = true;
                        }
                        spaceKeyReleased = false;
                        randomKeyPressed = true;
                        break;
                    default:
                        randomKeyPressed = true;
                }
            }
            else if(const auto* k = event->getIf<sf::Event::KeyReleased>())
            {
                switch (k->scancode) { using key = sf::Keyboard::Scancode;
                    case key::Space:
                        spaceKeyReleased = true;
                        break;
                }
            }
        }

        if(!gameOver) 
        {
            if (player.jump) {
                velocity = jumpForce;
                player.jump = false; 
            }

            velocity += gravity * deltaTime;
            player.y += velocity * deltaTime;

            if(player.y - 208 > 960) player.colision = true;

            if(player.y < 0) {
                player.y = 0 ;
                velocity = 0;
            }

            if(pipeSpawnTimer > 0) pipeSpawnTimer -= deltaTime; 
            float pipeSpeed = 100.0f + (float)score * 10.0f;
            if(pipeSpawnTimer <= 0) {
                pipeSpawnTimer = (float)(rand() % (400 - 300 + 1) + 300) / pipeSpeed;
                pipes.push_back(new Pipe);
            }

            playerSprite.setPosition({player.x,player.y});

            if(player.colision) {
                playerSprite.setColor(sf::Color::Red);
                gameOver = true;
                respawnTimer = 1000;
                lose_ogg.play();
                if (score > highscore) {
                    scoreBeaten = true;
                    highscore = score;
                };
            }
        } 
            for(auto& p : pipes) handlePipe(&window, deltaTime, &(pipeTex[0]), &(pipeTex[1]), &player, p, &score, &playerSprite, &point_ogg);
            if(!gameOver) window.draw(getText(&arial_ttf, ("Puntos: " + std::to_string(score)), sf::Color::White, {0, 0}, {1, 1}, false));

        if(gameOver) {
            playerSprite.move({0, 0.08});
            window.draw(getText(&horrendo_ttf, "GAME OVER", sf::Color::Red, {560 / 2, 400}, {1.5, 1.5}, true));
            window.draw(getText(&arial_ttf, ("Puntos: " + std::to_string(score)), sf::Color::Yellow, {560 / 2, 430}, {1, 1}, true));
            window.draw(getText(&arial_ttf, "Pulsa cualquier tecla para reiniciar", sf::Color::White, {560 / 2, 460}, {1, 1}, true));
            if(scoreBeaten) {
                window.draw(getText(&arial_ttf, "Nuevo record!", sf::Color::Green, {560 / 2, 490}, {1, 1}, true));
            } else {
                window.draw(getText(&arial_ttf, ("Tu mejor puntuacion es: " + std::to_string(highscore)), sf::Color::Green, {560 / 2, 490}, {1, 1}, true));
            }
            respawnTimer--;
            if(randomKeyPressed && respawnTimer < 0) {
                srand(time(0));
                for (auto& p : pipes) delete p;
                pipes.clear();
                player.colision = false;
                player.jump = false;
                player.x = 560/2-108/2;
                player.y = 10;
                playerSprite.setRotation(sf::degrees(0));
                playerSprite.setColor(sf::Color::White);
                scoreBeaten = false;
                velocity = 0.0f;
                pipeSpawnTimer = 0.0f;
                gameOver = false;
                score = 0;
                spaceKeyReleased = true;
            }
        }
        window.draw(playerSprite);
        window.display();
        randomKeyPressed = false;
    }

    for (auto& p : pipes) delete p;
    pipes.clear();

    std::ofstream fo("highscore.dat", std::ios::trunc);
    if(!fo) std::cerr << "error";
    fo << highscore;
    fo.close(); 
    return 0;
}

sf::Text getText(const sf::Font* font, const std::string& text, const sf::Color color, sf::Vector2<float> pos, sf::Vector2<float> scale, bool originCentered) {
    sf::Text t{*font};
    t.setString(text);
    t.setCharacterSize(24);
    t.setFillColor(color);
    t.setScale(scale);
    sf::FloatRect bounds = t.getLocalBounds();
    if(originCentered)
        t.setOrigin({bounds.position.x + bounds.size.x / 2.f,
                    bounds.position.y + bounds.size.y / 2.f});
    t.setPosition(pos);
    return t;
}

void handlePipe(sf::RenderWindow* window, float deltaTime, sf::Texture* pipeTex0, sf::Texture* pipeTex1, Player* player, Pipe* p, int* score, sf::Sprite* playerSprite, sf::Music* sound) {
    if(!player->colision) p->x -= (p->speed + (float)*score * 12.5f) * deltaTime;

    sf::Sprite sprite{*pipeTex0};
    sprite.setRotation(sf::degrees(180));
    sprite.setPosition({p->x+108, p->gapStart});
    window->draw(sprite);

    sprite.setTexture(*pipeTex1);
    float trunkY = p->gapStart;
    while (trunkY + 108 >= 0) {
        trunkY -= 108;
        sprite.setPosition({p->x+108, trunkY});
        window->draw(sprite);
    }

    sprite.setTexture(*pipeTex0);
    sprite.setRotation(sf::degrees(0));
    sprite.setPosition({p->x, p->gapStart + p->gapWidth});
    window->draw(sprite);

    sprite.setTexture(*pipeTex1);
    trunkY = p->gapStart + p->gapWidth + 108;
    while (trunkY < 960) {
        sprite.setPosition({p->x, trunkY});
        window->draw(sprite);
        trunkY += 108;
    }


    if(player->x + 95 > p->x && player->x - 13 < p->x + 108)
        if(player->y + 30 < p->gapStart || player->y + 70 > p->gapStart + p->gapWidth) player->colision = true;

    if(player->x > p->x + 108 && p->passed == false) {
        p->passed = true;
        (*score)++;
        sound->play();
    }
}