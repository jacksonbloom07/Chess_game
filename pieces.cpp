
#include <iostream>
// #include <cstdio>
#include "pieces.h"
#include "board.h"
#include "square.h"

Piece::Piece(const int val, const int color, const int id, const int file, const int rank) : 
    VALUE_(val), color_(color), PIECE_ID_(id), square_loc_(rank, file), has_moved_(false)
    {}

Rook::Rook(const int color, const int file, const int rank) : Piece(5, color, ROOK, file, rank)
    {}

// Rook::Rook() : Piece(5, NULL, ROOK, NULL, NULL)
//     {}

Knight::Knight(const int color, const int file, int rank) : Piece(3, color, KNIGHT, file, rank)
    {}

// Knight::Knight() : Piece(3, NULL, KNIGHT, NULL, NULL)
//     {}

Bishop::Bishop(const int color, const int file, int rank) : Piece(5, color, BISHOP, file, rank)
    {}

// Bishop::Bishop() : Piece(3, NULL, BISHOP, NULL, NULL)
//     {}

Queen::Queen(const int color, const int file, const int rank) : Piece(9, color, QUEEN, file, rank)
    {}

// Queen::Queen() : Piece(9, NULL, QUEEN, NULL, NULL)
//     {}

King::King(const int color, const int file, const int rank) : Piece(999, color, KING, file, rank)
    {}

// King::King() : Piece(999, NULL, KING, NULL, NULL)
//     {}

Pawn::Pawn(const int color, const int file, const int rank) : Piece(1, color, PAWN, file, rank)
    {}

// Pawn::Pawn() : Piece(1, NULL, PAWN, NULL, NULL)
//     {}

int Piece::value() const { return VALUE_; }
int Piece::color() const { return color_; }
int Piece::id() const { return PIECE_ID_; }
Square Piece::position() const { return square_loc_; }

bool Piece::first_move() const { return has_moved_ == false; }

void Piece::mark_as_moved() { has_moved_ = true;}

bool Piece::in_vision(const Board& b, const Square& s) const {
    
    std::vector<Square> sees = vision(b);
    for(size_t i = 0; i < sees.size(); i++) {
        if (sees[i] == s) {
            return true;
        }
    }
    return false;
}

void Piece::move_to(const int& file, const int& rank) { 
    square_loc_.file = file; square_loc_.rank = rank;
}
void Piece::move_to(const Square& s) {
    square_loc_ = s;
}




std::vector<Square> Rook::vision(const Board& b) const { // To Do
    std::vector<Square> sees;
    if (b.OOB(position())) {
        return sees;
    }

    int r[] = {-1, 1, 0, 0}; // rank increment and decrement
    int f[] = {0, 0, -1, 1}; // file increment and decrement
    for(int i = 0; i < 4; i++) {
        Square curr_tile = position(); // gets current position
        curr_tile.rank = curr_tile.rank + r[i];
        curr_tile.file = curr_tile.file + f[i];
        while(!b.OOB(curr_tile)) {
            
            if (!b.is_empty_square(curr_tile)) { // tile has a piece on it
                sees.push_back(curr_tile);
                break; // Break so vision doesn't look through piece

                // if (b.get_square(curr_tile)->color() != this->color()) {
                //     sees.push_back(curr_tile); // if piece is opponents, add to vision...
                // }
                // break; // else, don't. Break so vision doesn't look through piece
                // CHANGED EXPECTED BEHAVIOR (pieces can see their own color instead)
            }
            sees.push_back(curr_tile); // if empty tile, add to vision
            curr_tile.rank = curr_tile.rank + r[i];
            curr_tile.file = curr_tile.file + f[i];
        }
    }

    return sees;

}

std::vector<Square> Knight::vision(const Board& b) const {
    std::vector<Square> sees;
    if (b.OOB(position())) {
        return sees;
    }
    
    int r[] = {1, 2, 2, 1, -1, -2, -2, -1}; // rank increment and decrement
    int f[] = {-2, -1, 1, 2, 2, 1, -1, -2}; // file increment and decrement

    for(int i = 0; i < 8; i++) {
        if (Square s = Square(position().rank + r[i], position().file + f[i]); !b.OOB(s)) {
            sees.push_back(Square(s));
        }
       
    }

    return sees;

}

