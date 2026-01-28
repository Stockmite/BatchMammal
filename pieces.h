#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#include "PositionalModules\pawnstructure.h"

#define black false
#define white true

#define pawn 'p';
#define King 'K';
#define Queen 'Q';
#define Knight 'N';
#define Bishop "B";
#define Rook "R";

typedef struct {
    int x;
    int y;
    int ox; int oy;
    char promotion;
    char piece;
} move;

typedef struct {
    int Pieces[8][8];
    char PieceTypes[8][8];
    int Attacks[8][8];
    bool HasKingMoved;
    bool HasAFrookMoved;
    bool HasHFrookMoved;
    bool IsBackrankAttacked; //easier than checking each time
    bool Which_side;
    PawnNode BaseNodes[8];
    PawnNode PawnNodes[8];
    move LastMove;
    int KingPos[2];
    int backrank;
    int direction;
} Side;

typedef struct{

    Side WSide;
    Side BSide;

} Board;

//README: Please treat "true" as 1 and "false" as 0

bool GetSquareColor(int l, int f) {
    return (l % 2) != (f % 2);
}

bool IsThereAPiece(Side Cur_side, Side Opp_side, int x, int y) {

    return Opp_side.Pieces[x][y] || Cur_side.Pieces[x][y];

}

bool DoesSquareExist(int l, int f) {

    if ((l > -1 && l<8) && (f > -1 && f<8)) {return true;}
    return false;
}

void RegisterMove(int x, int y, int PiecePos[2], move * Buff, int * ind, Side * Opp_side, char piece) {

    if (!DoesSquareExist(x, y)) {return;}
    int ox = PiecePos[0]; int oy = PiecePos[1];

    int dind = *ind;
    
    if (y == Opp_side->backrank) {
        Opp_side->IsBackrankAttacked = true;

        if (piece=='p') {
            char Promotions[] = "QRBN";
            for (int a = 0; a < strlen(Promotions); a++) {
                Buff[dind+a].x = x; Buff[dind+a].y = y;
                Buff[dind+a].ox = ox; Buff[dind+a].oy = oy; 
                Buff[dind+a].promotion = Promotions[a];
                Buff[dind+a].piece = 'p';
                *ind = *ind + 1;
            }
            return;
        }
    }

    Buff[dind].x = x; Buff[dind].y = y;
    Buff[dind].ox = ox; Buff[dind].oy = oy;
    Buff[dind].promotion = 'a';
    Buff[dind].piece = Opp_side->PieceTypes[ox][oy];
    *ind = *ind + 1;

}

int GetAttack(int pos[2], Side Cur_side, Side Opp_side, int Increment[2], move * Squares, int control) {

    int x = pos[0]; int y = pos[1];
    int buf_x = x; int buf_y = y;
    int control_buf = control;

    while (true) {

        buf_x += Increment[0]; buf_y += Increment[1];
        bool enemy_piece = Opp_side.Pieces[buf_x][buf_y];

        if (!DoesSquareExist(buf_x, buf_y) || Cur_side.Pieces[buf_x][buf_y]) {break;}

        RegisterMove(buf_x, buf_y, pos, Squares, &control_buf, &Opp_side, 'a');
        if (enemy_piece) {break;}

    }

    return control_buf;

}

void Create_Piece(char piece, Side * Cur_side, Side * Opp_side, int piecePos[2]) {

    int x = piecePos[0]; int y = piecePos[1];
    Cur_side->PieceTypes[x][y] = piece;
    Opp_side->PieceTypes[x][y] = piece;
    Cur_side->Pieces[x][y] = true;
    Opp_side->Pieces[x][y] = false;

}

void Destroy_Piece(Side * Cur_side, Side * Opp_side, int piecePos[2]) {

    int x = piecePos[0]; int y = piecePos[1];
    Cur_side->PieceTypes[x][y] = 'a';
    Opp_side->PieceTypes[x][y] = 'a';
    Cur_side->Pieces[x][y] = false;
    Opp_side->Pieces[x][y] = false;

}

