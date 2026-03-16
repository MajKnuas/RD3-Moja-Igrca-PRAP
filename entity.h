// V header filih so protoipi funkcij

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
};
