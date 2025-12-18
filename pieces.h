#include <stdio.h>

#define false black
#define true white

void GetGeneralBoard(int * BlackSide[], int * WhiteSide[], int * GeneralBoard[]) {

    for (int x = 0; x < 8; x++)  {
        for (int y = 0; y < 8; y++) {
            int IsThereAPiece = BlackSide[x][y] & WhiteSide[x][y];
            GeneralBoard[x][y] = IsThereAPiece;
        }
    }  


}

void MovePawn(int * in_pos[], bool which_side, int * BlackSide[], int * WhiteSide[], int * GeneralBoard[]) {

    
    

}