int PawnMoves(Side Cur_side, Side Opp_side, int PawnPos[2], move * Moves) {
    //Behold: if statements!
    int count = 0;
    int x = PawnPos[0]; int y = PawnPos[1];
    int fileIncrement = Cur_side.direction;
    int PawnWall = Cur_side.backrank + fileIncrement;
    int new_y = y + fileIncrement;

    int poscap1 = Opp_side.Pieces[x + 1][new_y];
    int poscap2 = Opp_side.Pieces[x - 1][new_y];

    int NewPos[2] = {x, new_y};

    if (!(Opp_side.Pieces[x][new_y] || Cur_side.Pieces[x][new_y])) {
        RegisterMove(x, new_y, PawnPos, Moves, &count, &Opp_side, 'p');
    }

    int new_y2 = new_y + fileIncrement;
    if (y==PawnWall && !IsThereAPiece(Cur_side, Opp_side, x, new_y2)) {
        RegisterMove(x, new_y2, PawnPos, Moves, &count, &Opp_side, 'p');
    }

    if (poscap1) {
        RegisterMove(x + 1, new_y, PawnPos, Moves, &count, &Opp_side, 'p');
    }
    if (poscap2) {
        RegisterMove(x - 1, new_y, PawnPos, Moves, &count, &Opp_side, 'p');
    }
    
    int rec_x = Opp_side.LastMove.x; int rec_y = Opp_side.LastMove.y;
    if (rec_y == y && Opp_side.PieceTypes[rec_x][rec_y] == 'p') {
        RegisterMove(rec_x, rec_y, PawnPos, Moves, &count, &Opp_side, 'p');
    }

    return count;

}

int KnightMoves(Side Cur_side, Side Opp_side, int KnightPos[2], move * Moves) {

    int x = KnightPos[0]; int y = KnightPos[1];

    int count = 0;
    for (int b = -2; b <= 2; b = b + 4) {
            for (int a = -1; a <= 1; a = a + 2) {
                int x1 = x+a; int x2 = x+b;
                int y1 = y+b; int y2 = y+a;

                bool enemy_piece1 = Opp_side.Pieces[x1][y1];
                bool enemy_piece2 = Opp_side.Pieces[x2][y2];

                if (DoesSquareExist(x1, y1) && (!Cur_side.Pieces[x1][y1] || enemy_piece1)) {
                    RegisterMove(x1, y1, KnightPos, Moves, &count, &Opp_side, 'N');
                }
                if (DoesSquareExist(x2, y2) && (!Cur_side.Pieces[x2][y2] || enemy_piece2)) {
                    RegisterMove(x2, y2, KnightPos, Moves, &count, &Opp_side, 'N');
                }
            }
        }

    return count;

}

int KingMoves(Side Cur_side, Side Opp_side, int KingPos[2], move * Moves) {

    int x = KingPos[0]; int y = KingPos[1];
    int count = 0;

    for (int a = -1; a < 2; a++) {
        for (int b = -1; b < 2; b++) {
            int new_x = x+a; int new_y =y+b;
            bool does_square_exist = DoesSquareExist(new_x, new_y);
            bool does_square_have_enemy_piece = Opp_side.Pieces[new_x][new_y];

            if ((new_x== x && new_y == y) || !does_square_exist) {continue;}
            if (!does_square_have_enemy_piece && IsThereAPiece(Cur_side, Opp_side, new_x, new_y)) {continue;}

            RegisterMove(new_x, new_y, KingPos, Moves, &count, &Opp_side, 'K');
        }
    }

    if (!(Cur_side.HasKingMoved && Cur_side.IsBackrankAttacked)) {
        if (!Cur_side.HasAFrookMoved) {
            RegisterMove(3, Cur_side.backrank, KingPos, Moves, &count, &Opp_side, 'K');
            Moves[count - 1].promotion = 'K';  //slight alteration to make it clear this is about castling
        }
        if (!Cur_side.HasHFrookMoved) {
            RegisterMove(6, Cur_side.backrank, KingPos, Moves, &count, &Opp_side, 'K');
        }
    }

    return count;

}

int BishopMoves(Side Cur_side, Side Opp_side, int BishopPos[2], move * Moves) {

    int control = 0;
    int Increment[2] = {1,1}; control = GetAttack(BishopPos, Cur_side, Opp_side, Increment, Moves, control);
    Increment[0] = -1; control = GetAttack(BishopPos, Cur_side, Opp_side, Increment, Moves, control);
    Increment[1] = -1; control = GetAttack(BishopPos, Cur_side, Opp_side, Increment, Moves, control);
    Increment[0] = 1; control = GetAttack(BishopPos, Cur_side, Opp_side, Increment, Moves, control);

    return control;

}

