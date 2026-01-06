#include <stdio.h>
#include <stdlib.h>

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

float sum_material(int S_side[8][8], char Piecepositions[8][8]) {
    float sum = 0;

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (S_side[x][y]) {
                char piece = Piecepositions[x][y];

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
        }
    }

    return sum;

}

float KingSafety(Side Cur_side, Side Opp_side) {



}

float EvaluateSpecificPosition(Side WSide, Side BSide) {
    float wvalue = sum_material(WSide.Pieces, WSide.PieceTypes);
    float bvalue = sum_material(BSide.Pieces, BSide.PieceTypes);
    float material = wvalue - bvalue;

    
}