#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pieces.h"

char alphabet[] = "abcdefgh";

#define depth 4
int contr = 0;

typedef move * MoveList;

void GetSideFromSquare(int x, int y, Side * Cur_side, Side * Opp_side, Board CurBoard) {

    if (CurBoard.WSide.Pieces[x][y]) {
        *Cur_side = CurBoard.WSide;
        *Opp_side = CurBoard.BSide;
    } else if (CurBoard.BSide.Pieces[x][y]) {
        *Cur_side = CurBoard.BSide;
        *Opp_side = CurBoard.WSide;
    }

}

MoveList OrderMoves(MoveList UnorderedMoves, int len, Board CurBoard) {

    MoveList OrderedMoves = malloc(sizeof(move)*len);

    MoveList Captures = malloc(sizeof(move)); int len1 = 0;
    MoveList RegularMoves = malloc(sizeof(move)); int len2 = 0;
    MoveList Sacrifices = malloc(sizeof(move)); int len3 = 0;

    for (int ind = 0; ind < len; ind++) {
        move CurMove = UnorderedMoves[ind];
        int nx = CurMove.x; int ny = CurMove.y;

        Side Cur_side; Side Opp_side;
        GetSideFromSquare(CurMove.ox, CurMove.oy, &Cur_side, &Opp_side, CurBoard);

        char piece = CurMove.piece;
        char en_piece = Cur_side.PieceTypes[nx][ny];
        bool IsMoveUnsafe = Cur_side.Attacks[nx][ny];

        if (en_piece != 'a') {

            if (IsMoveUnsafe) {
                len1++;
                Captures = realloc(Captures, sizeof(move)*len1);
                Captures[len1-1] = CurMove; continue;
            } else {
                if (GetPieceValue(en_piece) > GetPieceValue(piece)) {
                    len1++;
                    Captures = realloc(Captures, sizeof(move)*len1);
                    Captures[len1-1] = CurMove; continue;
                } else if (GetPieceValue(en_piece) < GetPieceValue(piece)) {
                    len3++;
                    Sacrifices = realloc(Sacrifices, sizeof(move)*len3);
                    Sacrifices[len3-1] = CurMove; continue;
                }

            }


        }

        if (IsMoveUnsafe) {
            len3++;
            Sacrifices = realloc(Sacrifices, sizeof(move)*len3);
            Sacrifices[len3-1] = CurMove; continue;
        }

        len2++;
        RegularMoves = realloc(RegularMoves, sizeof(move)*len2);
        RegularMoves[len2-1] = CurMove; continue;
    }

    for (int ind1 = 0; ind1 < len1; ind1++) {
        OrderedMoves[ind1] = Captures[ind1];
    }

    for (int ind2 = 0; ind2 < len2; ind2++) {
        OrderedMoves[len1+ind2] = RegularMoves[ind2];
    }

    for (int ind3 = 0; ind3 < len3; ind3++) {
        OrderedMoves[len1+len2+ind3] = Sacrifices[ind3];
    }

    free(Captures); Captures = NULL;
    free(RegularMoves); RegularMoves = NULL;
    free(Sacrifices); Sacrifices = NULL;

    return OrderedMoves;

}

bool IsthereAcapture(MoveList moves, int len, Board CurBoard) {

    for (int ind = 0; ind < len; ind++) {
        if (IsThereAPiece(CurBoard.WSide, CurBoard.WSide, moves[ind].x, moves[ind].y)) {
            return true;
        }
    }

    return false;

}

float GetAttackStren(int Increment[2], int Pos[2], Side SSide) {
    int x = Pos[0]; int y = Pos[1];
    int buf_x = x + Increment[0]; int buf_y = y + Increment[1];

    float strength = 0.0f;
    while (DoesSquareExist(buf_x, buf_y)) {
        if (SSide.PieceTypes[buf_x][buf_y] == 'p') {
            strength -= 0.1f;
            if (strength == -0.2f) {return strength;}
        } 

        buf_x += Increment[0]; buf_y += Increment[1];
    }

    return strength;
}

float GetAttackLen(int Increment[2], int Pos[2], Side SSide) {
    int x = Pos[0]; int y = Pos[1];
    int BufPos[2] = {x + Increment[0], y + Increment[1]};

    float val = 0.0f;
    while (DoesSquareExist(BufPos[0], BufPos[1])) {
        if (SSide.PieceTypes[BufPos[0]][BufPos[1]] != 'a') {break;}
        val += 0.5f;
        BufPos[0] += Increment[0]; BufPos[1] += Increment[1];
    }

    return val;
}

