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
    //cin >> move;
    while (move != "end") {
        if (move == "print") {
            cout << colors[bool(total_moves % 2)] << ": " << total_moves / 2 + 1 << endl;
            b.print_board();
            continue;
        }
        cin >> move; 

        while (move != "end" && move != "print" && !b.move(players[bool(total_moves % 2)], move)) { // try move
            cout << "Error handling move: " << move << endl << "try again..." << endl;

            cin >> move; // try move input again
        }
        
        cout << colors[bool(total_moves % 2)] << ": " << total_moves / 2 + 1 << endl;
        b.print_board();
        total_moves++;
        


        // if (move == "exf6") { // e4 d5 e5 f5 exf6
        //     if(Piece* p = b.get_square(Square("f5"))){
        //             cout << *p << endl;
        //             std::vector<Square> vision_squares = p->vision(b);
        //             for(size_t i = 0; i < vision_squares.size(); i++) {
        //                 cout << vision_squares[i] << ": " << vision_squares[i].translate() << " "; 
        //             }
            
        //         }
        //     else {
        //         cout << "f5 == nullptr" << endl;
        //     }
        // }


        // cout << "1" << endl;
        /*
        0: white: 1
        1: Black: 1
        2: White: 2
        3: Black: 2
        4: White: 3
        5
        6
        get_move -> test move -> print color and move num -> print board -> increment total moves
        */
       
    }

    

    // cout << "White: 2" << endl;
    // b.move(p1, Square("f1"), Square("c4"));  

    // b.print_board();

    // cout << "Black: 2" << endl;
    // b.move(p2, Square("g8"), Square("f6"));  

    // b.print_board();

    // cout << "White: 3" << endl;
    // b.move(p1, Square("d1"), Square("h5"));  

    // b.print_board();

    // cout << "Black: 3" << endl;
    // b.move(p2, Square("f6"), Square("h5"));  

    // b.print_board();

    // cout << "White: 4" << endl;
    // b.move(p1, Square("g2"), Square("g4"));  

    // b.print_board();

    // cout << "Black: 4" << endl;
    // b.move(p2, Square("a7"), Square("a5"));  

    // b.print_board();

    // cout << "White: 5" << endl;
    // b.move(p1, Square("g4"), Square("h5"));  

    // b.print_board();

    // cout << "Black: 5" << endl;
    // b.move(p2, Square("a8"), Square("a6"));  

    // b.print_board();

    // cout << "White: 6" << endl;
    // b.move(p1, Square("e1"), Square("e2"));  

    // b.print_board();

    // cout << "Black: 6" << endl;
    // b.move(p2, Square("d8"), Square("f6"));  

    // b.print_board();

    // cout << "White: 7" << endl;
    // b.move(p1, Square("e2"), Square("f3"));  

    // b.print_board();

    // cout << "Black: 7" << endl;
    // b.move(p2, Square("e5"), Square("e4"));  

    // b.print_board();

    // cout << "White: 8" << endl;
    // b.move(p1, Square("f2"), Square("f4"));  

    // b.print_board();

    // cout << "Black: 8" << endl;
    // Piece* p = b.get_square(Square("e4"));
    // vector<Square> vision_squares = p->vision(b);
    // for (int i = 0; i < vision_squares.size(); i++) {
    //     cout << vision_squares[i] << " ";
    // }
    // cout << endl;

    // b.move(p2, Square("e4"), Square("f3"));  

    // b.print_board();

    





    
    // if(Piece* p = b.get_square(Square("e8"))){
    //     cout << *p << endl;
    //     std::vector<Square> vision_squares = p->vision(b);
    //     for(size_t i = 0; i < vision_squares.size(); i++) {
    //     cout << vision_squares[i] << ": " << vision_squares[i].translate() << " ";
    // }

    // }
    // else {
    //     cout << "nullptr" << endl;
    // }

    // cout << "C++ version = " << __cplusplus << endl;

    // std::vector<Square> squares = (b.get_square(Square("e2")))->vision(b);
    // cout << "here" << endl;
    // for(size_t i = 0; i < squares.size(); i++) {
    //     Square from = squares[i];
    //     cout << from.rank << "][" << from.file << "]" << endl;
    // }
    // cout << "here2" << endl;
    // cout << boolalpha << b.get_square(Square("e2"))->first_move() << endl;
}