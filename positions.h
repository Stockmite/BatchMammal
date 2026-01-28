#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pieces.h"

char alphabet[] = "abcdefgh";

#define depth 3

void ViewBoard(Board CurBoard) {
    for (int y=0; y<8; y++) {
        for (int x=0; x<8; x++) {
            printf("%c ", CurBoard.BSide.PieceTypes[7 - x][7 - y]);
        }
        printf("\n");
    }

    printf("\n \n \n");

}

float RoundFloatValue(float val) {

    int buf_val = (int)(val * 100.0f);
    return (float)buf_val / 100.0f;

}

char * Get_Pieces(Side Cur_side) {

    char * AllPieces = (char*)malloc(sizeof(char) * 18);
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

float KingSafety(Side Cur_side, int KingPos[2], char* OppPieces) {

    float safety = 0.0;

    int kx = KingPos[0]; int ky = KingPos[1];

    float lx = fabs((float)kx - 3.5f) + 0.5f;
    float ly = fabs((float)ky - 3.5f) + 0.5f;

    float ChMaPower = 0.6;
    for (int ind = 0; ind < strlen(OppPieces); ind++) {
        switch (OppPieces[ind]) {
            case 'Q':
                ChMaPower -= 0.3f;
                break;
            case 'R':
                ChMaPower -= 0.2f;
                break;
            case 'N':
                ChMaPower -= 0.1f;
                break;
            case 'B':
                ChMaPower -= 0.15f;
                break;
        }

    }

    for (int a = -1; a < 2; a++) {
        for (int b = -1; b < 2; b++) {
            int x = kx + a; int y = ky + b;
            if (!DoesSquareExist(x, y)) {continue;}
            if (!Cur_side.Pieces[x][y]) {
                if (Cur_side.Pieces[kx + a][ky + b]) {safety += 0.015f;}
                safety -= 0.025f;
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
    
    float activity = 0.0f;

    int conv_y = (Cur_side.direction == 1) ? y : 7 - y;
    int conv_x = (int)(fabs(3.5f - x) + 0.5f);
    activity += ((float)(5 - conv_x)/10.0f) + (conv_y * 0.1f);

    activity += (float)GetAPawn(x + 1, y, Cur_side.BaseNodes, Cur_side.direction) / 10.0f;
    activity += (float)GetAPawn(x - 1, y, Cur_side.BaseNodes, Cur_side.direction) / 10.0f;

    activity -= (float)GetAPawn(x + 1, y, Opp_side.BaseNodes, Opp_side.direction) / 5.0f;
    activity -= (float)GetAPawn(x - 1, y, Opp_side.BaseNodes, Opp_side.direction) / 5.0f;

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
    float activity = 0.0f;

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
                activity -= 0.1f;
            }

            if (p3 && p1 && SColor) {activity -= 0.1f;}
            if (p3 && p2 && SColor) {activity -= 0.1f;}
        }
    }

    return activity;
    
}

float QueenActivity(int QueenPos[2], Side Cur_side, Side Opp_side) {

    int qx = QueenPos[0]; int qy = QueenPos[1];
    float activity = 0.0f;

    for (int p = 0; p < 8; p++) {
        PawnNode * Node1 = &(Cur_side.BaseNodes[p]);
        PawnNode * Node2 = &(Opp_side.BaseNodes[p]);


        while(Node1->next != NULL) {
            Node1 = Node1->next;

            int dx = p - qx; int dy = Node1->y - qy;

            if (abs(dx) == abs(dy)) {
                activity -= 0.1f;
            }

            if (Node1->y == qy || p == qx) {
                activity -= 0.1f;
            }
        }

        while(Node2->next != NULL) {
            Node2 = Node2->next;

            int dx = p - qx; int dy = Node2->y - qy;

            if (abs(dx) == abs(dy)) {
                activity -= 0.1f;
            }

            if (Node2->y == qy || p == qx) {
                activity -= 0.1f;
            }
        }
    }

    return activity;

}

float PawnActivity(int PawnPos[2]) {
    int x = PawnPos[0]; int y = PawnPos[1];

    float lx = fabs((float)x - 3.5f) + 0.5f;
    float ly = fabs((float)y- 3.5f) + 0.5f;
    float b = (1.0f - (1.0f/lx)) + (1.0f - (1.0f/ly));
    return RoundFloatValue(b);
}

move * EvaluateSpecificPosition(Board CurBoard, float * eval_buf, int * ind, bool turn) {

    move * CandidateMoves = malloc(sizeof(move));
    Side WSide = CurBoard.WSide;
    Side BSide = CurBoard.BSide;
    Side Sides[2] = {BSide, WSide};

    char * Wpieces = Get_Pieces(WSide);
    char * Bpieces = Get_Pieces(BSide);

    float wvalue = sum_material(Wpieces);
    float bvalue = sum_material(Bpieces);

    float wking_safety = KingSafety(WSide, WSide.KingPos, Bpieces);
    float bking_safety = KingSafety(BSide, BSide.KingPos, Wpieces);

    free(Wpieces); Wpieces = NULL;
    free(Bpieces); Bpieces = NULL;

    float wstructure = PawnStructure(WSide, BSide);
    float bstructure = PawnStructure(BSide, WSide);

    float material = wvalue - bvalue;
    float structure = wstructure - bstructure;
    float king_safety = wking_safety - bking_safety;

    float activity = 0.0f;
    float dire = 1.0f;

    Side * Cur_side = &WSide;
    Side * Opp_side = &BSide;

    activity = 0.0F;
    CandidateMoves = malloc(sizeof(move));

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            int Pos[2] = {x,y};

            if (WSide.Pieces[x][y]) {
                dire = 1.0f;
                Cur_side = &WSide;
                Opp_side = &BSide;
            }
            else if (BSide.Pieces[x][y]) {
                dire = -1.0f;
                Cur_side = &BSide;
                Opp_side = &WSide;
            }

            switch (Cur_side->PieceTypes[x][y]) {
                case 'p':
                    activity += PawnActivity(Pos) * dire;
                    break;
                case 'Q':
                    activity += QueenActivity(Pos, *Cur_side, *Opp_side) * dire;
                    break;
                case 'N':
                    activity += KnightActivity(Pos, *Cur_side, *Opp_side) * dire;
                    break;
                case 'R':
                    activity += RookActivity(Pos, *Cur_side, *Opp_side) * dire;
                    break;
                case 'B':
                    activity += BishopActivity(Pos, *Cur_side, *Opp_side) * dire;
                    break;
                default:
                    break;
            }

            if (Sides[turn].Pieces[x][y]) {
                int len = *ind;
                switch (Cur_side->PieceTypes[x][y]) {
                case 'p':
                    CandidateMoves = realloc(CandidateMoves, sizeof(move) * (len + 12));
                    *ind = len + PawnMoves(*Cur_side, *Opp_side, Pos, CandidateMoves+len);
                    CandidateMoves = realloc(CandidateMoves, sizeof(move) * (*ind+1));
                    break;
                case 'Q':
                    CandidateMoves = realloc(CandidateMoves, sizeof(move) * (len + 27));
                    *ind = len + QueenMoves(*Cur_side, *Opp_side, Pos, CandidateMoves+len);
                    CandidateMoves = realloc(CandidateMoves, sizeof(move) * (*ind+1));
                    break;
                case 'N':
                    CandidateMoves = realloc(CandidateMoves, sizeof(move) * (len + 8));
                    *ind = len + KnightMoves(*Cur_side, *Opp_side, Pos, CandidateMoves+len);
                    CandidateMoves = realloc(CandidateMoves, sizeof(move) * (*ind+1));
                    break;
                case 'R':
                    CandidateMoves = realloc(CandidateMoves, sizeof(move) * (len + 14));
                    *ind = len + RookMoves(*Cur_side, *Opp_side, Pos, CandidateMoves+len);
                    CandidateMoves = realloc(CandidateMoves, sizeof(move) * (*ind+1));
                    break;
                case 'B':
                    CandidateMoves = realloc(CandidateMoves, sizeof(move) * (len + 13));
                    *ind = len + BishopMoves(*Cur_side, *Opp_side, Pos, CandidateMoves+len);
                    CandidateMoves = realloc(CandidateMoves, sizeof(move) * (*ind+1));
                    break;
                default:
                    break;
            }
            }
        }
    }

    if (*ind > 0) {CandidateMoves = realloc(CandidateMoves, sizeof(move) * (*ind));}
    
    *eval_buf = activity + king_safety + structure + material;

    return CandidateMoves;
    
}



