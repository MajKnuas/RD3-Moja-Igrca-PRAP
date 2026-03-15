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

void Player::moveUp() { y -= 20; }

void Player::moveDown() { y += 20; }

void Player::moveLeft() { x -= 20; }

void Player::moveRight() { x += 20; }

float Entity::getX()  { return x; } 

float Entity::getY() { return y; }

float Entity::getHP() { return HP;}