int RookMoves(Side Cur_side, Side Opp_side, int RookPos[2], move * Moves) {

    int control = 0;
    int Increment[2] = {1,0}; control = GetAttack(RookPos, Cur_side, Opp_side, Increment, Moves, control);
    Increment[0] = -1; control = GetAttack(RookPos, Cur_side, Opp_side, Increment, Moves, control);
    Increment[0] = 0;
    Increment[1] = 1; control = GetAttack(RookPos, Cur_side, Opp_side, Increment, Moves, control);
    Increment[1] = -1; control = GetAttack(RookPos, Cur_side, Opp_side, Increment, Moves, control);

    return control;

}

int QueenMoves(Side Cur_side, Side Opp_side, int QueenPos[2], move * Moves) {

    int control = RookMoves(Cur_side, Opp_side, QueenPos, Moves);
    control += BishopMoves(Cur_side, Opp_side, QueenPos, Moves+control);

    return control;

}

int GetFirstRook(int dire, Side Cur_side) {
    int br = Cur_side.backrank;

    for (int x = 4-(4*dire); x < 4+(4*dire); x=x+dire) {
        if (Cur_side.PieceTypes[x][br] == 'R') {return x;}
    }

    return -1;

}

void MovePiece(int OgPos[2], int NewPos[2], Side * Cur_side, Side * Opp_side, char piece) {

    int ox = OgPos[0]; int oy = OgPos[1];
    int nx = NewPos[0]; int ny = NewPos[0];

    Destroy_Piece(Cur_side, Opp_side, OgPos);
    Create_Piece(piece, Cur_side, Opp_side, NewPos);
}

void MakeAMove(move Move, Side * Cur_side, Side * Opp_side, char piece) {

    int ox = Move.ox; int oy = Move.oy;
    int OgPos[2] = {ox, oy};
    int NewPos[2] = {Move.x, Move.y};
    int OgOtherPiece[2];
    int nx = Move.x; int ny = Move.y;

    switch (piece) {
        //There's probably a smarter way to do what I'm doing, but this'll work for now
        case 'K':
            if (Move.promotion = 'K') { //i.e: if the king castled

                int increment = 1;
                switch(nx) {
                    case 6:
                        increment = 1;
                        break;
                    case 3:
                        increment = -1;
                        break;
                }

                NewPos[0] = nx+increment;

                MovePiece(OgPos, NewPos, Cur_side, Opp_side, piece);

                int rx = GetFirstRook(increment, *Cur_side);
                OgOtherPiece[0] = rx; OgOtherPiece[1] = Cur_side->backrank;
                NewPos[0] = nx+increment;
                MovePiece(OgPos, NewPos, Cur_side, Opp_side, 'R');
            } else {MovePiece(OgPos, NewPos, Cur_side, Opp_side, piece);}

            break;
        case 'p':
            OgOtherPiece[0] = nx; OgOtherPiece[1] = ny;

            if (abs(nx - ox) == abs(ny - oy) && Opp_side->PieceTypes[nx][ny] == 'a') {

                OgOtherPiece[1] = oy;

                MovePiece(OgPos, NewPos, Cur_side, Opp_side, piece);
                MovePawn(OgPos, NewPos, Cur_side->BaseNodes);
                Destroy_Piece(Opp_side, Cur_side, OgOtherPiece);
                DestroyPawn(OgOtherPiece, Cur_side->BaseNodes);
                
            } else if (ny == Opp_side->backrank) {
                Destroy_Piece(Cur_side, Opp_side, OgPos);
                DestroyPawn(OgPos, Cur_side->BaseNodes);
                Create_Piece(Move.promotion, Cur_side, Opp_side, NewPos);
            } else {
                if (Opp_side->PieceTypes[nx][ny] == 'p') {
                    DestroyPawn(NewPos, Cur_side->BaseNodes);
                }
                MovePawn(OgPos, NewPos, Cur_side->BaseNodes);
                MovePiece(OgPos, NewPos, Cur_side, Opp_side, piece);
            }
            break;
        default:
            MovePiece(OgPos, NewPos, Cur_side, Opp_side, piece);
            break;

    }

}
