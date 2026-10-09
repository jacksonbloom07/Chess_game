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
    int inc_dec[] = {-1, 1}; // increments or decrements by WHITE or BLACK orientation
    for (int rank = 7; rank >= 0; rank += inc_dec[orientation_]) {
        for (int file = 0; file < 8; file++) {
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

const bool Board::is_legal_move_(const Player& plr, const Square& from, const Square& to) const {
    if (OOB(from) || OOB(to)) {
        std::cout << "Error: Out of bounds move check (Square from, Square to): [" << from << "][" << to << "]"
                  << std::endl;
        return false;
    }
    if (Piece* p = get_square(from)) {
        if (p->color() != plr.color_pieces()) {
            std::cout << "Error: Attempting to move piece that is not your color: " << *p << std::endl;
            return false;
        }
        if (p->id() == KING && in_check(p) && !p->in_vision(*this, to)) {
            std::cout << "Error: Attempting to move king that is in check to a square that does not block check: " << *p << std::endl;
            return false;
        }
        
        if (!p->in_vision(*this, to)) {
            std::cout << "Error: Attempting to move piece that cannot see destination square: " << *p << std::endl;
            return false;
        }
        return true;
    }
    else {
        std::cout << "Error: Attempting to move piece that does not exist on square: " << from.translate() << std::endl;
        return false;
    }
}

const Move Board::returns_legal_move_(const Player& plr, const MoveData& move) const {
    if (move.candidates.size() == 0 && move.to_move_to == Square(-1, -1)) {
        return {Square(-1, -1), Square(-1, -1)};
    }
    if (move.candidates.size() < 1) {
        throw InvalidMoveException("Invalid move: No pieces can move to " + move.to_move_to.translate());
    }
    else if (move.candidates.size() > 1) {
        throw InvalidMoveException("Invalid move: Too many pieces can move to " + move.to_move_to.translate());
    }
    Square from = move.candidates[0];
    Square to = move.to_move_to;
    if (is_legal_move_(plr, from, to)) {
        return {from, to};
    }
    else {
        throw InvalidMoveException("Invalid move: Piece on " + from.translate() + " cannot move to " + to.translate());
    }

}

const MoveData Board::read_move_notation_(const Player& plr, const std::string& action) {
    MoveData empty_move = {Square(-1, -1), {}};
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

        std::vector<Square> candidates = {};
        if (!OOB(sqr_behind_tile)) {
            if (Piece* potential_pawn = get_square(sqr_behind_tile)) {
                if (potential_pawn->id() == PAWN && potential_pawn->color() == plr.color_pieces()) {
                    candidates.push_back(sqr_behind_tile);
                }
            }
            else if (tile.rank == 3 + plr.color_pieces()) { // tile is a square that can be double-jumped to
               if (Piece* potential_pawn = get_square(two_sqrs_behind_tile)) {
                    if (potential_pawn->id() == PAWN && potential_pawn->color() == plr.color_pieces()) {
                        candidates.push_back(two_sqrs_behind_tile);
                    }
                }
            }
        }
        return {tile, candidates};
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
            // Pawn at d4 sees: d3 c3 
            // if en passant() -> en_passant pawn is on c4
            // if action = exc4, c4 is not in pawn's vision, BUT if en_passant() && tile == en_passant()->position + direction[] -> its valid
            // therefore the pawn should be able to move to en_passant()->position + direction[] := c3 
            // when interpreted, c3 is in vision of the pawn and should pass the legal move check

        
            // If not en passant, ensure the pawn is capturing a piece
            if (is_empty_square(tile)) {
                throw InvalidMoveException("Bad notation; attempting to capture empty square, or maybe accidentally used lowercase letters for pieces?");
            }
        }

        Piece* potential_piece = nullptr;
        std::vector<Square> candidates;
        for (int i = 0; i < pieces_[plr.color_pieces()].size(); i++) {
            if ((potential_piece = pieces_[plr.color_pieces()][i])->id() == id_to_move) {
                if (potential_piece->in_vision(*this, tile)) {
                    candidates.push_back(potential_piece->position());
                }
            }
        }
        // std::cout << "Debug: done, candidates.size() = " << candidates.size() << std::endl;
        // std::cout << "Debug: about to attempt to assign to_move..." << std::endl;
        return {tile, candidates};
        // std::cout << "Debug: position of to_move: " << to_move->position() << std::endl;

    }

    else if (action.length() == 4 || (action.length() == 5 && action[2] == 'x')) { // R2e4 || Rfxd3
        
        tile = (action.length() == 4) ? Square(action.substr(2, 2)) : Square(action.substr(3, 2));
        if (action.length() == 5 && (is_empty_square(tile) || (get_square(tile)->color() == plr.color_pieces()))) {
            throw InvalidMoveException("Bad notation, attempting to capture empty square: " + tile.translate());
        }

        std::vector<Square> candidates;
        if (isdigit(action[1])) { // Rank specificiation
            int rank = action[1] - '1';
            for(int i = 0; i < 8; i++) {
                if (Piece* p = board[rank][i]) {
                    if (p->color() == plr.color_pieces() && p->in_vision(*this, tile)) {
                        candidates.push_back(p->position());
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
                        candidates.push_back(p->position());
                    }
                }
            }
        }

        return {tile, candidates};
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
    else if (action.length() == 5 || (action.length() == 6 && action[3] == 'x')) { // Re1e5 || TODO: Rc1xc2
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

bool Board::move(Player& plr, const std::string& action) { // TODO write castle(), long_castle(), and promotion()
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
    catch(std::runtime_error& e) {
        std::cout << e.what() << std::endl;
        return false;
    }
}

bool Board::move(Player& plr, const Square& from, const Square& to) {
    bool before_plr_in_check = in_check(kings_[plr.color_pieces()]);
    
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

    if (en_passant() && to_capture && to_capture->position() != to) { // player took en passant
        board[(to_capture->position()).rank][(to_capture->position()).file] = nullptr; // Remove captured pawn
    }

    if (before_plr_in_check && in_check(kings_[plr.color_pieces()])) {
        if (en_passant() && to_capture == en_passant()) { 
            board[(to_capture->position()).rank][(to_capture->position()).file] = to_capture;
        }
        else {
            board[to.rank][to.file] = to_capture;
            // technically if to_capture == nullptr, it's nullptr anyway but I think this is more explicit and readable
        }
        
        board[from.rank][from.file] = p;
        std::cout << "Invalid move: Player currently in check" << std::endl;
        return false;
    }
    else if (to_capture) {
        remove_piece_(to_capture);

        plr.capture(to_capture);
        delete to_capture;
        to_capture = nullptr;
    }
    

    // Book keeping

    p->move_to(to);
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

void Board::remove_piece_(const Piece* p) {
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
                    // std::cout << "Debug: square_is_attacked: pawn on " << enemy_pieces[i]->position().translate() << " attacking " << s.translate() << std::endl;
                    return true;
                }
            }
            else {
                // std::cout << "Debug: square_is_attacked: piece on " << enemy_pieces[i]->position().translate() << " attacking " << s.translate() << std::endl;
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

bool Board::in_check(const Piece* king) const {
    return (square_is_attacked(king->position(), !king->color()));
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
            // std::cout << "Debug: creating queen at " << rank << ", " << file << std::endl;
            // std::cout << "Debug: rank, file = " << Square(rank, file).translate() << std::endl;
            // std::cout << "Debug: queen->position = " << (board[rank][file]->position()).translate() << std::endl;
            break;
    }
    board[pawn->position().rank][pawn->position().file] = nullptr;
    remove_piece_(pawn);
    
    // Book keeping
    pieces_[pawn->color()].push_back(board[rank][file]);
    p->mark_as_moved();
    last_moved_piece_ = p;
}

void Board::castle_(const Player& plr, const bool& long_castle) {
    Piece* king = kings_[plr.color_pieces()];
    if (in_check(king)) {
        throw std::runtime_error("Error: Attempting to castle while in check");
    }
    if (!king->first_move()) {
        throw std::runtime_error("Error: Attempting to castle with king that has already moved");
    }

    if (long_castle) {
        Piece* rook = get_square(king->position().rank, A);
        if (!rook || rook->id() != ROOK || !rook->first_move()) {
            throw std::runtime_error("Error: Attempting to long castle with rook that has already moved");
        }
        if (!is_empty_square(Square("b1")) || !is_empty_square(Square("c1")) || !is_empty_square(Square("d1"))) {
            throw std::runtime_error("Error: Attempting to long castle with pieces in the way");
        }
        if (square_is_attacked(Square("c1"), !plr.color_pieces()) || square_is_attacked(Square("d1"), !plr.color_pieces())) {
            throw std::runtime_error("Error: Attempting to long castle through check");
        }
        board[0][C] = king;
        board[0][D] = rook;

        board[0][E] = nullptr;
        board[0][A] = nullptr;

        // Book keeping
        king->move_to(Square("c1"));
        rook->move_to(Square("d1"));
        
        king->mark_as_moved();
        rook->mark_as_moved();

        last_moved_piece_ = king;
        last_moved_from_ = Square("e1");
    }
    else {
        Piece* rook = get_square(king->position().rank, H);
        if (!rook || rook->id() != ROOK || !rook->first_move()) {
            throw std::runtime_error("Error: Attempting to castle with rook that has already moved");
        }
        if (!is_empty_square(Square("f1")) || !is_empty_square(Square("g1"))) {
            throw std::runtime_error("Error: Attempting to castle with pieces in the way");
        }
        if (square_is_attacked(Square("f1"), !plr.color_pieces()) || square_is_attacked(Square("g1"), !plr.color_pieces())) {
            throw std::runtime_error("Error: Attempting to castle through check");
        }
        board[0][G] = king;
        board[0][F] = rook;

        board[0][E] = nullptr;
        board[0][H] = nullptr;

        // Book keeping
        king->move_to(Square("g1"));
        rook->move_to(Square("f1"));

        king->mark_as_moved();
        rook->mark_as_moved();

        last_moved_piece_ = king;
        last_moved_from_ = Square("e1");
    }
}