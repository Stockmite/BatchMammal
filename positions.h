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
        bool ep3 = GetAPawn(p, far_y, Opp_side.BaseNodes, Opp_side.direction);

        if (DoesFHavePawns(p-1, Cur_side.BaseNodes) && DoesFHavePawns(p+1, Cur_side.BaseNodes)) {
            structure += 0.1;
        }
        if (ep1 && ep2 && ep3) {
            structure -= 0.1; //TODO: Fix this later
        }
    }

    return structure;

}

float KnightActivity(int KnightPos[2], Side Cur_side, Side Opp_side) {
    int x = KnightPos[0]; int y = KnightPos[1];
    int dire = Cur_side.direction;
    
    float activity = 0.0;

    int conv_y = (Cur_side.direction == 1) ? y : 7 - y;
    int conv_x = (int)(fabs(3.5 - x) + 0.5);
    activity += ((float)(5 - x)/10) + (conv_y * 0.1);

    activity += (float)GetAPawn(x + 1, y, Cur_side.BaseNodes, Cur_side.direction) / 10.0;
    activity += (float)GetAPawn(x - 1, y, Cur_side.BaseNodes, Cur_side.direction) / 10.0;

    activity -= (float)GetAPawn(x + 1, y, Opp_side.BaseNodes, Opp_side.direction) / 5.0;
    activity -= (float)GetAPawn(x - 1, y, Opp_side.BaseNodes, Opp_side.direction) / 5.0;

    return activity;
}

float RookActivity(int RookPos[2], Side Cur_side, Side Opp_side) {

    int rx = RookPos[0]; int ry = RookPos[1];
    float activity = 0.0;

    for (int p = 0; p < 8; p++) {
        PawnNode * Node1 = &(Cur_side.BaseNodes[p]);
        PawnNode * Node2 = &(Opp_side.BaseNodes[p]);

        while(Node1->next != NULL) {
            Node1 = Node1->next;
            if (Node1->y == ry || p == rx) {
                activity -= 0.1;
            }
        }

        while(Node2->next != NULL) {
            Node2 = Node2->next;
            if (Node2->y == ry || p == rx) {
                activity -= 0.1;
            }
        }
    }

    return activity;

}

float BishopActivity(int BishopPos[2], Side Cur_side, Side Opp_side) {

    int bx = BishopPos[0]; int by = BishopPos[1];
    float activity = 0.0;

    for (int p = 0; p < 8; p++) {
        PawnNode * Node1 = &(Cur_side.BaseNodes[p]);
        PawnNode * Node2 = &(Opp_side.BaseNodes[p]);

        while(Node1->next != NULL) {
            Node1 = Node1->next;

            int dx = p - bx; int dy = Node1->y - by;

            if (abs(dx) == abs(dy)) {
                activity -= 0.1;
            }
        }

        while(Node2->next != NULL) {
            Node2 = Node2->next;

            int PawnPos1[2] = {p, Node2->y + 1};
            int PawnPos2[2] = {p, Node2->y - 1};
            int PawnPos3[2] = {p, Node2->y};

            bool p1 = GetPawnB(PawnPos1, Opp_side.BaseNodes);
            bool p2 = GetPawnB(PawnPos2, Opp_side.BaseNodes);
            bool p3 = GetPawnB(PawnPos3, Opp_side.BaseNodes);
            bool SColor = GetSquareColor(bx, by) == Node2->y;

            int dx = p - bx; int dy = Node1->y - by;
            if (abs(dx) == abs(dy)) {
                activity -= 0.1;
            }

            if (p3 && p1 && SColor) {activity -= 0.1;}
            if (p3 && p2 && SColor) {activity -= 0.1;}
        }
    }

    return activity;
    
}

float QueenActivity(int QueenPos[2], Side Cur_side, Side Opp_side) {

    int qx = QueenPos[0]; int qy = QueenPos[1];
    float activity = 0.0;

    for (int p = 0; p < 8; p++) {
        PawnNode * Node1 = &(Cur_side.BaseNodes[p]);
        PawnNode * Node2 = &(Opp_side.BaseNodes[p]);


        while(Node1->next != NULL) {
            Node1 = Node1->next;

            int dx = p - qx; int dy = Node1->y - qy;

            if (abs(dx) == abs(dy)) {
                activity -= 0.1;
            }

            if (Node1->y == qy || p == qx) {
                activity -= 0.1;
            }
        }

        while(Node2->next != NULL) {
            Node2 = Node2->next;

            int dx = p - qx; int dy = Node2->y - qy;

            if (abs(dx) == abs(dy)) {
                activity -= 0.1;
            }

            if (Node2->y == qy || p == qx) {
                activity -= 0.1;
            }
        }
    }

    return activity;

}

float PawnActivity(int PawnPos[2], Side Cur_side, Side Opp_side) {
    int x = PawnPos[0]; int y = PawnPos[1];

    float lx = fabs((float)x - 3.5) + 0.5;
    float ly = fabs((float)y- 3.5) + 0.5;
    return (1 - (1/lx)) + (1 - (1/ly));
}

float EvaluateSpecificPosition(Side WSide, Side BSide) {

    char * Wpieces = Get_Pieces(WSide);
    char * Bpieces = Get_Pieces(BSide);

    float wvalue = sum_material(Wpieces);
    float bvalue = sum_material(Bpieces);

    float wking_safety = KingSafety(WSide, BSide, WSide.KingPos, Bpieces);
    float bking_safety = KingSafety(BSide, WSide, BSide.KingPos, Wpieces);

    float wstructure = PawnStructure(WSide, BSide);
    float bstructure = PawnStructure(BSide, WSide);

    float material = wvalue - bvalue;
    float structure = wstructure - bstructure;
    float king_safety = wking_safety - bking_safety;

    float wactivity = 0.0;
    float bactivity = 0.0;

    float * act_ptr = &wactivity;

    Side * Cur_side = &WSide;
    Side * Opp_side = &BSide;

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            int Pos[2] = {x,y};

            if (WSide.Pieces[x][y]) {
                act_ptr = &wactivity;
                Cur_side = &WSide;
                Opp_side = &BSide;
            }
            else if (BSide.Pieces[x][y]) {
                act_ptr = &bactivity;
                Cur_side = &BSide;
                Opp_side = &WSide;
            }

            switch (Cur_side->PieceTypes[x][y]) {
                case 'p':
                    *act_ptr += PawnActivity(Pos, *Cur_side, *Opp_side);
                case 'Q':
                    *act_ptr += QueenActivity(Pos, *Cur_side, *Opp_side);
                case 'N':
                    *act_ptr += KnightActivity(Pos, *Cur_side, *Opp_side);
                case 'R':
                    *act_ptr += RookActivity(Pos, *Cur_side, *Opp_side);
                case 'B':
                    *act_ptr += BishopActivity(Pos, *Cur_side, *Opp_side);
            }
        }
    }

    float activity = wactivity - bactivity;
    
    return activity + king_safety + structure + material;

    
}