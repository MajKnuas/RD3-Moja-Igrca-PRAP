// V header filih so protoipi funkcij

// Map constants
const float MAP_WIDTH = 2560;
const float MAP_HEIGHT = 1440;
const float PLAYER_SIZE = 50;
const float SAND_START_X = MAP_WIDTH - (MAP_WIDTH / 3.0f);

class Entity {
    protected:
    float x, y, HP; 
    public:
    Entity();
    Entity(float x, float y);
    float getX();
    float getY();
    float getHP();
    bool isInSand() const;
};

class Player:public Entity{
    float angle = 0;
    float speed = 200;
    float slowdownTimer = 0.0f;
    float slowdownMultiplier = 1.0f;
    public:
    Player();
    Player(float x, float y);
    void moveUp(float dt);
    void moveDown(float dt);
    void moveLeft(float dt);
    void moveRight(float dt);
    float getAngle();
    void clampToMap();
    void clampToSea();
    bool isOnSand() const;
    void takeDamage(float damageAmount);
    void applySlowdown();
    void updateSlowdown(float dt);
};

class Enemy:public Entity{
    float damageCooldownTimer;
    float movementSpeed;
    float angle;
    float directionTimer;
    int direction; // 0=up, 1=down, 2=left, 3=right
    float trashSpawnTimer;
    float trashSpawnInterval;
    float directionChangeInterval;
    public:
    // Static da si vsi enemyiji delijo lastnost DIRECTIONOV in ne rabi vsak objekt imeti svoje saj so enake
    static const int DIR_UP = 0;
    static const int DIR_DOWN = 1;
    static const int DIR_LEFT = 2;
    static const int DIR_RIGHT = 3;
    static constexpr float TRASH_SPAWN_INTERVAL = 5.0f;
    Enemy();
    Enemy(float x, float y);
    void takeDamage(float damageAmount, float dt);
    bool isAlive() const;
    void updateCooldown(float dt);
    void updateAI(float dt);
    bool updateTrashSpawn(float dt);
    void chooseRandomDirection(float dt);
    bool isInSand() const;
    float getAngle() const;
    void setMovementSpeed(float speed);
    void setTrashSpawnInterval(float interval);
    void setDirectionChangeInterval(float interval);
};
