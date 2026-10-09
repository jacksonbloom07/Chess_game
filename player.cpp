#include "player.h"

Player::Player(const int& c) : color_(c), material_score_(0), captured_() {}

void Player::capture(const Piece* p) {
    material_score_ += p->value();
    captured_.push_back(p->id());

}

int Player::color_pieces() const { return color_; }