bool SeesPiece(int Increment[2], int Pos[2], Side SSide) {
    int x = Pos[0]; int y = Pos[1];
    int buf_x = x + Increment[0]; int buf_y = y + Increment[1];

    char piece = SSide.PieceTypes[buf_x][buf_y];

    while (DoesSquareExist(buf_x, buf_y)) {
        char cur_p = SSide.PieceTypes[buf_x][buf_y];

        if (cur_p != 'a') {
            if (cur_p == piece) {return true;}
            else {return false;}
        }

        buf_x += Increment[0]; buf_y += Increment[1];
    }

    return false;
}

void ViewBoard(Board CurBoard) {
    for (int y=0; y<8; y++) {
        for (int x=0; x<8; x++) {
            printf("%c ", CurBoard.BSide.PieceTypes[x][7 - y]);
        }
        printf("\n");
    }

    printf("\n \n \n");

    for (int y=0; y<8; y++) {
        for (int x=0; x<8; x++) {
            printf("%d ", CurBoard.BSide.Attacks[x][7 - y] || CurBoard.WSide.Attacks[x][7 - y] );
        }
        printf("\n");
    }

    printf("\n \n \n");

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

    float ChMaPower = -0.7f; //omg
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
                float sightline = GetAttackLen(Increment, KingPos, Cur_side);
                safety -= (0.05f + sightline*(0.01f - 0.0012f*((sightline+1.0f)/2.0f))) * ChMaPower * 0.05f;
            }
            safety -= (float)Cur_side.Attacks[x][y] * 0.025f;
        }
    }
    
    float xDebuff = lx * 0.4; int yDebuff = ly * 0.6;

    safety += (xDebuff + yDebuff) * ChMaPower;

    return safety;

}

float PawnStructure(Side Cur_side, Side Opp_side) {

    float structure = 0.0;

    for (int p = 0; p < 8; p++) {
      if (Cur_side.PawnFiles[p] > 1) { structure -= 0.025 * (float)Cur_side.PawnFiles[p];} 

        int far_y = GetFurthestPawn(p, Cur_side);
        bool IsPassed = IsPawnPassed(p, far_y, Opp_side);

        if (DoesFHavePawns(p, Cur_side)) {
         if (!(DoesFHavePawns(p-1, Cur_side) || !DoesFHavePawns(p+1, Cur_side))) {
            structure -= 0.025f;
         }
        } else {continue;}
        if (IsPassed) {
            structure += 0.05f; //TODO: Fix this later
        }
    }

    return structure;

}

float KnightActivity(int KnightPos[2], Side Cur_side, Side Opp_side) {

    int x = KnightPos[0]; int y = KnightPos[1];
    int dire = Cur_side.direction;
    
    float activity = 0.0f;

    int conv_y = (Cur_side.direction == 1) ? y : 7 - y;
    int conv_x = (int)(fabs(3.5f - (float)x) + 0.5f);
    activity += ((float)(5 - conv_x) * 0.08f) + (conv_y * 0.075f);

    activity += (float)GetAPawn(x + 1, y, Cur_side) / 8.0;
    activity += (float)GetAPawn(x - 1, y, Cur_side) / 8.0f;

    activity -= (float)GetAPawn(x + 1, y, Opp_side) / 11.0f;
    activity -= (float)GetAPawn(x - 1, y, Opp_side) / 11.0f;

    return activity;
}

