#include <iostream>
#include "pieces.h"
#include "board.h"
#include "player.h"
#include <algorithm>
#include <iterator>

Board::Board() : orientation_(WHITE), last_moved_piece_(nullptr), last_moved_from_(-1, -1) {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            board[i][j] = nullptr;
        }
    }
}

Board::Board(const int& orientation) : orientation_(orientation), last_moved_piece_(nullptr), 
                                        last_moved_from_(-1, -1) {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            board[i][j] = nullptr;
        }
    }
}

Board::~Board() {
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            delete board[i][j];
            board[i][j] = nullptr;
        }
    }
}

void Board::set_up_board() {
    pieces_.resize(2);
    Piece* black_pieces[8] = {new Rook(BLACK, A, 7), 
                      new Knight(BLACK, B, 7), 
                      new Bishop(BLACK, C, 7), 
                      new Queen(BLACK, D, 7), 
                      new King(BLACK, E, 7), 
                      new Bishop(BLACK, F, 7), 
                      new Knight(BLACK, G, 7), 
                      new Rook(BLACK, H, 7)};

    Piece* white_pieces[9] = {new Rook(WHITE, A, 0), 
                       new Knight(WHITE, B, 0), 
                       new Bishop(WHITE, C, 0), 
                       new Queen(WHITE, D, 0), 
                       new King(WHITE, E, 0), 
                       new Bishop(WHITE, F, 0), 
                       new Knight(WHITE, G, 0), 
                       new Rook(WHITE, H, 0)};

    kings_.push_back(white_pieces[E]);
    kings_.push_back(black_pieces[E]);

    for (int file = 0; file < 8; file++) {
        board[7][file] = black_pieces[file]; // Pieces facing away from user
        board[6][file] = new Pawn(BLACK, file, 6); // Pawns


        board[1][file] = new Pawn(WHITE, file, 1); // Pawns of other color
        board[0][file] = white_pieces[file]; // User's pieces


        pieces_[BLACK].push_back(board[7][file]);
        pieces_[BLACK].push_back(board[6][file]);
        pieces_[WHITE].push_back(board[1][file]);
        pieces_[WHITE].push_back(board[0][file]);
    }

    for (int rank = 0; rank < 4; rank++) {
        for (int file = 0; file < 8; file++) {
            board[rank + 2][file] = nullptr; // Empty squares on ranks 2-6
        }
    }
}

void Board::flip_board() {
    orientation_ = !orientation_;
}

bool Board::OOB(const Square& s) const {
    if (s.rank < 0 || s.rank > 7 || s.file < 0 || s.file > 7) { 
        return true;
    }
    return false;
}

bool Board::OOB(const int& rank, const int& file) const {
    if (rank < 0 || rank > 7 || file < 0 || file > 7) { 
        return true;
    }
    return false;
}

