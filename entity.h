// V header filih so protoipi funkcij

class Entity {
    protected:
    float x, y, HP;

    public:
    Entity();
    Entity(float x, float y);
    float getX();
    float getY();
    float getHP();
};

class Player:public Entity{
    public:
    Player();
    Player(float x, float y);
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
};
