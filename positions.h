#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pieces.h"

char * Get_Pieces(Side Cur_side) {

    char * AllPieces = (char*)malloc(sizeof(char) * 17);
    int count = 0;
    
    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (Cur_side.Pieces[x][y]) {
                AllPieces[count] = Cur_side.PieceTypes[x][y];
                count++;
            }
        }
    }

    AllPieces[count] = '\0'; AllPieces = (char*)realloc(AllPieces, count + 1);
    return AllPieces;

}

float sum_material(char * PieceTypes) {
    float sum = 0;

    for (int x = 0; x < strlen(PieceTypes); x++) {
        char piece = PieceTypes[x];

        switch (piece) {
            case 'p':
                sum++;
                break;
            case 'K':
                sum += 200.0;
            break;
            case 'Q':
                sum += 9.0;
                break;
            case 'N':
                sum += 3.25;
                break;
            case 'B':
                sum += 3.5;
                break;
            case 'R':
                sum += 5;
                break;
            default:
                break;
            }
    }

    return sum;

}

float KingSafety(Side Cur_side, Side Opp_side, int KingPos[2], char* OppPieces) {

    float safety = 0;
    int x = KingPos[0]; int y = KingPos[1];

    int lx = fabs((float)x - 3.5) + 0.5;
    int ly = fabs((float)y - 3.5) + 0.5;
    


}

float EvaluateSpecificPosition(Side WSide, Side BSide) {
    char * Wpieces = Get_Pieces(WSide);
    char * Bpieces = Get_Pieces(BSide);

    float wvalue = sum_material(Wpieces);
    float bvalue = sum_material(Bpieces);
    float material = wvalue - bvalue;

    
}