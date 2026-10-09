#ifndef PLAYER_H
#define PLAYER_H

#include "board.h"



class Player {
    public:

        Player(const int& c); // takes one argument: color of pieces assigned for that player

        void capture(const Piece* p);

        int color_pieces() const;

    private:
        int color_; // Color pieces the player has

        int material_score_; // count of the value of captured material

        std::vector<int> captured_; // vector storing pieces captured
        
};







#endif