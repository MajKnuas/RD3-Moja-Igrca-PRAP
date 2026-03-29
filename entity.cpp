// V normalnem cpp filu specificiramo kaj funkcije naredijo
#include "entity.h"

Entity::Entity(){

}

Entity::Entity(float startX, float startY) {
    x = startX;
    y = startY;
    HP = 20;
}

Player::Player(float startX, float startY) {
    x = startX;
    y = startY;
    HP = 100;
}

Enemy::Enemy() {

}

Enemy::Enemy(float startX, float startY) {
    x = startX;
    y = startY;
    HP = 100;
}

void Player::moveUp(float dt) {
    y -= speed * dt;
    angle = 180;
}

void Player::moveDown(float dt) {
    y += speed * dt;
    angle = 0;
}

void Player::moveLeft(float dt) {
    x -= speed * dt;
    angle = 90;
}

void Player::moveRight(float dt){
    x += speed * dt;
    angle = 270;
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