#include <iostream>
#include "board.h"
#include "player.h"

using namespace std;
int main() {
    Board b;
    Player p1(WHITE);
    Player p2(BLACK);
    b.set_up_board();

    Player players[] = {p1, p2};
    string colors[] = {"White", "Black"};

    string move = "";
    int total_moves = 0; // white moves + blacks moves
    cout << colors[bool(total_moves % 2)] << ": " << total_moves / 2 + 1 << endl;
    b.print_board();
    /*
    Test for promotion -- Queen: e4 e5 d4 exd4 c4 dxc4 a3 cxb2 Nc3 b1=Q a4 Qc2
    Test for promotion -- Knight: e4 e5 d4 exd4 c4 dxc4 a3 cxb2 Nc3 b1=N Ra2 Nxa3
    Test for promotion -- Bishop: e4 e5 d4 exd4 c4 dxc4 a3 cxb2 Nc3 b1=B a4 Bxe4
    Test for promotion -- Rook: e4 e5 d4 exd4 c4 dxc4 a3 cxb2 Nc3 b1=R a4 Rb6
    Test to castle -- Kingside: e4 e5 Nf3 Nc6 Bc4 Nf6 O-O
    Test to castle -- Queenside: e4 d5 exd5 Qxd5 Nc3 Qa5 Bd2 c6 Nf3 Bg4 Be2 Nf6 h3 Bxf3 Bxf3 e6 Qe2 Bb4 O-O-O
    Tests for incorrect castling:
        Kingside -- Castling with no rook on h1: e4 e5 Nf3 Nc6 Bc4 Nf6 Nc3 Bc5 h3 h5 d3 Rh6 Be3 Rh8 Bxc5 O-O
                 -- Castling with no King on e1:
                 -- Castling after moving king: e4 e5 Bc4 Nf6 d3 Bb4 Kf1 O-O Nf3 Nc6 Bd2 d6 Ke1 O-O
                 -- Castling after moving rook: e4 e5 Nf3 Nc6 Bc4 Nf6 Rg1 Bc5 Rh1 O-O O-O
                 -- Castling through check: e4 e5 Nf3 Nc6 Bc4 Nf6 Nc3 Bc5 h3 h5 d3 d5 Be3 Bd7 Bxc5 O-O

    Test for check recognition: e4 e5 d4 Bb4
    Tests for trying to escape check:
        - Blocking: e4 e5 d4 Bb4 c3
        - Capturing: e4 e5 d4 exd4 Qxd4 Bb4 Qxb4
        - King moving out of check: e4 e5 Bc4 Nf6 d3 Bb4 Kf1
    Tests for moving into check:
        - King moving into check: e4 e5 Bc4 Nf6 d3 Bb4 Kf1 O-O Ke1
        - Moving a pinned piece
        - Capturing with a pinnned piece
        - Taking en passant while the passant-ing pawn is pinned
        - King moving out of check and into another one
        - Attempting to castle out of check

    Checkmate tests:
        - Fools mate: e4 f5 d4 g5 Qh5
        - Scholars Mate: e4 e5 Bc4 Nc6 Qf3 Bc5 Qxf7
        - Backrank mate: 
    Stalemate test:
        - Fastest possible stalemate: e3 a5 Qh5 Ra6 Qxa5 h5 h4 Rah6 Qxc7 f6 Qxd7 Kf7 Qxb7 Qd3 Qxb8 Qh7 Qxc8 Kg6 Qe6
    Ambiguities:
        - Issue with single ambiguity for same rank rooks (Rcd1 still says mutliple candidates)
    */
    while (move != "end") {
        cin >> move; 
        Player& player_to_move = players[bool(total_moves % 2)];
        Player& opposing_player = players[!bool(total_moves % 2)];
        while (move != "end" && move != "print" && move != "flip" && !b.move(player_to_move, move)) { // try move
            cout << "Error handling move: " << move << endl << "try again..." << endl;

            cin >> move; // try move input again
        }
        if (move == "flip") {
            b.flip_board();
        }
        cout << colors[bool(total_moves % 2)] << ": " << total_moves / 2 + 1 << endl;
        b.print_board();

        if (move != "print") {
            if (b.in_check(opposing_player)) {
                if (b.checkmate(opposing_player)) {
                    cout << "Checkmate!" << endl;
                    return 1;
                }
                cout << "Check!" << endl;
            }
            else if(b.stalemate(opposing_player)) {
                cout << "Stalemate" << endl;
                return 1;
            }
            total_moves++;
        }  
    }
}