void Board::print_board() const {
    const char* SQUARES[2] = {"□", "■"};
    for (int i = 0; i < 8; ++i) {
        int rank = (orientation_ == WHITE) ? (7 - i) : i;
        // if orientation 
        for (int j = 0; j < 8; ++j) {
            int file = (orientation_ == WHITE) ? j : (7 - j);
            if (is_empty_square(rank, file)) {
                // Black if col and row are both even or if col and row are both odd
                // else white
                int color_tile = ((rank % 2 == 0 && file % 2 == 0) || 
                                  (rank % 2 != 0 && file % 2 != 0));
                // color_tile = 1 if tile statement is true (tile is black), else, = 0
                std::cout << SQUARES[color_tile] << " ";
            }
            else {
                std::cout << *board[rank][file] << " ";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    
}

bool Board::is_empty_square(const int& rank, const int& file) const {
    if (OOB(Square(rank, file))) { 
            std::cout << "Out of bounds empty square check (int file, int rank): [" << rank << "][" << file << "]"
                      << std::endl;
            return false;
    }
    return !(board[rank][file]);
}
bool Board::is_empty_square(const Square& s) const {
        if (OOB(s)) { 
            std::cout << "Out of bounds empty square check (Square s): [" << s.rank << "][" << s.file << "]"
                      << std::endl;
            return false;
        }
    return !(board[s.rank][s.file]);
}

Piece* Board::get_square(const int& rank, const int& file) const { return board[rank][file]; }

int Board::orientation() const { return orientation_; }

Piece* Board::get_square(const Square& s) const { return board[s.rank][s.file]; }

const bool Board::is_legal_move_(const Player& plr, const Square& from, const Square& to) {
    if (OOB(from) || OOB(to)) {
        // std::cout << "Error: Out of bounds move check (Square from, Square to): [" << from << "][" << to << "]"
        //           << std::endl;
        return false;
    }
    if (Piece* p = get_square(from)) {
        if (p->color() != plr.color_pieces()) {
            // std::cout << "Error: Attempting to move piece that is not your color: " << *p << std::endl;
            return false;
        }
        if (Piece* p2 = get_square(to); p2 && p2->color() == p->color()) {
            // std::cout << "Error: Square blocked by own piece: " << *p2 << std::endl;
            return false;
        }
        if (p->id() == KING) {
            if (square_is_attacked(to, !p->color())) {
                return false;
            }
        }
        if (!p->in_vision(*this, to)) { // might never trigger
            // std::cout << "Error: Attempting to move piece that cannot see destination square: " << *p << std::endl;
            return false;
        }
        /*
        Do move, check to see if it leaves player in check:
        */
        bool leaves_king_in_check = false;
        Piece* to_capture = board[to.rank][to.file];

        int direction[] = {1, -1};
        if (auto enemy_pawn = en_passant(); enemy_pawn && 
            p->id() == PAWN && 
            to.rank == enemy_pawn->position().rank + direction[p->color()]) {
            to_capture = enemy_pawn;
        }

        board[to.rank][to.file] = p;
        board[from.rank][from.file] = nullptr;
        p->move_to(to);

        if (en_passant() && to_capture && to_capture->position() != to) { // player took en passant
            board[(to_capture->position()).rank][(to_capture->position()).file] = nullptr; // Remove captured pawn
        }

        Square capture_position;
        if (to_capture) {
            capture_position = to_capture->position(); // saves position before moving off board
            to_capture->move_to(-1, -1); // temperarily moving piece "off the board" so that it has no "vision"
        }

        if (in_check(plr)) {
            leaves_king_in_check = true;
        }

        // returning everything back to where it was
        board[from.rank][from.file] = p;
        board[to.rank][to.file] = nullptr;
        p->move_to(from);
        if (to_capture) {
            board[capture_position.rank][capture_position.file] = to_capture; // put piece back
            to_capture->move_to(capture_position); // reinstate its position
        }
        if (leaves_king_in_check) {
            return false;
        }
        return true;
    }
    else {
        // std::cout << "Error: Attempting to move piece that does not exist on square: " << from.translate() << std::endl;
        return false;
    }
}

const Move Board::returns_legal_move_(const Player& plr, const MoveData& move) {
    if (move.canidates.size() == 0 && move.to_move_to == Square(-1, -1)) {
        return {Square(-1, -1), Square(-1, -1)};
    }
    if (move.canidates.size() < 1) {
        throw InvalidMoveException("Invalid move: No pieces of that type can move to " + move.to_move_to.translate());
    }
    else if (move.canidates.size() > 1) {
        throw InvalidMoveException("Invalid move: Too many pieces can move to " + move.to_move_to.translate());
    }
    Square from = move.canidates[0];
    Square to = move.to_move_to;
    if (is_legal_move_(plr, from, to)) {
        return {from, to};
    }
    else {
        if (is_empty_square(from)) {
            throw InvalidMoveException("Invalid move: there is no piece on " + from.translate());
        }
        else if (get_square(from)->color() != plr.color_pieces()) {
            throw InvalidMoveException("Invalid move: Piece on " + from.translate() + " belongs to the other player");
        }
        else {
            throw InvalidMoveException("Invalid move: Piece on " + from.translate() + " cannot move to " + to.translate());
        }
    }

}

const MoveData Board::read_move_notation_(const Player& plr, std::string& action) {
    MoveData empty_move = {Square(-1, -1), {}}; 
    if (action[action.length() - 1] == '+' || action[action.length() - 1] == '#') {
        action.pop_back(); // check or checkmate notation does not change the move and just makes it harder to parse

        // unfortunate side-effect is that any move is valid with + or # suffix regardless if that is actually valid
        // I wanted to support the notation, but I figured that validity checking this would just be more code and complicate the program
    }
    if (action == "O-O") {
       castle_(plr, false);
       return empty_move;
    }
    else if  (action == "O-O-O") {
        castle_(plr, true);
        return empty_move;
    }

    Square tile = Square();
    if (action.length() == 4 && action[2] == '=') { // a8=Q TODO: write promotion()
        tile = Square(action.substr(0, 2));

        if (action[3] != 'R' && action[3] != 'N' && action[3] != 'B' && action[3] != 'Q') {
            throw InvalidMoveException("Error: Bad promotion notation, must be R, N, B, or Q");
        }

        int direction[] = {1, -1};
        Square sqr_behind_promotion = Square(tile.rank - direction[plr.color_pieces()], tile.file);
        if (Piece* potential_pawn = get_square(sqr_behind_promotion)) { // checking if there is a piece behind tile
            if (potential_pawn->id() == PAWN &&  
            potential_pawn->color() == plr.color_pieces()) { // and if that piece is a pawn of the right color

                promote_pawn_(tile, action[3]);
                return empty_move;
            }
        }
        else {
            throw InvalidMoveException("Error: missing pawn before promotion square, no pawn on: " + tile.translate());
        }  
    }

    Square to_move_from(-1, -1);
    int id_to_move;
    switch (action[0]) {
        case 'R':
            id_to_move = ROOK;
            break;
        case 'N':
            id_to_move = KNIGHT;
            break;
        case 'B':
            id_to_move = BISHOP;
            break;
        case 'Q':
            id_to_move = QUEEN;
            break;
        case 'K':
            id_to_move = KING;
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':    
        case 'g':
        case 'h':
            id_to_move = PAWN;
            break;

        default:
            throw InvalidMoveException("Invalid notation: Bad read on piece ID");
    }
    if (action.length() == 2) { // d6
        tile = Square(action); // need to check if this tile has a friendly piece on it

        int direction[] = {1, -1};
        Square sqr_behind_tile = Square(tile.rank - (direction[plr.color_pieces()]), tile.file);
        Square two_sqrs_behind_tile = Square(tile.rank - 2 * (direction[plr.color_pieces()]), tile.file);

        std::vector<Square> canidates = {};
        if (!OOB(sqr_behind_tile)) {
            if (Piece* potential_pawn = get_square(sqr_behind_tile)) {
                if (potential_pawn->id() == PAWN && potential_pawn->color() == plr.color_pieces()) {
                    canidates.push_back(sqr_behind_tile);
                }
            }
            else if (tile.rank == 3 + plr.color_pieces()) { // tile is a square that can be double-jumped to
               if (Piece* potential_pawn = get_square(two_sqrs_behind_tile)) {
                    if (potential_pawn->id() == PAWN && potential_pawn->color() == plr.color_pieces()) {
                        canidates.push_back(two_sqrs_behind_tile);
                    }
                }
            }
        }
        return {tile, canidates};
    }

    if (action.length() == 3 || (action.length() == 4 && action[1] == 'x')) { // Qc2 || Nxh7 TODO add en_passant checking for capturing
        tile = (action.length() == 3) ? Square(action.substr(1, 2)) : Square(action.substr(2, 2));

        if (is_empty_square(tile) && !(en_passant()) && (action.length() == 4 && action[1] == 'x')) {
            throw InvalidMoveException("Bad notation, attempting to capture empty square");
        }
        if (id_to_move == PAWN && (action.length() == 3)) {
            throw InvalidMoveException("Bad notation; Maybe accidentally used lowercase letters for pieces?");
        }

        if (id_to_move == PAWN) {
            // Check for en passant
            if (Piece* en_passant_pawn = en_passant(); en_passant_pawn) {
                int direction[] = {1, -1};
                Square capturer_square = Square((action[3] - '0') - 1, action[0] - 'a'); // The square of the pawn that is capturing en passant
                if (tile == en_passant_pawn->position() && get_square(capturer_square) && get_square(capturer_square)->color() != en_passant_pawn->color()) { // If the target square matches the en passant square
                    return {Square(tile.rank - direction[en_passant_pawn->color()], tile.file), {capturer_square}}; // Return the square of the captured pawn
                }
            }
        
            // If not en passant, if the pawn is not capturing a piece, it is incorrect
            if (is_empty_square(tile)) {
                throw InvalidMoveException("Bad notation; attempting to capture empty square, or maybe accidentally used lowercase letters for pieces?");
            }
        }

        Piece* potential_piece = nullptr;
        std::vector<Square> canidates;
        for (int i = 0; i < pieces_[plr.color_pieces()].size(); i++) {
            if ((potential_piece = pieces_[plr.color_pieces()][i])->id() == id_to_move) {
                if (potential_piece->in_vision(*this, tile)) {
                    canidates.push_back(potential_piece->position());
                }
            }
        }
        return {tile, canidates};
    }

    else if (action.length() == 4 || (action.length() == 5 && action[2] == 'x')) { // R2e4 || Rfxd3
        
        tile = (action.length() == 4) ? Square(action.substr(2, 2)) : Square(action.substr(3, 2));
        if (action.length() == 5 && (is_empty_square(tile) || (get_square(tile)->color() == plr.color_pieces()))) {
            throw InvalidMoveException("Bad notation, attempting to capture empty square: " + tile.translate());
        }

        std::vector<Square> canidates;
        if (isdigit(action[1])) { // Rank specificiation
            int rank = action[1] - '1';
            for(int i = 0; i < 8; i++) {
                if (Piece* p = board[rank][i]) {
                    if (p->color() == plr.color_pieces() && p->in_vision(*this, tile)) {
                        canidates.push_back(p->position());
                    }
                }
            }
        }
        else { // File specificiation
            int file = action[1] - 'a';
            for(int i = 0; i < 8; i++) {
                if (Piece* p = board[i][file]) {
                    if (p->color() == plr.color_pieces() &&
                    p->in_vision(*this, tile)) {
                        canidates.push_back(p->position());
                    }
                }
            }
        }

        return {tile, canidates};
    }
    /*
    captures: [Piece]x[tile] len = 4
            Bxc7
            Qxe2
            - Pawn:
                dxc7
                exd6
            
            Specification captures: R1xc2, Rfxd3, Qdxf7  len = 5
            Double specification captures: Rc1xc2, Rf3xd3  len = 6
    */
    else if (action.length() == 5 || (action.length() == 6 && action[3] == 'x')) { // Re1e5 || Rc1xc2
        Square piece_loc = Square(action.substr(1, 2));
        tile = (action.length() == 5) ? Square(action.substr(3, 2)) : Square(action.substr(4, 2));


        if (action.length() == 6 && (is_empty_square(tile) || (get_square(tile)->color() == plr.color_pieces()))) {
            throw InvalidMoveException("Invalid notation: No piece to capture on " + tile.translate());
        }

        return {tile, {piece_loc}};
    }
    else {
        throw InvalidMoveException("Invalid notation: Bad read on action length");
    }
}

bool Board::move(Player& plr, std::string& action) { // TODO write castle(), long_castle(), and promotion()
    /*
    action shorthands:
        Basic: [Piece][tile] len = 3, 
            Qe5
            Ke2
            Ra5
        Pawn: [tile] len = 2
            e4
            d5
        Special: 
            Castle: O-O
            Long Castle: O-O-O
            Promotion: a8=Q
            // Check: Bb5+
            // Checkmate: Qh7#

        Explicit reference to file or rank when two pieces can move to a square:
            File: Rae1, Qed7
            Rank: R2e4, R4g5

            Double specification: Ra1a5
        
        captures: [Piece]x[tile] len = 4
            Bxc7
            Qxe2
            - Pawn:
                dxc7
                exd6
            
            Specification captures: R1xc2, Rfxd3, Qdxf7  len = 5
            Double specification captures: Rc1xc2, Rf3xd3  len = 6
                - Specification captures where both pieces that can capture are on different files AND ranks
                □ ■ □ ■ □ ♚ □ ■  - in this case, the Queen on b3 and d5 can both capture the pawn on d3
                ■ □ ■ □ ■ □ ■ □  - It would be equally acceptable to shorthand the b3 queen capturing on d3 as
                □ ■ □ ■ □ ■ □ ■    Qbxd3 or Q3xd3
                ■ □ ■*♕*■ □ ■ □  - Chess.com uses the former (Qbxd3) by default so I will use that as well,
                □ ■ □ ■ □ ■ □ ■    but allow either when it comes to move inputs
                ■*♕*■ ♟ ■ □ ■ □  - On top of this, any overspecification (Bf1c4 when Bc4 is applicable) will
                □ ■ □ ■ □ ■ □ ■    not be flagged as invalid and will be accepted
                ■ □ ■ □ ♔ □ ■ □ 
    */
    try {
        Move m = returns_legal_move_(plr, read_move_notation_(plr, action));
        
        if (m == Move{Square(-1, -1), Square(-1, -1)}) return true; // empty move

        return move(plr, m.from, m.to);
    }
    catch(InvalidMoveException& e) {
        std::cout << e.what() << std::endl;
        return false;
    }
    catch(IllegalMoveException& e) {
        std::cout << e.what() << std::endl;
        return false;
    }
    catch(std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        return false;
    }
}

bool Board::move(Player& plr, const Square& from, const Square& to) {
    Piece* p = get_square(from);
    if (!p) {
        throw std::runtime_error("Error: No piece on square " + from.translate());
    }
    

    Piece* to_capture = board[to.rank][to.file];

    int direction[] = {1, -1};
    if (auto enemy_pawn = en_passant(); enemy_pawn && 
        p->id() == PAWN && 
        to.rank == enemy_pawn->position().rank + direction[p->color()]) {
        to_capture = enemy_pawn;
    }

    board[to.rank][to.file] = p;
    board[from.rank][from.file] = nullptr;
    p->move_to(to);

    if (en_passant() && to_capture && to_capture->position() != to) { // player took en passant
        board[(to_capture->position()).rank][(to_capture->position()).file] = nullptr; // Remove captured pawn
    }

    Square capture_position;
    if (to_capture) {
        capture_position = to_capture->position(); // saves position before moving off board
        to_capture->move_to(-1, -1); // temperarily moving piece "off the board" so that it has no "vision"
    }

    if (in_check(plr)) { // if move leaves player still in check
        board[from.rank][from.file] = p;
        board[to.rank][to.file] = nullptr;
        p->move_to(from);
        if (to_capture) {
            board[capture_position.rank][capture_position.file] = to_capture; // put piece back
            to_capture->move_to(capture_position); // reinstate its position
        }
        throw IllegalMoveException("Illegal move: move leaves player in check");
    }

    if (to_capture) {
        remove_from_pieces_(to_capture);

        plr.capture(to_capture);
        delete to_capture;
        to_capture = nullptr;
    }
    

    // Book keeping

    // p->move_to(to);
    last_moved_piece_ = p;
    last_moved_from_ = from;
    if (p->first_move()) {
        p->mark_as_moved();
    }
    if (!get_square(to)) {
        throw std::runtime_error("\n2 Error: Piece not on square after move");
    }
    return true;
    
}

void Board::remove_from_pieces_(const Piece* p) {
    auto &p_vec = pieces_[p->color()]; // "Pieces_ vector" created for readability
    auto it = std::find(p_vec.begin(), p_vec.end(), p);
    int capture_index = std::distance(p_vec.begin(), it);
    
    std::swap(p_vec[capture_index], p_vec[p_vec.size() - 1]);
    p_vec.pop_back();
}

bool Board::square_is_attacked(const Square& s, const int& by_color) const {
    if (is_adjacent_(kings_[by_color]->position(), s)) {
        return true;
    }

    auto& enemy_pieces = pieces_[by_color];

    for(size_t i = 0; i < enemy_pieces.size(); i++) {
        if (enemy_pieces[i]->id() != KING && enemy_pieces[i]->in_vision(*this, s)) {
            if (enemy_pieces[i]->id() == PAWN) {
                int direction[] = {1, -1};
                Square left_pawn_capture_square = Square(enemy_pieces[i]->position().rank + direction[enemy_pieces[i]->color()], enemy_pieces[i]->position().file - 1);
                Square right_pawn_capture_square = Square(enemy_pieces[i]->position().rank + direction[enemy_pieces[i]->color()], enemy_pieces[i]->position().file + 1);
                if (left_pawn_capture_square == s || right_pawn_capture_square == s) {
                    return true;
                }
            }
            else {
                return true;
            }
        }
    }
    return false;
}

/*
    Returns true if tile is adjacent but not equal to to_check
*/
bool Board::is_adjacent_(const Square& tile, const Square& to_check) const {
    return (std::abs(tile.rank - to_check.rank) == 1 && std::abs(tile.file - to_check.file) == 1);
}

bool Board::in_check(const Player& plr) const {
    return (square_is_attacked(kings_[plr.color_pieces()]->position(), !plr.color_pieces()));
}


/*
    Returns last_moved_piece_ if it is a pawn last moved from home rank (2, 6) to rank (4, 5, respectively
    (moving 2 squares). 

    Board::move() uses it to check for valid pawn capture in the case that it is capturing an empty square

    ** Returning piece pointer so Pawn::vision() can check if it can take advantage of the potential 
    en_passant. Board::move() uses it as a bool **

*/
Piece* Board::en_passant() const {
    if (Piece* p = last_moved_piece_) {
        if (p->id() == PAWN && 
        p->position().rank == 3 + p->color() &&
        last_moved_from_.rank == 1 + 5 * p->color()) {
            return p;
        }
    }
    return nullptr;
}


void Board::promote_pawn_(const Square& promotion_square, const char promotion_letter) {
    int rank = promotion_square.rank;
    int file = promotion_square.file;
    Piece* pawn = nullptr;
    if (rank == 7) {
        pawn = get_square(6, promotion_square.file);
        last_moved_from_ = Square(1, promotion_square.file);
    }
    else if (rank == 0) {
        pawn = get_square(1, promotion_square.file);
        last_moved_from_ = Square(1, promotion_square.file);
    }
    else if (rank != 7 && rank != 0) {
        throw std::runtime_error("Error: Attempting to promote pawn on non-promotion rank");
    }
    else if (!pawn || pawn->id() != PAWN) {
        throw std::runtime_error("Error: Attempting to promote non-pawn piece");
    }
    Piece* p;
    switch (promotion_letter) {
        case 'R': 
            p = board[rank][file] = new Rook(pawn->color(), file, rank);
            break;
        case 'N': 
            p = board[rank][file] = new Knight(pawn->color(), file, rank);
            break;
        case 'B': 
            p = board[rank][file] = new Bishop(pawn->color(), file, rank); 
            break;
        case 'Q':
            p = board[rank][file] = new Queen(pawn->color(), file, rank);
            break;
    }
    board[pawn->position().rank][pawn->position().file] = nullptr;
    remove_from_pieces_(pawn);
    
    // Book keeping
    pieces_[pawn->color()].push_back(board[rank][file]);
    p->mark_as_moved();
    last_moved_piece_ = p;
}

void Board::castle_(const Player& plr, const bool& long_castle) {
    Piece* king = kings_[plr.color_pieces()];
    const int rank = king->position().rank; // 0 for white, 7 for black

    if (in_check(plr)) {
        throw std::runtime_error("Error: Attempting to castle while in check");
    }
    if (!king->first_move()) {
        throw std::runtime_error("Error: Attempting to castle with king that has already moved");
    }

    if (long_castle) {
        Piece* rook = get_square(rank, A);
        if (!rook || rook->id() != ROOK || !rook->first_move()) {
            throw std::runtime_error("Error: Attempting to long castle with rook that has already moved");
        }
        if (!is_empty_square(Square(rank, B)) || !is_empty_square(Square(rank, C)) || !is_empty_square(Square(rank, D))) {
            throw std::runtime_error("Error: Attempting to long castle with pieces in the way");
        }
        if (square_is_attacked(Square(rank, C), !plr.color_pieces()) || square_is_attacked(Square(rank, D), !plr.color_pieces())) {
            throw std::runtime_error("Error: Attempting to long castle through check");
        }
        board[rank][C] = king;
        board[rank][D] = rook;

        board[rank][E] = nullptr;
        board[rank][A] = nullptr;

        // Book keeping
        king->move_to(Square(rank, C));
        rook->move_to(Square(rank, D));
        king->mark_as_moved();
        rook->mark_as_moved();

        last_moved_piece_ = king;
        last_moved_from_ = Square(rank, E);
    }
    else {
        Piece* rook = get_square(rank, H);
        if (!rook || rook->id() != ROOK || !rook->first_move()) {
            throw std::runtime_error("Error: Attempting to castle with rook that has already moved");
        }
        if (!is_empty_square(Square(rank, F)) || !is_empty_square(Square(rank, G))) {
            throw std::runtime_error("Error: Attempting to castle with pieces in the way");
        }
        if (square_is_attacked(Square(rank, F), !plr.color_pieces()) || square_is_attacked(Square(rank, G), !plr.color_pieces())) {
            throw std::runtime_error("Error: Attempting to castle through check");
        }
        board[rank][G] = king;
        board[rank][F] = rook;

        board[rank][E] = nullptr;
        board[rank][H] = nullptr;

        // Book keeping
        king->move_to(Square(rank, G));
        rook->move_to(Square(rank, F));

        king->mark_as_moved();
        rook->mark_as_moved();

        last_moved_piece_ = king;
        last_moved_from_ = Square(rank, E);
    }
}

bool Board::checkmate(const Player& plr) {
    if (!in_check(plr)) {
        return false;
    }
    Piece* king = kings_[plr.color_pieces()];
    for (const Square& s : king->vision(*this)) { // can move out of check
        if (is_legal_move_(plr, king->position(), s)) {
            std::cout << "Debug: NOT Checkmate by available legal King move" << std::endl;
            return false;
        }
    }
    Piece* attacking_piece = nullptr;
    for (Piece* enemy_piece : pieces_[!plr.color_pieces()]) {
        if (enemy_piece->in_vision(*this, king->position())) {
            if (attacking_piece != nullptr) {
                std::cout << "Debug: Checkmate by double check" << std::endl;
                return true; // king has no legal moves && in double check
            }
            attacking_piece = enemy_piece;
        }
    }
    if (attacking_piece == nullptr) {
        throw std::logic_error("Error: In checkmate() -- King in check but no pieces found attacking king");
    }
    
    std::vector<Square> blocking_squares; // includes the square of the attacking piece for capture

    if (king->position().rank == attacking_piece->position().rank) {
        int k_file = king->position().file;
        int r_file = attacking_piece->position().file;
        for (size_t i = 0; i < std::abs(k_file - r_file); ++i) {
            int f = (r_file < k_file) ? (r_file + i) : (r_file - i);
            blocking_squares.push_back(Square(king->position().rank, f));
            
            /*
                3
                2   R * * K
                1
                  a b c d e
            */ 
        }
    }
    else if (king->position().file == attacking_piece->position().file) {
        int k_rank = king->position().rank;
        int r_rank = attacking_piece->position().rank;
        for (size_t i = 0; i < std::abs(k_rank - r_rank); ++i) {
            int r = (r_rank < k_rank) ? (r_rank + i) : (r_rank - i);
            blocking_squares.push_back(Square(r, king->position().file));
            /*
            6   R
            5   *
            4   *
            3   *
            2   K
            1
                c
            */
        }
    }
    else if (std::abs(king->position().file - attacking_piece->position().file) ==
             std::abs(king->position().rank - attacking_piece->position().rank)) {
        const Square& k_pos = king->position();
        const Square& p_pos = attacking_piece->position();
        for (size_t i = 0; i < std::abs(k_pos.rank - p_pos.rank); ++i) {
            int r = (p_pos.rank < k_pos.rank) ? (p_pos.rank + i) : (p_pos.rank - i);
            int f = (p_pos.file < k_pos.file) ? (p_pos.file + i) : (p_pos.file - i);
            blocking_squares.push_back(Square(r, f));
            /*
            6   B
            5      *
            4         *
            3            * 
            2               K
            1
                a  b  c  d  e
            */
        }
    }
    if (attacking_piece->id() != KNIGHT) {
        blocking_squares.push_back(attacking_piece->position());
    }
    else if (attacking_piece->id() != KNIGHT) {
        throw std::logic_error("Error: In checkmate() -- King in check but no pieces found on same rank, file or diagonal && piece is not knight");
    }
    for (const Piece* p : pieces_[plr.color_pieces()]) {
        for (const Square& s : blocking_squares) {
            if (p->id() != KING && is_legal_move_(plr, p->position(), s)) {
                std::cout << "Debug: NOT Checkmate by available legal move: piece on " << p->position().translate() << " -> " << s.translate() << std::endl;
                return false;
            }
        }
    }
    std::cout << "Debug: blocking_squares: ";
    for (const Square& s : blocking_squares) {
        std::cout << s.translate() << " ";
    }
    std::cout << std::endl;
    return true; // king has no legal moves && no pieces can capture the attacking piece && no piece can block
    
}

bool Board::stalemate(const Player& plr) {
    if (in_check(plr)) {
        return false;
    }
    for (const Piece* p : pieces_[plr.color_pieces()]) {
        for (const Square& s : p->vision(*this)) {
            if (is_legal_move_(plr, p->position(), s)) {
                return false;
            }
        }
    }
    return true;

}