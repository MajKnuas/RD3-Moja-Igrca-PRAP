// V normalnem cpp filu specificiramo kaj funkcije naredijo
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include "entity.h"

static float randomFloat(float minValue, float maxValue) {
    return minValue + static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * (maxValue - minValue);
}

Entity::Entity(){}

Entity::Entity(float startX, float startY){}

Player::Player(float startX, float startY) {
    x = startX;
    y = startY;
    HP = 100;
}

Enemy::Enemy() {
    directionTimer = 0.0f;
    damageCooldownTimer = 0.0f;
    trashSpawnInterval = TRASH_SPAWN_INTERVAL;
    trashSpawnTimer = trashSpawnInterval;
    directionChangeInterval = 1.0f;
    movementSpeed = 140.0f;
    chooseRandomDirection(0.0f);
}

Enemy::Enemy(float startX, float startY) {
    x = startX;
    y = startY;
    HP = 100;
    directionTimer = 0.0f;
    damageCooldownTimer = 0.0f;
    trashSpawnInterval = TRASH_SPAWN_INTERVAL;
    trashSpawnTimer = trashSpawnInterval;
    directionChangeInterval = 1.0f;
    movementSpeed = 140.0f;
    chooseRandomDirection(0.0f);
}

void Enemy::updateCooldown(float dt) {
    if (damageCooldownTimer > 0.0f) {
        damageCooldownTimer -= dt;
    }
}

void Enemy::takeDamage(float damageAmount, float dt) {
    if (damageCooldownTimer <= 0.0f) {
        HP -= damageAmount;
        damageCooldownTimer = 1.0f;
    } else {
        damageCooldownTimer -= dt;
    }
}

bool Enemy::isAlive() const {
    return HP > 0;
}

bool Enemy::isInSand() const {
    return x + PLAYER_SIZE > SAND_START_X;
}

void Enemy::chooseRandomDirection(float dt) {
    int start = rand() % 4;
    for (int i = 0; i < 4; i++) {
        int testDir = (start + i) % 4;
        float nextX = x;
        float nextY = y;

        if (testDir == DIR_UP) nextY -= movementSpeed * dt;
        if (testDir == DIR_DOWN) nextY += movementSpeed * dt;
        if (testDir == DIR_LEFT) nextX -= movementSpeed * dt;
        if (testDir == DIR_RIGHT) nextX += movementSpeed * dt;

        if (nextX < 0.0f || nextY < 0.0f) continue;
        if (nextX + PLAYER_SIZE > SAND_START_X) continue;
        if (nextY + PLAYER_SIZE > MAP_HEIGHT) continue;

        direction = testDir;
        break;
    }

    directionTimer = directionChangeInterval;
    if (direction == DIR_UP) angle = 180.0f;
    else if (direction == DIR_DOWN) angle = 0.0f;
    else if (direction == DIR_LEFT) angle = 90.0f;
    else angle = 270.0f;
}

void Enemy::updateAI(float dt) {
    if (!isAlive()) {
        return;
    }

    if (isInSand()) {
        x = std::min(x, SAND_START_X - PLAYER_SIZE);
        chooseRandomDirection(dt);
    }

    directionTimer -= dt;
    if (directionTimer <= 0.0f) {
        chooseRandomDirection(dt);
    }

    float moveX = 0.0f;
    float moveY = 0.0f;

    if (direction == DIR_UP) moveY = -movementSpeed * dt;
    else if (direction == DIR_DOWN) moveY = movementSpeed * dt;
    else if (direction == DIR_LEFT) moveX = -movementSpeed * dt;
    else moveX = movementSpeed * dt;

    float nextX = x + moveX;
    float nextY = y + moveY;

    if (nextX < 0.0f || nextX + PLAYER_SIZE > SAND_START_X || nextY < 0.0f || nextY + PLAYER_SIZE > MAP_HEIGHT) {
        chooseRandomDirection(dt);
        return;
    }

    x = nextX;
    y = nextY;
}

bool Enemy::updateTrashSpawn(float dt) {
    if (!isAlive()) {
        return false;
    }

    trashSpawnTimer -= dt;
    if (trashSpawnTimer <= 0.0f) {
        trashSpawnTimer += trashSpawnInterval;
        return true;
    }
    return false;
}

float Enemy::getAngle() const {
    return angle;
}

void Enemy::setMovementSpeed(float speed) {
    movementSpeed = speed;
}

void Enemy::setTrashSpawnInterval(float interval) {
    trashSpawnInterval = interval;
    trashSpawnTimer = trashSpawnInterval;
}

void Enemy::setDirectionChangeInterval(float interval) {
    directionChangeInterval = interval;
    directionTimer = directionChangeInterval;
}

void Player::moveUp(float dt) {
    speed = isOnSand() ? 100 : 200;
    y -= speed * slowdownMultiplier * dt;
    angle = 180;
}

void Player::moveDown(float dt) {
    speed = isOnSand() ? 100 : 200;
    y += speed * slowdownMultiplier * dt;
    angle = 0;
}

void Player::moveLeft(float dt) {
    speed = isOnSand() ? 100 : 200;
    x -= speed * slowdownMultiplier * dt;
    angle = 90;
}

void Player::moveRight(float dt){
    speed = isOnSand() ? 100 : 200;
    x += speed * slowdownMultiplier * dt;
    angle = 270;
}

void Player::takeDamage(float damageAmount) {
    HP -= damageAmount;
    if (HP < 0.0f) {
        HP = 0.0f;
    }
}

void Player::applySlowdown() {
    slowdownMultiplier = 0.60f;  // 40% slower
    slowdownTimer = 2.0f;        // Lasts 2 second
}

void Player::updateSlowdown(float dt) {
    if (slowdownTimer > 0.0f) {
        slowdownTimer -= dt;
        if (slowdownTimer <= 0.0f) {
            slowdownMultiplier = 1.0f;
        }
    }
}


float Entity::getX()  { return x; } 

float Entity::getY() { return y; }

float Entity::getHP() { return HP;}

float Player::getAngle() {return angle;}

// Metoda ki naredi da ne mores ven iz mape
void Player::clampToMap() {
    if (x < 0) {
        x = 0;
    }
    if (x + PLAYER_SIZE > MAP_WIDTH) {
        x = MAP_WIDTH - PLAYER_SIZE;
    }
    
    if (y < 0) {
        y = 0;
    }
    if (y + PLAYER_SIZE > MAP_HEIGHT) {
        y = MAP_HEIGHT - PLAYER_SIZE;
    }
}

void Player::clampToSea() {
    if (x + PLAYER_SIZE > SAND_START_X) {
        x = SAND_START_X - PLAYER_SIZE;
    }
}

bool Player::isOnSand() const {
    return x + PLAYER_SIZE > SAND_START_X;
}
