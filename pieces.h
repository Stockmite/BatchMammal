#include <stdio.h>
#include <stdbool.h>

#define black false
#define white true

typedef struct {
    int WSide[8][8];
    int Bside[8][8];
    int GenBoard[8][8]; //feels useful enough to define it as a value rather than just rederiving it
    char PiecePositions[8][8];

} position;

//README: Please treat "true" as 1 and "false" as 0

void GetGeneralBoard(int * BlackSide[], int * WhiteSide[], int * GeneralBoard[]) {

    for (int x = 0; x < 8; x++)  {
        for (int y = 0; y < 8; y++) {
            int IsThereAPiece = BlackSide[x][y] & WhiteSide[x][y];
            GeneralBoard[x][y] = IsThereAPiece;
        }
    }  


}

void Create_Piece(char piece, bool which_side, position * cur_pos, int piecePos[2]) {

    int x = piecePos[0]; int y = piecePos[0];
    cur_pos->PiecePositions[x][y] = piece;
    cur_pos->GenBoard[x][y] = true;

    if (which_side == black) {cur_pos->Bside[x][y] = true;}
    else {cur_pos->WSide[x][y] = true;}

}

void Destroy_Piece(char piece, bool which_side, position * cur_pos, int piecePos[2]) {

    int x = piecePos[0]; int y = piecePos[0];
    cur_pos->PiecePositions[x][y] = 'a';
    cur_pos->GenBoard[x][y] = false;

    if (which_side == black) {cur_pos->Bside[x][y] = false;}
    else {cur_pos->WSide[x][y] = false;}

}

bool MovePawn(bool which_side, position * cur_pos, int pawnPos[2]) {
    
    int x = pawnPos[0]; int y = pawnPos[1];
    if (cur_pos->GenBoard[x][y]) {return false;}

    int fileIncrement = 1; if (which_side == black) {fileIncrement *= -1;}
    int newPos[2] = {x, y + fileIncrement};

    Destroy_Piece()

    return true;
}

bool PawnCapture(int * in_pos[], bool which_side, int *SSide[], int *GenBoard[], int pawnPos[2], int piecePos[2]) {

    

}