float RookActivity(int RookPos[2], Side Cur_side, Side Opp_side) {

    int rx = RookPos[0]; int ry = RookPos[1];
    float activity = 0.0;
    int Increment[2] = {0,1};

    bool SeesOtherRook = false;

    activity += GetAttackStren(Increment, RookPos, Cur_side);
    SeesOtherRook = SeesOtherRook || SeesPiece(Increment, RookPos, Cur_side);

    Increment[1] = -1;
    activity += GetAttackStren(Increment, RookPos, Cur_side);
    SeesOtherRook = SeesOtherRook || SeesPiece(Increment, RookPos, Cur_side);

    Increment[1] = 0; Increment[0] = 1;
    activity += GetAttackStren(Increment, RookPos, Cur_side);
    SeesOtherRook = SeesOtherRook || SeesPiece(Increment, RookPos, Cur_side);

    Increment[0] = -1;
    activity += GetAttackStren(Increment, RookPos, Cur_side);
    SeesOtherRook = SeesOtherRook || SeesPiece(Increment, RookPos, Cur_side);

    activity *= 0.8;

    activity += (float)(ry == Opp_side.backrank + Opp_side.direction) * 0.05f;
    activity += (float)(SeesOtherRook) * 0.05f;

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

    activity *= 0.25f;

     int conv_y = (int)(fabs(3.5f - (float)by) + 0.5f);
     int conv_x = (int)(fabs(3.5f - (float)bx) + 0.5f);
     activity += ((float)(5 - conv_x) * 0.09f) + ((float)(5 - conv_y) * 0.09f);

     bool IsFianchettoed = conv_x == conv_y;
     activity += (float)IsFianchettoed * 0.07f;

     return activity;
    
}

float QueenActivity(int QueenPos[2], Side Cur_side, Side Opp_side) {


    int qx = QueenPos[0]; int qy = QueenPos[1];
    float activity = BishopActivity(QueenPos, Cur_side, Opp_side) + RookActivity(QueenPos, Cur_side, Opp_side);

    return activity * 0.0005f;

}

float PawnActivity(int PawnPos[2], int dire, Side Opp_side) {
    int x = PawnPos[0]; int y = PawnPos[1];

    int rel_y = (dire == 1) ? y : 7 - y;

    float lx = 5.0f - fabs((float)x - 3.5f) + 0.5f;
    float ly = (IsPawnPassed(x,y,Opp_side)) ? (float)rel_y : 5.0f - (fabs((3.5f - (float)y)) + 0.5f);
    float b = lx * ly;
    return b * 0.1f;
}

