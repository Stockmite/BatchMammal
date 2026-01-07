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

    int x = KingPos[0]; int y = KingPos[1];

    float lx = fabs((float)x - 3.5) + 0.5;
    float ly = fabs((float)y - 3.5) + 0.5;

    float ChMaPower = 0.6;
    for (int ind = 0; ind < strlen(OppPieces); ind++) {
        switch (OppPieces[ind]) {
            case 'Q':
                ChMaPower -= 0.3;
                break;
            case 'R':
                ChMaPower -= 0.2;
                break;
            case 'N':
                ChMaPower -= 0.1;
                break;
            case 'B':
                ChMaPower -= 0.15;
                break;
        }

    }
    
    float xDebuff = lx * 0.1; int yDebuff = ly * 0.05;

    return (xDebuff + yDebuff) * ChMaPower;

}

float PawnStructure(Side Cur_side, Side Opp_side) {

    float structure = 0.0;

    bool OccupiedFiles[8] = {false};

    for (int p = 0; p < 8; p++) {
        int x = Cur_side.Pawns[p][0]; int y = Cur_side.Pawns[p][1];

        if (OccupiedFiles[y]) {structure -= 0.1;}
        else {OccupiedFiles[y] = true;}

        int ny = y + 1; int prx = y - 1;
        int next_f = (ny > -1 && ny < 8) ? OccupiedFiles[ny] : false;
        int prev_f = (prx > -1 && prx < 8) ? OccupiedFiles[prx] : false;

        if (OccupiedFiles[y] && !(next_f || prev_f)) {
            structure -= 0.1;
        }
    }

}

float EvaluateSpecificPosition(Side WSide, Side BSide) {
    char * Wpieces = Get_Pieces(WSide);
    char * Bpieces = Get_Pieces(BSide);

    float wvalue = sum_material(Wpieces);
    float bvalue = sum_material(Bpieces);
    float material = wvalue - bvalue;

    
}