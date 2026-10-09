#ifndef BOARD_H
#define BOARD_H
#include "pieces.h"

enum File {A, B, C, D, E, F, G, H};

class InvalidMoveException : public std::runtime_error {
    public:
        explicit InvalidMoveException(const std::string& msg)
            : std::runtime_error(msg) {}
    };
    
struct MoveData {
    Square to_move_to;                  // the single destination square, e.g. e1
    std::vector<Square> candidates;     // every piece that could legally move there syntactically
}; 

struct Move {
    Square from;
    Square to;
};

class Player;

class Board {
    public: 
        // Default WHITE orientation
        Board();
        Board(const int& orientation);
        ~Board();

        // sets up the board at the start of the game (white's orientation default)
        void set_up_board();

        // Prints the board with spaces seperating peices
        void print_board() const;
       
        // Returns the peice on the square, if empty, returns nullptr
        Piece* get_square(const int& rank, const int& file) const;
        Piece* get_square(const Square& s) const;

        int orientation() const;
        
        void promote_pawn(const Square& promotion_square, const int& promotion_id);
        // Checks if the square is empty (nullptr)
        bool is_empty_square(const int& rank, const int& file) const;
        bool is_empty_square(const Square& s) const;
        
        // Move a piece from one square to another
        bool move(Player& plr, const Square& from, const Square& to);
        // void move(const int& from_rank, const int& from_file, const int& to_rank, const int& to_file);
        bool move(Player& plr, const std::string& action);

        // Flip the board orientation
        void flip_board();

        // True if Square s is out of bounds on the board
        bool OOB(const Square& s) const;
        bool OOB(const int& rank, const int& file) const;

        // True if s is being attacked by the pieces of the color
        bool square_is_attacked(const Square& s, const int& by_color) const;

        // if there is a pawn moved 2 squares on the last turn, that pawn is returned, 
        // else, nullptr is returned
        // - The position and color of that pawn are for Pawn::vision() for en passant oppurtunity
        Piece* en_passant() const;

        bool in_check(const Piece* king) const; // eventually change to in_check(const Player& plr) const or in_check(const int& plr_color) const


    private:
        Piece *board[8][8]; // 8 x 8 chess board

        int orientation_; // Orientation of the board from the players perspective
        
        Piece* last_moved_piece_; // What piece was moved, in the previous move
        Square last_moved_from_; // Where the piece moved from in the previous move

        std::vector< std::vector<Piece*> > pieces_; // Pieces[0] = Pieces[WHITE] = vector of white pieces
        std::vector<Piece*> kings_;
        void remove_piece_(const Piece* p); // Removes p from pieces_ faster by swapping to back and calling .pop_back()

        // True if there is a King opposite color to "k" adjacent to Square to "to_check"
        // bool adjacent_enemy_king_(const int& king_color, const Square& to_check) const;

        bool is_adjacent_(const Square& tile, const Square& to_check) const;

        /* 
            Helper function for move() to decouple reading logic from move logic:
            Returns a vector of size 2: (square to move piece from, square to move to)
            - does not bound check
            - does not check for incorrect piece movement
            these exceptions will be caught by move()
        */
        const MoveData read_move_notation(const Player& plr, const std::string& action) const;
        const Move returns_legal_move(const Player& plr, const MoveData& move) const;
        const bool is_legal_move(const Player& plr, const Square& from, const Square& to) const;

       
};

#endif