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

    float safety = 0.0;

    int kx = KingPos[0]; int ky = KingPos[1];

    float lx = fabs((float)kx - 3.5) + 0.5;
    float ly = fabs((float)ky - 3.5) + 0.5;

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

    for (int a = -1; a < 2; a++) {
        for (int b = -1; b < 2; b++) {
            int x = kx + a; int y = ky + b;
            if (!DoesSquareExist(x, y)) {continue;}
            if (!Cur_side.Pieces[x][y]) {
                if (Cur_side.Pieces[kx + a][ky + b]) {safety += 0.05;}
                safety -= 0.1;
            }
        }
    }
    
    float xDebuff = lx * 0.1; int yDebuff = ly * 0.05;

    safety += (xDebuff + yDebuff) * ChMaPower;

    return safety;

}

float PawnStructure(Side Cur_side, Side Opp_side) {

    float structure = 0.0;

    for (int p = 0; p < 8; p++) {
        structure -= 0.1 * (float)HowManyPawnsInF(p, Cur_side.BaseNodes);

        int far_y = GetFurthestPawn(p, Cur_side.BaseNodes, Cur_side.direction);
        bool ep1 = GetAPawn(p+1, far_y, Opp_side.BaseNodes, Opp_side.direction);
        bool ep2 = GetAPawn(p-1, far_y, Opp_side.BaseNodes, Opp_side.direction);

        if (DoesFHavePawns(p-1, Cur_side.BaseNodes) && DoesFHavePawns(p+1, Cur_side.BaseNodes)) {
            structure += 0.1;
        }
        if (ep1 && ep2 && DoesFHavePawns(p, Opp_side.BaseNodes)) {
            structure -= 0.1; //TODO: Fix this later
        }
    }

    return structure;

}

float KnightActivity(Side Cur_side, Side Opp_side) {

    

}

float EvaluateSpecificPosition(Side WSide, Side BSide) {
    char * Wpieces = Get_Pieces(WSide);
    char * Bpieces = Get_Pieces(BSide);

    float wvalue = sum_material(Wpieces);
    float bvalue = sum_material(Bpieces);
    float material = wvalue - bvalue;

    
}