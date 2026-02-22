#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "positions.h"

int ConvertChar(char c) {
    switch (c) {
        case 'a':
            return 0;
            break;
        case 'b':
            return 1;
            break;
        case 'c':
            return 2;
            break;
        case 'd':
            return 3;
            break;
        case 'e':
            return 4;
            break;
        case 'f':
            return 5;
            break;
        case 'g':
            return 6;
            break;
        case 'h':
            return 7;
            break;
    }
};

void Initialize_position(Side * S, int br, int direction, bool which_side) {

    S->Which_side = which_side;
    S->direction = direction;
    S->backrank = br;
    S->HasAFrookMoved = false;
    S->HasHFrookMoved = false;
    S->HasKingMoved = false;

    for (int x = 0; x < 8; x++) {
        S->PawnFiles[x] = 0;
        for (int y = 0; y < 8; y++) {

            S->Attacks[x][y] = false;
            S->Pieces[x][y] = false;
            S->PieceTypes[x][y] = 'a';
            S->LVA[x][y] = 0.0f;
        }

    }

}

void AssignPieceType(Side* Cur_side, Side* Opp_side, int x, int y, char piece) {

    Cur_side->PieceTypes[x][y] = piece;
    Opp_side->PieceTypes[x][y] = piece;

    if (piece == 'K') {Cur_side->KingPos[0] = x;
    Cur_side->KingPos[1] = y;}

}

void SymetricalPieceAssign(Side* Cur_side, Side* Opp_side, int x, int y, char piece) {
    AssignPieceType(Cur_side, Opp_side, x, y, piece);
    AssignPieceType(Cur_side, Opp_side, 7 - x, y, piece);
}

void Initialize_classical_backrank(Side* Cur_side, Side* Opp_side) {
    int cbackrank = Cur_side->backrank;
    int dire = Cur_side->direction;

    for (int x = 0; x < 8; x++) {
        Cur_side->PawnFiles[x] = 1;
        Cur_side->Pieces[x][cbackrank] = true;
        Cur_side->Pieces[x][cbackrank + dire] = true;
        Cur_side->PieceTypes[x][cbackrank + dire] = 'p';
        Opp_side->PieceTypes[x][cbackrank + dire] = 'p';
    }

    //Oh well
    SymetricalPieceAssign(Cur_side, Opp_side, 0, cbackrank, 'R');
    SymetricalPieceAssign(Cur_side, Opp_side, 1, cbackrank, 'N');
    SymetricalPieceAssign(Cur_side, Opp_side, 2, cbackrank, 'B');
    AssignPieceType(Cur_side, Opp_side, 4, cbackrank, 'K');
    AssignPieceType(Cur_side, Opp_side, 3, cbackrank, 'Q');

}

void Initialize_classical_position(Side* W, Side* B) {

    int Wdirection = 1;
    int WBackrank = 0;
    int BBackrank = 7;

    Initialize_position(W, WBackrank, Wdirection, white);
    Initialize_position(B, BBackrank, Wdirection * -1, black);

    Initialize_classical_backrank(W, B);
    Initialize_classical_backrank(B, W);

}

void PlayMove(char* str, Side* Cur_side, Side* Opp_side) {
    move TheMove;

    if (!strcmp(str, "0-0-0")) {
        TheMove.piece = 'K'; TheMove.promotion = 'K';
        TheMove.x = 2; TheMove.y = Cur_side->backrank;
        TheMove.ox = 4; TheMove.oy = Cur_side->backrank;
        MakeAMove(TheMove, Cur_side, Opp_side); return;
    } else if (!strcmp(str, "0-0")) {
        TheMove.piece = 'K'; TheMove.promotion = 'K';
        TheMove.x = 6; TheMove.y = Cur_side->backrank;
        TheMove.ox = 4; TheMove.oy = Cur_side->backrank;
        MakeAMove(TheMove, Cur_side, Opp_side); return;
    };

    TheMove.piece = str[0];
    TheMove.ox = ConvertChar(str[3]); TheMove.x = ConvertChar(str[1]);
    TheMove.oy = ((int)(str[4]) - (int)'0')  - 1; TheMove.y = ((int)(str[2]) - (int)'0') - 1;
    TheMove.promotion = 'a';
    if (TheMove.y == Opp_side->backrank) {TheMove.promotion = str[5];}
    MakeAMove(TheMove, Cur_side, Opp_side); return;

}

int main() {

    char alphabet[8] = "abcdefgh";

    Side White;
    Side Black;

    Initialize_classical_position(&White, &Black);

    Board MainBoard = {White, Black};
    move BestMove;

    float eval = 0.0f;

    while (MainBoard.BSide.KingPos[0] != -1 && MainBoard.WSide.KingPos[0] != -1) {

        eval = Evaluate(MainBoard, &BestMove, white);
        printf("The evaluation is: %f\n", eval);
        printf("I'll play: %c-> %c%d (%c%d)\n", BestMove.piece, alphabet[BestMove.x], BestMove.y + 1, alphabet[BestMove.ox], BestMove.oy + 1);
        MakeAMove(BestMove, &MainBoard.WSide, &MainBoard.BSide);                                                                           

        ViewBoard(MainBoard);

        eval = Evaluate(MainBoard, &BestMove, black);
        printf("The evaluation is: %f\n", eval);
        printf("I'll play: %c-> %c%d (%c%d)\n", BestMove.piece, alphabet[BestMove.x], BestMove.y + 1, alphabet[BestMove.ox], BestMove.oy + 1);
        MakeAMove(BestMove, &MainBoard.BSide, &MainBoard.WSide);    


    }
}
