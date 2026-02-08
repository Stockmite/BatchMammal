#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pieces.h"

char alphabet[] = "abcdefgh";

#define depth 6
int contr = 0;


//This is going to tank performance, but I am desperate to see this engine working
float GetAttackStren(int Increment[2], int Pos[2], Side SSide) {
    int x = Pos[0]; int y = Pos[1];
    int BufPos[2] = {x, y};

    float strenght = 0.0f;
    while (DoesSquareExist(BufPos[0], BufPos[1])) {
        BufPos[0] += Increment[0]; BufPos[1] += Increment[1];
        if (SSide.PieceTypes[BufPos[0]][BufPos[1]] != 'a') {
            if (strenght < 0.0f) {return 0.2f;}
            strenght -= 0.1f;
        }
    }

    return strenght;
}

float GetAttackLen(int Increment[2], int Pos[2], Side SSide) {
    int x = Pos[0]; int y = Pos[1];
    int BufPos[2] = {x, y};

    float val = 0.0f;
    while (DoesSquareExist(BufPos[0], BufPos[1])) {
        BufPos[0] += Increment[0]; BufPos[1] += Increment[1];
        if (SSide.PieceTypes[BufPos[0]][BufPos[1]] == 'a') {
            val += 0.5f;
        }
    }

    return val;
}

void ViewBoard(Board CurBoard) {
    for (int y=0; y<8; y++) {
        for (int x=0; x<8; x++) {
            printf("%c ", CurBoard.BSide.PieceTypes[x][7 - y]);
        }
        printf("\n");
    }

    printf("\n \n \n");

    //for (int y=0; y<8; y++) {
        //for (int x=0; x<8; x++) {
            //printf("%d ", CurBoard.BSide.Pieces[x][7 - y]);
        //}
        //printf("\n");
    //}

    //printf("\n \n \n");

    //for (int y=0; y<8; y++) {
        //for (int x=0; x<8; x++) {
            //printf("%d ", CurBoard.WSide.Pieces[x][7 - y]);
        //}
        //printf("\n");
    //}

    //printf("\n \n \n");

}

