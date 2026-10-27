#ifndef BOARD_H
#define BOARD_H
#include "pieces.h"

enum File {A, B, C, D, E, F, G, H};

class InvalidMoveException : public std::runtime_error {
    public:
        explicit InvalidMoveException(const std::string& msg)
            : std::runtime_error(msg) {}
    };

class IllegalMoveException : public std::runtime_error {
    public:
        explicit IllegalMoveException(const std::string& msg)
            : std::runtime_error(msg) {}
    };
    
struct MoveData {
    Square to_move_to;                  // the single destination square, e.g. e1
    std::vector<Square> candidates;     // every piece that could legally move there
}; 

struct Move {
    Square from;
    Square to;
    bool operator==(const Move& other) const {
        return (from == other.from && to == other.to);
    }
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
        
        // Checks if the square is empty (nullptr)
        bool is_empty_square(const int& rank, const int& file) const;
        bool is_empty_square(const Square& s) const;
        
        // Move a piece from one square to another
        bool move(Player& plr, const Square& from, const Square& to);
        // void move(const int& from_rank, const int& from_file, const int& to_rank, const int& to_file);
        bool move(Player& plr, std::string& action);

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

        bool in_check(const Player& plr) const;

        bool checkmate(const Player& plr);
        bool stalemate(const Player& plr);


    private:
        Piece *board[8][8]; // 8 x 8 chess board

        int orientation_; // Orientation of the board from the players perspective
        
        Piece* last_moved_piece_; // What piece was moved, in the previous move
        Square last_moved_from_; // Where the piece moved from in the previous move

        std::vector< std::vector<Piece*> > pieces_; // Pieces[0] = Pieces[WHITE] = vector of white pieces
        std::vector<Piece*> kings_;
        void remove_from_pieces_(const Piece* p); // Removes p from pieces_ faster by swapping to back and calling .pop_back()

        // True if there is a King opposite color to "k" adjacent to Square to "to_check"
        // bool adjacent_enemy_king_(const int& king_color, const Square& to_check) const;

        bool is_adjacent_(const Square& tile, const Square& to_check) const;

        /* 
            Helper function for move() to decouple reading logic from move logic:
            Returns a MoveData object: (A Square for the intended square to move to,
                                        a vector of possible candidates for pieces to move to that square)

            - does not bound check
            - does not check for incorrect piece movement
            * these exceptions will be caught by returns_legal_move_ and is_legal_move_ 
              (A bit redundant to have two functions doing that but it will be optimized in the future)

            
            Ex: "Ne4" returns: (e4, [position of any knight that can move to e4])
                - this gets passed to returns_legal_move_ which returns the legal knight move if it is unambiguous (only one knight can move there)
        */
        const MoveData read_move_notation_(const Player& plr, std::string& action);

        /*
            A helper function for move(): given a vector of possible moves, it returns a single Move
            Parameters: 
            - player making the move
            - a MoveData object containing a list of candidates that can move to a certain square

            This function decerns which of the canidate pieces is the correct one to move to the specified tile
        */
        const Move returns_legal_move_(const Player& plr, const MoveData& move);

        /*
            A helper function for move():
            Parameters:
            - Player making the move
            - Square of the piece moving
            - Square for piece to move to

            Returns true given that the move is legal, else returns false
        
        */
        const bool is_legal_move_(const Player& plr, const Square& from, const Square& to);
        
        void promote_pawn_(const Square& promotion_square, const char promotion_letter);
        void castle_(const Player& plr, const bool& long_castle=false);
};

#endif