#ifndef PIECES_H
#define PIECES_H

#include <vector>
#include <iostream>
#include "square.h"


enum COLORS {WHITE, BLACK};
enum PIECES {ROOK, KNIGHT, BISHOP, QUEEN, KING, PAWN};
// char *piece_symbols[2][6] = {{"♖", "♘", "♗", "♕", "♔", "♙"}, 
//                                    {"♜", "♞", "♝", "♛", "♚", "♟"}};
// const char *SQUARES[2] = {"□", "■"};
// const int SIZE = 8;


class Board;

class Piece {
    public:
       // friend class Board;

        // virtual char* print_piece();
        Piece() = default;
        Piece(const int val, const int color, const int id, const int file, const int rank);
        virtual ~Piece() = default;

        /* Vector of all squares piece can "see:"
            - All squares the piece can move to or that have allied peices it can protect
             - includes squares that contain enemy or ally pieces
                * Enemy pieces because moving onto an enemy square is legal and a capture 
                * Ally pieces because vision() is used to check if an enemy king can capture a 
                piece
        */

        virtual std::vector<Square> vision(const Board& b) const = 0;
        
        // returns true if has_moved_ == false
        bool first_move() const;

        // sets has_moved_ to true;
        void mark_as_moved();

        // Returns true if s is in the vision() vector
        bool in_vision(const Board& b, const Square& s) const;

        
        void move_to(const int& file, const int& rank);
        void move_to(const Square& s);

        int value() const;
        int color() const;
        int id() const;
        Square position() const;
        // friend void Board::set_up_board(int orientation=WHITE);
        // friend void Board::print_board() const;
        // friend void Board::promote_pawn(int &col);

        friend std::ostream& operator<<(std::ostream &os, const Piece &p);

        // void set_color(int color);


    private:
        int VALUE_;  // const in-game piece value (p: 1, N: 3, Q: 9, ...)
        int color_;  // (White or black)
        int PIECE_ID_;  // ID for checking piece type for method implementation (R: 0, N: 1, B: 2, Q: 3 ...)
        Square square_loc_;  // The square on the board the piece resides on the board[] matrix

        bool has_moved_;  // True if the piece has been moved
                          /* Cases in which this matters*
                             * Rooks and Kings (Castling rights)
                             * Pawns (First move)
                            
                            Unfortunately, I can't think of a better way to implement this:
                             - Making this a member for each of those pieces means that 
                             it can't be accessed generally by calling a Piece* on Board[][]
                          */

};

class Rook : public Piece {
    public:
        // char* print_piece();
        std::vector<Square> vision(const Board& b) const;

        // Rook();
        Rook(const int color, const int file, const int rank);
        

};

class Knight : public Piece {
    public:
        // char* print_piece();
        std::vector<Square> vision(const Board& b) const;
        // Knight();
        Knight(const int color, const int file, const int rank);

};

class Bishop : public Piece {
    public:
        // char* print_piece();
        std::vector<Square> vision(const Board& b) const;
        // Bishop();
        Bishop(const int color, const int file, const int rank);

};

class Queen : public Piece {
    public:
        // char* print_piece();
        std::vector<Square> vision(const Board& b) const;
        // Queen();
        Queen(const int color, const int file, const int rank);
};

class King : public Piece {
    public:
        // char* print_piece();
        std::vector<Square> vision(const Board& b) const;

        King(const int color, const int file, const int rank);

        bool in_check() const;
        bool checkmate() const; 
        // friend bool Board::danger_square(const King* k, const Square& s);

};

class Pawn : public Piece {
    public:
        // char* print_piece();
        std::vector<Square> vision(const Board& b) const;
        // Pawn() = default;
        Pawn(const int color, const int file, const int rank);

        int promotion() const; // returns an int of the column promotion occured on

       // bool first_move() const; // 

        bool en_passant() const;
};

#endif


