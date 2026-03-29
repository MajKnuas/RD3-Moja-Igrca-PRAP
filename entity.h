// V header filih so protoipi funkcij

// Map constants
const float MAP_WIDTH = 2560;
const float MAP_HEIGHT = 1440;
const float PLAYER_SIZE = 50;

class Entity {
    protected:
    float x, y, HP;
    float speed = 200; 

    public:
    Entity();
    Entity(float x, float y);
    float getX();
    float getY();
    float getHP();
};

class Player:public Entity{
    float angle = 0;
    public:
    Player();
    Player(float x, float y);
    void moveUp(float dt);
    void moveDown(float dt);
    void moveLeft(float dt);
    void moveRight(float dt);
    float getAngle();
    void clampToMap();
};

class Enemy:public Entity{
    public:
    Enemy();
    Enemy(float x, float y);
};