move * EvaluateSpecificPosition(Board CurBoard, float * eval_buf, int * ind, int * piece_count, bool turn) {

    move * CandidateMoves = (move*)malloc(sizeof(move));
    move * BufMoves = (move*)malloc(sizeof(move));
    int nind = 0;

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

    float wstructure = PawnStructure(WSide, BSide);
    float bstructure = PawnStructure(BSide, WSide);

    float material = wvalue - bvalue;
    float structure = wstructure - bstructure;

    float activity = 0.0f;
    float dire = 1.0f;

    Side * Cur_side = &WSide;
    Side * Opp_side = &BSide;

    activity = 0.0F;
    CandidateMoves = (move*)malloc(sizeof(move));

    for (int a = 0; a < 8; a++) {
        for (int b = 0; b < 8; b++) {
            Cur_side->Attacks[a][b] = false;
            Opp_side->Attacks[a][b] = false;
            Cur_side->LVA[a][b] = 0.0f;
            Opp_side->LVA[a][b] = 0.0f;
        }
    }

    int SquareOrder[] = {4, 3, 5, 2, 6, 1, 7, 0};
    float na, pa, ba, ra, qa = 0.0f;

    float king_safety = 0.0f;

    for (int a = 0; a < 8; a++) {
        unsigned int x = SquareOrder[a];
        for (int b = 0; b < 8; b++) {
            unsigned int y = SquareOrder[b];

            int Pos[2] = {x,y};
            int oind = nind;

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
                    activity += PawnActivity(Pos, dire, *Opp_side) * dire;
                    pa += PawnActivity(Pos, dire, *Opp_side) * dire;
                    break;
                case 'Q':
                    activity += QueenActivity(Pos, *Cur_side, *Opp_side) * dire;
                    qa += QueenActivity(Pos, *Cur_side, *Opp_side) * dire;
                    break;
                case 'N':
                    activity += KnightActivity(Pos, *Cur_side, *Opp_side) * dire;
                    na += KnightActivity(Pos, *Cur_side, *Opp_side) * dire;
                    break;
                case 'R':
                    activity += RookActivity(Pos, *Cur_side, *Opp_side) * dire;
                    ra += RookActivity(Pos, *Cur_side, *Opp_side) * dire;
                    break;
                case 'B':
                    activity += BishopActivity(Pos, *Cur_side, *Opp_side) * dire;
                    ba += BishopActivity(Pos, *Cur_side, *Opp_side) * dire;
                    break;
                default:
                    break;
            }

            if (Sides[turn].Pieces[x][y]) {
                int len = *ind;
                switch (Cur_side->PieceTypes[x][y]) {
                case 'p':
                    CandidateMoves = (move*)realloc(CandidateMoves, sizeof(move) * (len + 12));
                    *ind = len + PawnMoves(Cur_side, Opp_side, Pos, CandidateMoves+len);
                    CandidateMoves = (move*)realloc(CandidateMoves, sizeof(move) * (*ind+1));
                    break;
                case 'Q':
                    CandidateMoves = (move*)realloc(CandidateMoves, sizeof(move) * (len + 27));
                    *ind = len + QueenMoves(Cur_side, Opp_side, Pos, CandidateMoves+len);
                    CandidateMoves = (move*)realloc(CandidateMoves, sizeof(move) * (*ind+1));
                    break;
                case 'N':
                    CandidateMoves = (move*)realloc(CandidateMoves, sizeof(move) * (len + 8));
                    *ind = len + KnightMoves(Cur_side, Opp_side, Pos, CandidateMoves+len);
                    activity += ((float)(*ind - len) * dire) * 0.035f;
                    na += ((float)(*ind - len) * dire) * 0.035f; 
                    CandidateMoves = (move*)realloc(CandidateMoves, sizeof(move) * (*ind+1));
                    break;
                case 'R':
                    CandidateMoves = (move*)realloc(CandidateMoves, sizeof(move) * (len + 14));
                    *ind = len + RookMoves(Cur_side, Opp_side, Pos, CandidateMoves+len);
                    CandidateMoves = (move*)realloc(CandidateMoves, sizeof(move) * (*ind+1));
                    break;
                case 'K':
                    CandidateMoves = (move*)realloc(CandidateMoves, sizeof(move) * (len + 8));
                    *ind = len + KingMoves(Cur_side, Opp_side, Pos, CandidateMoves+len);
                    king_safety -= ((float)(*ind - len) * dire) * 0.00001f;
                    CandidateMoves = (move*)realloc(CandidateMoves, sizeof(move) * (*ind+1));
                    break;                                                                  
                case 'B':
                    CandidateMoves = (move*)realloc(CandidateMoves, sizeof(move) * (len + 13));
                    *ind = len + BishopMoves(Cur_side, Opp_side, Pos, CandidateMoves+len);
                    activity += ((float)(*ind - len) * dire) * 0.05f;
                    ba += ((float)(*ind - len) * dire) * 0.08f;
                    CandidateMoves = (move*)realloc(CandidateMoves, sizeof(move) * (*ind+1));
                    break;
                default:
                    break;                                                                      
            }
            } else if (Sides[!turn].Pieces[x][y]) {
              switch (Cur_side->PieceTypes[x][y]) {                                                                                          
              case 'p':                                                                      
                  BufMoves = (move*)realloc(BufMoves, sizeof(move) * (nind + 12));       
                  nind += PawnMoves(Cur_side, Opp_side, Pos, BufMoves + nind);              
                  break;                                                                                                                                 
              case 'Q':                                                                      
                  BufMoves = (move*)realloc(BufMoves, sizeof(move) * (nind + 27));           
                  nind += QueenMoves(Cur_side, Opp_side, Pos, BufMoves+nind);                                                                             
                  break;                                                                     
              case 'N':                                                                      
                  BufMoves = (move*)realloc(BufMoves, sizeof(move) * (nind + 8));     
                  nind += KnightMoves(Cur_side, Opp_side, Pos, BufMoves+nind);
                  activity += ((float)(nind - oind) * dire) * 0.035f;
                  break;                                                                     
              case 'R':                                                                      
                  BufMoves = (move*)realloc(BufMoves, sizeof(move) * (nind + 14));   
                  nind += RookMoves(Cur_side, Opp_side, Pos, BufMoves+nind);         
                  break;                                                                     
              case 'B':                                                                      
                  BufMoves = (move*)realloc(BufMoves, sizeof(move) * (nind + 14));         
                  nind += BishopMoves(Cur_side, Opp_side, Pos, BufMoves+nind);
                  activity += ((float)(nind - oind) * dire) * 0.08f;
                  break;
              case 'K':                                                           
                  BufMoves = (move*)realloc(BufMoves, sizeof(move) * (nind + 8));       
                  nind += KingMoves(Cur_side, Opp_side, Pos, BufMoves+nind);
                  king_safety -= ((float)(nind - oind) * dire) * 0.00001f;
                  break;                                                          
              default:
                  break;
              
            }
             
        }
    }
    }

      CandidateMoves = (move*)realloc(CandidateMoves, sizeof(move) * (*ind + 5));
      bool CanSideCastle = CanCastle(Sides[turn], CandidateMoves+*ind, ind);
      CandidateMoves = (move*)realloc(CandidateMoves, sizeof(move) * (*ind+2));
    
    free(BufMoves); BufMoves = NULL;

    if (*ind > 0) {CandidateMoves = (move*)realloc(CandidateMoves, sizeof(move) * (*ind));}

    float wking_safety = KingSafety(WSide, WSide.KingPos, Bpieces);
    float bking_safety = KingSafety(BSide, BSide.KingPos, Wpieces);

    king_safety += wking_safety - bking_safety;

    free(Wpieces); Wpieces = NULL; 
    free(Bpieces); Bpieces = NULL;

    *eval_buf = activity + king_safety + structure + material;

    MoveList OrderedMoves = OrderMoves(CandidateMoves, *ind, CurBoard);

    free(CandidateMoves); CandidateMoves = NULL;

    return OrderedMoves;
    
}



