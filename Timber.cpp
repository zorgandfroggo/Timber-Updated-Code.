//important libraries
#include <iostream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

//namespace
using namespace sf;

//branch stuff
void updateBranches(int seed);
const int NUM_BRANCHES = 6;
enum class side { LEFT, RIGHT, NONE };
side branchPositions[NUM_BRANCHES];

//start of game
int main()
{
    //creates window
    VideoMode vm{ {1920, 1080} };
    RenderWindow window(vm, "Timber", State::Windowed);

    //creates background
    Texture textureBackground;
    textureBackground.loadFromFile("graphics/background.png");
    Sprite spriteBackground(textureBackground);
    spriteBackground.setPosition(Vector2f(0,0));

    //creates tree
    Texture textureTree;
    textureTree.loadFromFile("graphics/tree.png");
    Sprite spriteTree(textureTree);
    spriteTree.setPosition(Vector2f(810, 0));

    //creates bee
    Texture textureBee;
    textureBee.loadFromFile("graphics/bee.png");
    Sprite spriteBee(textureBee);
    spriteBee.setPosition(Vector2f(0, 800));
    bool beeActive = false;
    float beeSpeed = 0.0f;

    //creates 3 clouds
    Texture textureCloud;
    textureCloud.loadFromFile("graphics/cloud.png");
    Sprite spriteCloud1(textureCloud);
    Sprite spriteCloud2(textureCloud);
    Sprite spriteCloud3(textureCloud);
    spriteCloud1.setPosition(Vector2f(0, 0));
    spriteCloud2.setPosition(Vector2f(0, 150));
    spriteCloud3.setPosition({ 0, 300 });
    bool cloud1Active = false;
    bool cloud2Active = false;
    bool cloud3Active = false;
    float cloud1Speed = 0.0f;
    float cloud2Speed = 0.0f;
    float cloud3Speed = 0.0f;

    //controls time
    Clock clock;

    //creates time bar
    RectangleShape timeBar;
    float timeBarStartWidth = 400;
    float timeBarHeight = 80;
    timeBar.setSize({ timeBarStartWidth, timeBarHeight });
    timeBar.setFillColor(Color::Red);
    timeBar.setPosition({ (1920 / 2) - timeBarStartWidth / 2, 980 });
    Time gameTimeTotal;
    float timeRemaining = 6.0f;
    float timeBarWidthPerSecond = timeBarStartWidth / timeRemaining;

    //pause game
    bool paused = true;

    //font and text
    Font font;
    font.openFromFile("fonts/KOMIKAP_.ttf");
    int score = 0;
    Text messageText(font, "Press Enter to start.", 75);
    Text scoreText(font, "Score = 0", 100);
    messageText.setFillColor(Color::White);
    scoreText.setFillColor(Color::White);
    FloatRect textRect = messageText.getLocalBounds();
    messageText.setOrigin({ textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f });
    messageText.setPosition({ 1920 / 2.0f, 1080 / 2.0f });
    scoreText.setPosition({ 20,20 });

    //branch textures
    Texture textureBranch;
    textureBranch.loadFromFile("graphics/branch.png");
    std::vector<sf::Sprite> branches(NUM_BRANCHES, Sprite(textureBranch));
    for (int i = 0; i < NUM_BRANCHES; ++i) {
        branches[i].setPosition(Vector2f(-2000, -2000));
        branches[i].setOrigin({ 220,20 });
    }

    //player textures
    Texture texturePlayer;
    texturePlayer.loadFromFile("graphics/player.png");
    Sprite spritePlayer(texturePlayer);
    spritePlayer.setPosition({ 580, 720 });

    side playerSide = side::LEFT;

    //death textures
    Texture textureRIP;
    textureRIP.loadFromFile("graphics/RIP.png");
    Sprite spriteRIP(textureRIP);
    spriteRIP.setPosition({ 600, 860 });

    //axe textures
    Texture textureAxe;
    textureAxe.loadFromFile("graphics/axe.png");
    Sprite spriteAxe(textureAxe);
    spriteAxe.setPosition({ 700, 830 });

    //axe position
    const float AXE_POSITION_LEFT = 700;
    const float AXE_POSITION_RIGHT = 1075;

    //flying log texture
    Texture textureLog;
    textureLog.loadFromFile("graphics/log.png");
    Sprite spriteLog(textureLog);
    spriteLog.setPosition({ 810, 720 });

    bool logActive = false;
    float logSpeedX = 1000;
    float logSpeedY = -1500;

    bool acceptInput = false;

    //chop sound
    SoundBuffer chopBuffer;
    chopBuffer.loadFromFile("sound/chop.wav");
    Sound chop(chopBuffer);

    //death sound
    SoundBuffer deathBuffer;
    deathBuffer.loadFromFile("sound/death.wav");
    Sound death(deathBuffer);

    //out of time sound
    SoundBuffer timeBuffer;
    timeBuffer.loadFromFile("sound/out_of_time.wav");
    Sound outOfTime(timeBuffer);

    //fps counter
    std::string fps;
    Text fpsText(font, fps, 50);
    fpsText.setPosition({ 1700, 20 });

    //game loop
    while (window.isOpen()) {

        while (const std::optional event = window.pollEvent()) {
            if (event->is<Event::KeyReleased>() && !paused) {
                acceptInput = true;
                spriteAxe.setPosition({ 2000, spriteAxe.getPosition().y });
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::Escape)) {
            window.close();
        }

        if (Keyboard::isKeyPressed(Keyboard::Key::Enter)) {
            paused = false;
            score = 0;
            timeRemaining = 6;

            for (int i = 1; i < NUM_BRANCHES; i++) {
                branchPositions[i] = side::NONE;
            }

            spriteRIP.setPosition({ 675, 2000 });
            spritePlayer.setPosition({ 580, 720 });

            acceptInput = true;
            chop.pause();
            outOfTime.pause();
            death.pause();
        }

        if (acceptInput) {

            if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                playerSide = side::RIGHT;
                score++;

                timeRemaining += (2 / score) + .15;

                spriteAxe.setPosition({ AXE_POSITION_RIGHT, spriteAxe.getPosition().y });
                spritePlayer.setPosition({ 1200, 720 });

                updateBranches(score);

                spriteLog.setPosition({ 810,720 });
                logSpeedX = -5000;
                logActive = true;

                acceptInput = false;
                chop.play();
            }

            if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                playerSide = side::LEFT;
                score++;

                timeRemaining += (2 / score) + .15;

                spriteAxe.setPosition({ AXE_POSITION_LEFT, spriteAxe.getPosition().y });
                spritePlayer.setPosition({ 580,720 });

                updateBranches(score);

                spriteLog.setPosition({ 810,720 });
                logSpeedX = 5000;
                logActive = true;

                acceptInput = false;
                chop.play();
            }
        }

        if (!paused) {
            //measures delta time (distance between two times)
            Time dt = clock.restart();

            std::stringstream ssfps;
            ssfps << 1 / dt.asSeconds();
            fps = ssfps.str();

            //time stuff
            timeRemaining -= dt.asSeconds();
            timeBar.setSize({ timeBarWidthPerSecond * timeRemaining, timeBarHeight });
            if (timeRemaining <= 0.00f) {
                paused = true;
                messageText.setString("Out of time!");
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin({ textRect.position.x + textRect.size.x / 2.0f, textRect.position.y + textRect.size.y / 2.0f });
                messageText.setPosition({ 1920 / 2.0f, 1080 / 2.0f });
                outOfTime.play();
            }

            //gives life to bee
            if (!beeActive) {
                srand((int)time(0));
                beeSpeed = rand() % 200 + 200;

                srand((int)time(0) * 10);
                float height = (rand() % 500) + 500;
                spriteBee.setPosition(Vector2f(2000, height));
                beeActive = true;
            }
            else {
                spriteBee.setPosition(Vector2f(spriteBee.getPosition().x - (beeSpeed * dt.asSeconds()), spriteBee.getPosition().y));
                if (spriteBee.getPosition().x < -100) {
                    beeActive = false;
                }
            }

            //blows 3 clouds
            if (!cloud1Active) {
                srand((int)time(0) * 10);
                cloud1Speed = rand() % 200;

                srand((int)time(0) * 10);
                float height = rand() % 150;
                spriteCloud1.setPosition(Vector2f(-300, height));
                cloud1Active = true;
            }
            else {
                spriteCloud1.setPosition(Vector2f(spriteCloud1.getPosition().x + (cloud1Speed * dt.asSeconds()), spriteCloud1.getPosition().y));
                if (spriteCloud1.getPosition().x > 1920) {
                    cloud1Active = false;
                }
            }
            if (!cloud2Active) {
                srand((int)time(0) * 20);
                cloud2Speed = rand() % 200;

                srand((int)time(0) * 20);
                float height = rand() % 300 - 150;
                spriteCloud2.setPosition(Vector2f(-300, height));
                cloud2Active = true;
            }
            else {
                spriteCloud2.setPosition(Vector2f(spriteCloud2.getPosition().x + (cloud2Speed * dt.asSeconds()), spriteCloud2.getPosition().y));
                if (spriteCloud2.getPosition().x > 1920) {
                    cloud2Active = false;
                }
            }
            if (!cloud3Active) {
                srand((int)time(0) * 30);
                cloud3Speed = rand() % 200;

                srand((int)time(0) * 30);
                float height = rand() % 450 - 150;
                spriteCloud3.setPosition(Vector2f(-300, height));
                cloud3Active = true;
            }
            else {
                spriteCloud3.setPosition(Vector2f(spriteCloud3.getPosition().x + (cloud3Speed * dt.asSeconds()), spriteCloud3.getPosition().y));
                if (spriteCloud3.getPosition().x > 1920) {
                    cloud3Active = false;
                }
            }

            //update score text
            std::stringstream ss;
            ss << "Score = " << score;
            scoreText.setString(ss.str());

            for (int i = 0; i < NUM_BRANCHES; i++) {
                float height = i * 150;

                if (branchPositions[i] == side::LEFT) {
                    branches[i].setPosition({ 660, height });
                    branches[i].setRotation(degrees(180));
                }
                else if (branchPositions[i] == side::RIGHT) {
                    branches[i].setPosition({ 1330, height });
                    branches[i].setRotation(degrees(0));
                }
                else {
                    branches[i].setPosition({ 3000, height });
                }
            }

            if (logActive) {
                spriteLog.setPosition({ spriteLog.getPosition().x + (logSpeedX * dt.asSeconds()),
                     spriteLog.getPosition().y + (logSpeedY * dt.asSeconds()) });

                if (spriteLog.getPosition().x < -100 || spriteLog.getPosition().x > 2000) {
                    logActive = false;
                    spriteLog.setPosition({ 810, 720 });
                }
            }

            if (branchPositions[5] == playerSide) {
                paused = true;
                acceptInput = false;

                spriteRIP.setPosition({ 525, 760 });
                spritePlayer.setPosition({ 2000, 600 });

                messageText.setString("SQUISHED!");
                FloatRect textRect = messageText.getLocalBounds();
                messageText.setOrigin({ textRect.position.x + textRect.size.x / 2, textRect.position.y + textRect.size.y / 2 });
                messageText.setPosition({ 1920 / 2, 1080 / 2 });

                death.play();
            }
        }

            //updates frame
            window.clear();
            window.draw(spriteBackground);
            window.draw(spriteCloud1);
            window.draw(spriteCloud2);
            window.draw(spriteCloud3);
            window.draw(spriteTree);
            for (int i = 0; i < NUM_BRANCHES; i++) {
                window.draw(branches[i]);
            }
            window.draw(spritePlayer);
            window.draw(spriteAxe);
            window.draw(spriteLog);
            window.draw(spriteRIP);
            window.draw(spriteBee);
            window.draw(scoreText);
            window.draw(timeBar);
            fpsText.setString(fps);
            window.draw(fpsText);
            if (paused) {
                window.draw(messageText);
            }
            window.display();
    }
    return 0;
}

void updateBranches(int seed) {
    for (int i = NUM_BRANCHES - 1; i > 0; i--) {
        branchPositions[i] = branchPositions[i - 1];
    }

    srand((int)time(0) + seed);
    int r = rand() % 5;

    switch (r) {
    case 0:
        branchPositions[0] = side::LEFT;
        break;
    case 1:
        branchPositions[0] = side::RIGHT;
        break;
    default:
        branchPositions[0] = side::NONE;
        break;
    }
}