std::vector<Square> Bishop::vision(const Board& b) const {
    std::vector<Square> sees;
    
    if (b.OOB(position())) {
        return sees;
    }

    int r[] = {-1, 1, 1, -1}; // rank increment and decrement
    int f[] = {-1, -1, 1, 1}; // file increment and decrement
    for(int i = 0; i < 4; i++) {
        Square curr_tile = position(); // gets current position
        curr_tile.rank = curr_tile.rank + r[i];
        curr_tile.file = curr_tile.file + f[i];
        while(!b.OOB(curr_tile)) {

            // std::cout << "adding to [" << position().translate() << "] bishop vision: " << curr_tile.translate() << std::endl;

            if (!b.is_empty_square(curr_tile)) { // tile has a piece on it
                sees.push_back(curr_tile);
                break; // Break so vision doesn't look through piece
            }
            sees.push_back(curr_tile); // if empty tile, add to vision

            curr_tile.rank = curr_tile.rank + r[i];
            curr_tile.file = curr_tile.file + f[i];
        }
    }

    return sees;

}

std::vector<Square> Queen::vision(const Board& b) const {
    std::vector<Square> sees;
    int r[] = {-1, 1, 0, 0, -1, 1, 1, -1}; // rank increment and decrement
    int f[] = {0, 0, -1, 1, -1, -1, 1, 1}; // file increment and decrement

    for(int i = 0; i < 8; i++) {
        Square curr_tile = position(); // gets current position
        curr_tile.rank = curr_tile.rank + r[i];
        curr_tile.file = curr_tile.file + f[i];

        while(!b.OOB(curr_tile)) {
            if (!b.is_empty_square(curr_tile)) { // tile has a piece on it
                sees.push_back(curr_tile);
                break; // Break so vision doesn't look through piece
            }

            sees.push_back(curr_tile); // if empty tile, add to vision
            curr_tile.rank = curr_tile.rank + r[i];
            curr_tile.file = curr_tile.file + f[i];
        }
    }
    return sees;
}

std::vector<Square> King::vision(const Board& b) const {
    std::vector<Square> sees;
    int r[8] = {1, 1, 1, 0, -1, -1, -1, 0};
    int f[8] = {-1, 0, 1, 1, 1, 0, -1, -1};

    for(int i = 0; i < 8; i++) {
        Square adjacent_square = Square(position().rank + r[i], position().file + f[i]);
        
        if(!b.OOB(adjacent_square)) {
            sees.push_back(adjacent_square);
        }
    }
    // std::cout << "King at " << position().translate() << " sees: ";
    // for (const auto& square : sees) {
    //     std::cout << square.translate() << " ";
    // }
    // std::cout << std::endl;
    return sees;

}

std::vector<Square> Pawn::vision(const Board& b) const {
    std::vector<Square> sees;
    Square curr_square = position();

    if (b.OOB(curr_square)) {
        return sees;
    }

    int direction[] = {1, -1}; // direction (1 square forward or backward for white or black)

    // Forward movement
    Square sqr_ahead = Square(curr_square.rank + direction[color()], curr_square.file);
    Square two_sqrs_ahead = Square(curr_square.rank + 2 * direction[color()], curr_square.file);
    if (!b.OOB(sqr_ahead) && b.is_empty_square(sqr_ahead)) {
        sees.push_back(sqr_ahead);

        // Check for two-square move on first move
        if (first_move() && !b.OOB(two_sqrs_ahead) && b.is_empty_square(two_sqrs_ahead)) {
            sees.push_back(two_sqrs_ahead);
        }
    }

    // Diagonal captures
    for (int file_offset : {-1, 1}) {
        Square diag_square = Square(curr_square.rank + direction[color()], curr_square.file + file_offset);
        if (!b.OOB(diag_square) && !b.is_empty_square(diag_square)) {
            if (b.get_square(diag_square)->color() != color()) {
                sees.push_back(diag_square);
            }
        }
    }

    // En Passant
    if (const Piece* p = b.en_passant(); p &&
        p->position().rank == curr_square.rank &&
        std::abs(p->position().file - curr_square.file) == 1) { // Check if en passant pawn is adjacent
        Square en_passant_square = Square(curr_square.rank + direction[color()], p->position().file);
        sees.push_back(en_passant_square); // Add the square the pawn lands on
    }

    // // std::cout << "Pawn at " << curr_square.translate() << " sees: ";
    // for (const auto& square : sees) {
    //     std::cout << square.translate() << " ";
    // }
    // std::cout << std::endl;
    return sees;
}



std::ostream& operator<<(std::ostream &os, const Piece &p) {
    const char* piece_symbols[2][7] = {{"♖", "♘", "♗", "♕", "♔", "♙", "□"}, 
                                   {"♜", "♞", "♝", "♛", "♚", "♟", "■"}};
    // if (p == nullptr) { return os; }
    os << piece_symbols[p.color_][p.PIECE_ID_];
    return os;
}