float JudgeABranch(Board CurBoard, move * CandidateMoves, int len, int cur_depth, bool turn, move * BestMove) {

    float BestLineVal = 0.0f;

    Side BufW = CurBoard.WSide;
    Side BufB = CurBoard.BSide;
    Side BufSides[2] = {BufB, BufW};

    Side * Cur_side = &BufW;
    Side * Opp_side = &BufB;
    Side * Buf_Ptr = Cur_side;
    bool cur_turn = turn;

    if (turn == black) {
        Cur_side = Opp_side;
        Opp_side = &BufW;
        Buf_Ptr = Cur_side;
    }

    for (int ind = 0; ind < len; ind++) {
        
                move ChosenMove = CandidateMoves[ind];
                int PiecePos[2] = {ChosenMove.ox, ChosenMove.oy};
                MakeAMove(ChosenMove, Cur_side, Opp_side, ChosenMove.piece);

                float bufval = 0.0f;
                int buflen = 0;
                move BufBeMo;
                Board TempBoard = {BufW, BufB};
                move * BufMoves = EvaluateSpecificPosition(TempBoard, &bufval, &buflen, turn);
                
                if (ind == 0) {BestLineVal = bufval; *BestMove = ChosenMove;}
                if (cur_depth+1 < depth) {
                    bufval = JudgeABranch(TempBoard, BufMoves, buflen, cur_depth+1, !turn, &BufBeMo); //Behold, recursive functions!
                }

                bool IsBestMove = (turn == white) ? bufval > BestLineVal : bufval < BestLineVal;
                if (IsBestMove) {BestLineVal = bufval; *BestMove = ChosenMove;}
                //if(BestMove->y==2){printf("best move is %c %c%d -> %f \n", ChosenMove.piece, alphabet[ChosenMove.x], ChosenMove.y, bufval);};

                *Cur_side = BufSides[turn];
                *Opp_side = BufSides[!turn];
                
            }

    free(CandidateMoves);
    CandidateMoves = NULL;
    return BestLineVal;

}

float Evaluate(Board CurBoard, move * BestMove, bool turn) {

    int len = 0;
    int cur_depth = 0;
    float CurEvaluation = 0.0f;

    move * CandidateMoves = EvaluateSpecificPosition(CurBoard, &CurEvaluation, &len, turn);
    CurEvaluation = JudgeABranch(CurBoard, CandidateMoves, len, cur_depth+1, turn, BestMove);

    return CurEvaluation;

}