void AssignMoveArray(move * Array1, move * Array2, int len) {
    for (int ind = 0; ind < len; ind++) {
        Array1[ind] = Array2[ind];
    }
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

float sum_material(char * PieceTypes, int * piece_count) {
    float sum = 0;

    for (int x = 0; x < strlen(PieceTypes); x++) {
        char piece = PieceTypes[x];

        sum+=GetPieceValue(piece);
        *piece_count = *piece_count+1;
    }

    return sum;

}

float KingSafety(Side Cur_side, int KingPos[2], char* OppPieces) {

    float safety = 0.0f;

    int kx = KingPos[0]; int ky = KingPos[1];

    float lx = fabs((float)kx - 3.5f) + 0.5f;
    float ly = fabs((float)ky - 3.5f) + 0.5f;

    float ChMaPower = -0.5f; //omg
    for (int ind = 0; ind < strlen(OppPieces); ind++) {
        switch (OppPieces[ind]) {
            case 'Q':
                ChMaPower += 0.3f;
                break;
            case 'R':
                ChMaPower += 0.2f;
                break;
            case 'N':
                ChMaPower += 0.1f;
                break;
            case 'B':
                ChMaPower += 0.15f;
                break;
        }

    }

    for (int a = -1; a < 2; a++) {
        for (int b = -1; b < 2; b++) {
            int x = kx + a; int y = ky + b;
            if (!DoesSquareExist(x, y)) {continue;}
            if (!Cur_side.Pieces[x][y] && !((a == 0) && (b == 0))) {
                int Increment[2] = {a, b};
                safety -= (GetAttackLen(Increment, KingPos, Cur_side) + 0.025f) * ChMaPower;
            }
        }
    }
    
    float xDebuff = lx * 0.15; int yDebuff = ly * 0.1;

    safety += (xDebuff + yDebuff) * ChMaPower;

    return safety;

}

float PawnStructure(Side Cur_side, Side Opp_side) {

    float structure = 0.0;

    for (int p = 0; p < 8; p++) {
        structure -= 0.1 * (float)Cur_side.PawnFiles[p];

        int far_y = GetFurthestPawn(p, Cur_side);
        bool ep1 = GetAPawn(p+1, far_y, Opp_side);
        bool ep2 = GetAPawn(p-1, far_y, Opp_side);
        bool ep3 = GetAPawn(p, far_y, Opp_side);

        if (DoesFHavePawns(p-1, Cur_side) && DoesFHavePawns(p+1, Cur_side)) {
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

    activity += (float)GetAPawn(x + 1, y, Cur_side) / 10.0f;
    activity += (float)GetAPawn(x - 1, y, Cur_side) / 10.0f;

    activity -= (float)GetAPawn(x + 1, y, Opp_side) / 5.0f;
    activity -= (float)GetAPawn(x - 1, y, Opp_side) / 5.0f;

    return activity;
}

float RookActivity(int RookPos[2], Side Cur_side, Side Opp_side) {

    int rx = RookPos[0]; int ry = RookPos[1];
    float activity = 0.0;
    int Increment[2] = {0,1};

    activity += GetAttackStren(Increment, RookPos, Cur_side);
    Increment[1] = -1;
    activity += GetAttackStren(Increment, RookPos, Cur_side);
    Increment[1] = 0; Increment[0] = 1;
    activity += GetAttackStren(Increment, RookPos, Cur_side);
    Increment[0] = -1;
    activity += GetAttackStren(Increment, RookPos, Cur_side);

    activity *= 0.5;

    return activity;

}

float BishopActivity(int BishopPos[2], Side Cur_side, Side Opp_side) {

    int bx = BishopPos[0]; int by = BishopPos[1];
    float activity = 0.0f;
    int Increment[2] = {1,1};

    activity += GetAttackStren(Increment, BishopPos, Cur_side); Increment[0] = -1;
    activity += GetAttackStren(Increment, BishopPos, Cur_side); Increment[1] = -1;
    activity += GetAttackStren(Increment, BishopPos, Cur_side); Increment[0] = 1;
    activity += GetAttackStren(Increment, BishopPos, Cur_side);

    activity *= 0.5;

    return activity;
    
}

float QueenActivity(int QueenPos[2], Side Cur_side, Side Opp_side) {


    int qx = QueenPos[0]; int qy = QueenPos[1];
    float activity = BishopActivity(QueenPos, Cur_side, Opp_side) + RookActivity(QueenPos, Cur_side, Opp_side);

    return activity * 0.05;

}

float PawnActivity(int PawnPos[2], int dire) {
    int x = PawnPos[0]; int y = PawnPos[1];

    float lx = fabs((float)x - 3.5f) + 0.5f;
    float ly = (dire == 1) ? (float)y : (float)(7 - y);
    float b = (ly/lx);
    return b;
}

move * EvaluateSpecificPosition(Board CurBoard, float * eval_buf, int * ind, int * piece_count, bool turn) {
    
    move * CandidateMoves = (move*)malloc(sizeof(move));
    Side WSide = CurBoard.WSide;
    Side BSide = CurBoard.BSide;
    Side Sides[2] = {BSide, WSide};

    char * Wpieces = Get_Pieces(WSide);
    char * Bpieces = Get_Pieces(BSide);

    float wvalue = sum_material(Wpieces, piece_count);
    float bvalue = sum_material(Bpieces, piece_count);

    if (wvalue < 200.0f) {
        *ind = 0;
        *eval_buf = -2000.0f;
        return CandidateMoves;
    } else if (bvalue < 200.0f) {
        *ind = 0;
        *eval_buf = 2000.0f;
        return CandidateMoves;
    }

    if (wvalue == 200.0f || bvalue == 200.0f) {
        if (wvalue == bvalue) {
            *ind = 0;
            *eval_buf = 0.0f;
            return CandidateMoves;
        }
    }

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
            Sides[!turn].Attacks[x][y] = 0;
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

            if (Cur_side->Pieces[x][y] || Opp_side->Pieces[x][y]) {
                char piece = Cur_side->PieceTypes[x][y];
                float dif = Cur_side->Attacks[x][y] - GetPieceValue(piece);
                if (dif < 0 && piece != 'K') {material -= dif * dire;}
            }

            switch (Cur_side->PieceTypes[x][y]) {
                case 'p':
                    activity += PawnActivity(Pos, dire) * dire;
                    break;
                case 'Q':
                    //activity += QueenActivity(Pos, *Cur_side, *Opp_side) * dire;
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
                    activity += (float)(*ind - len) * dire;
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
                    activity += (float)(*ind - len) * dire;
                    CandidateMoves = realloc(CandidateMoves, sizeof(move) * (*ind+1));
                    break;
                case 'K':
                    CandidateMoves = realloc(CandidateMoves, sizeof(move) * (len + 8));
                    *ind = len + KingMoves(*Cur_side, *Opp_side, Pos, CandidateMoves+len);
                    CandidateMoves = realloc(CandidateMoves, sizeof(move) * (*ind+1));
                    //king_safety -= ((float)(*ind - len) * 0.05f) * dire;
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



float JudgeABranch(Board CurBoard, move * CandidateMoves, int len, int cur_depth, bool turn, move * BestMove, float alpha, float beta) {
    contr++;
    float BestLineVal = 0.0f;

    Side BufW = CurBoard.WSide;
    Side BufB = CurBoard.BSide;
    Side BufSides[2] = {BufB, BufW};
    float alphabuf = alpha; float betabuf = beta;

    Side * Cur_side = &BufW;
    Side * Opp_side = &BufB;

    if (turn == black) {
        Cur_side = &BufB;
        Opp_side = &BufW;
    }

    for (int ind = 0; ind < len; ind++) {
                

                move ChosenMove = CandidateMoves[ind];
                int PiecePos[2] = {ChosenMove.ox, ChosenMove.oy};
                MakeAMove(ChosenMove, Cur_side, Opp_side);
                
                move BestMoveBuf;

                float bufval = 0.0f;
                int buflen = 0;
                Board TempBoard = {BufW, BufB};
                int buf = 0;
                move * BufMoves = EvaluateSpecificPosition(TempBoard, &bufval, &buflen, &buf, !turn);

                if (fabs(bufval) == 2000.0f) {
                    BestLineVal = bufval;
                    break;
                }

                
                if (cur_depth+1 < depth) {
                    bufval = JudgeABranch(TempBoard, BufMoves, buflen, cur_depth+1, !turn, &BestMoveBuf, alphabuf, betabuf);
                    bufval += can_castle;
                     //Behold, recursive functions!
                } else {free(BufMoves); BufMoves = NULL;}
                if (cur_depth == 0) {
                    printf("%c-> %c%d (%c%d): %f\n",ChosenMove.piece, alphabet[ChosenMove.x], ChosenMove.y + 1, alphabet[ChosenMove.ox], ChosenMove.oy + 1, bufval);
                }
                if (ind == 0) {
                    BestLineVal = bufval;
                    *BestMove = ChosenMove;

                }

                bool IsBestMove = (turn == white) ? bufval > BestLineVal : bufval < BestLineVal;
                if (IsBestMove) {
                    BestLineVal = bufval;
                    *BestMove = ChosenMove;
                }

                if (turn) {
                    if (alphabuf < bufval) {alphabuf = bufval;}
                } else {if (betabuf > bufval) {betabuf = bufval;}}

                if (betabuf <= alphabuf) {
                    break;
                }

                *Cur_side = CurBoard.WSide;
                *Opp_side = CurBoard.BSide;


                if (turn == black) {
                    *Cur_side = CurBoard.BSide;
                    *Opp_side = CurBoard.WSide;
                }
                
    }

    free(CandidateMoves);
    CandidateMoves = NULL;
    return BestLineVal;

}

float Evaluate(Board CurBoard, move * BestMove, bool turn) {

    int len = 0;
    int cur_depth = 0;
    float CurEvaluation = 0.0f;
    float alpha = -2000.0f; float beta = 2000.0f;

    int piece_count = 0;
    move * CandidateMoves = EvaluateSpecificPosition(CurBoard, &CurEvaluation, &len, &piece_count, turn);

    int dif = (32 - piece_count);
    cur_depth -= (dif - (dif % 8))/8;
    printf("depth:%d \n", depth - cur_depth);

    CurEvaluation = JudgeABranch(CurBoard, CandidateMoves, len, cur_depth, turn, BestMove, alpha, beta);
    printf("Positions analyzed: %d\n", contr);

    return CurEvaluation;

}