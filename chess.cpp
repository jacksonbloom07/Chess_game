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
    
    */
    while (move != "end") {
        cin >> move; 

        while (move != "end" && move != "print" && !b.move(players[bool(total_moves % 2)], move)) { // try move
            cout << "Error handling move: " << move << endl << "try again..." << endl;

            cin >> move; // try move input again
        }
        if (move == "print") {
            cout << colors[bool(total_moves % 2)] << ": " << total_moves / 2 + 1 << endl;
            b.print_board();
            continue;
        }
        else {
            cout << colors[bool(total_moves % 2)] << ": " << total_moves / 2 + 1 << endl;
            b.print_board();
            total_moves++;
        }  
    }
}