float JudgeABranch(Board CurBoard, move * CandidateMoves, int len, int cur_depth, bool turn, move * BestMove, float alpha, float beta) {
    float BestLineVal = 2000.0f;

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

                if (!DoesSquareExist(ChosenMove.x, ChosenMove.y) || !(DoesSquareExist(ChosenMove.ox, ChosenMove.oy))) {
                  printf("%c-> %c%d (%c%d): error\n",ChosenMove.piece, alphabet[ChosenMove.x], ChosenMove.y + 1, alphabet[ChosenMove.ox], ChosenMove.oy + 1);
                  continue;}
                MakeAMove(ChosenMove, Cur_side, Opp_side);

                contr++;
                
                move BestMoveBuf;

                float bufval = 0.0f;
                int buflen = 0;
                Board TempBoard = {BufW, BufB};
                int buf = 0;
                move * BufMoves = EvaluateSpecificPosition(TempBoard, &bufval, &buflen, &buf, !turn);

                if (ind == 0) {
                    BestLineVal = bufval;
                    *BestMove = ChosenMove;                          
                }                  

                if (fabs(bufval) == 2000.0f) {
                  if ((bufval > 0 && turn) || (bufval < 0 && !turn)) {*BestMove = ChosenMove;}
                  int bias = (bufval > 0) ? 1 : -1; //reduces the value depending on how far away it is from
                                                    //checkmate. This is to prevent the engine from instanly
                                                    //resigning the moment it sees forced checkmate
                    BestLineVal = bufval - (50.0f * cur_depth * bias);
                    break;
                }

                int kx = Cur_side->KingPos[0]; int ky = Cur_side->KingPos[1];
                if (Cur_side->Attacks[kx][ky]) {
                    if (ind == 0) {
                        BestLineVal = 2000.0f * Opp_side->direction;
                    }
                    continue;
                }
                 
                if (cur_depth+1 < depth || IsthereAcapture(BufMoves, buflen, TempBoard)) {
                  bufval = JudgeABranch(TempBoard, BufMoves, buflen, cur_depth+1, !turn, &BestMoveBuf, alphabuf, betabuf);
                  if (ind == 0) {
                        BestLineVal = bufval;
                    }
                }
                     //Behold, recursive functions!
               else {free(BufMoves); BufMoves = NULL;}

               if (cur_depth == 0) {
                printf("%c-> %c%d (%c%d) : %f\n",ChosenMove.piece, alphabet[ChosenMove.x], ChosenMove.y + 1, alphabet[ChosenMove.ox], ChosenMove.oy + 1, bufval);
               }

                bool IsBestMove = (turn == white) ? bufval > BestLineVal : bufval < BestLineVal;
                if (IsBestMove) {
                    BestLineVal = bufval;
                    *BestMove = ChosenMove;
                }

                if (turn) {
                    if (alphabuf < bufval) {alphabuf = bufval;}
                    if (betabuf <= alphabuf) {break;}
                } else {
                    {if (betabuf > bufval) {betabuf = bufval;}}
                    if (betabuf <= alphabuf) {break;}
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
    contr = 0;

    return CurEvaluation;

}
