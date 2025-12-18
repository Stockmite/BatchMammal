#include <stdio.h>
#include <stdbool.h>

#define black false
#define white true

void GetGeneralBoard(int * BlackSide[], int * WhiteSide[], int * GeneralBoard[]) {

    for (int x = 0; x < 8; x++)  {
        for (int y = 0; y < 8; y++) {
            int IsThereAPiece = BlackSide[x][y] & WhiteSide[x][y];
            GeneralBoard[x][y] = IsThereAPiece;
        }
    }  


}

bool MovePawn(int * in_pos[], bool which_side, int *SSide[], int *GenBoard[], int pawnPos[2]) {

    int fileIncrement = 1; if (which_side == black) {fileIncrement *= -1;}
    int x = pawnPos[0]; int y = pawnPos[1];

    if (GenBoard[x][y]) {return false;}

    SSide[x][y], GenBoard[x][y] = false; SSide[x][y + fileIncrement], GenBoard[x][y = fileIncrement] = true;
    in_pos[x][y] = 'a'; in_pos[x][y + fileIncrement] = 'p';

    return true;
}

bool PawnCapture(int * in_pos[], bool which_side, int *SSide[], int *GenBoard[], int pawnPos[2], int piecePos[2]) {


}