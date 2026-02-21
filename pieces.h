#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>

#define black false
#define white true

#define pawn 'p';
#define King 'K';
#define Queen 'Q';
#define Knight 'N';
#define Bishop 'B';
#define Rook 'R';

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
    bool HasKingMoved;
    bool HasAFrookMoved;
    bool HasHFrookMoved;
    bool Which_side;
    bool Attacks[8][8];
    float LVA[8][8];
    int PawnFiles[8];
    move LastMove;
    int KingPos[2];
    int backrank;
    int direction;
} Side;

typedef struct{

    Side WSide;
    Side BSide;

} Board;

float can_castle = 0.0f;

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

float Max(float val1, float val2) {
    if (val1 < val2) {return val2;} return val1;
}

float Min(float val1, float val2) {
    if (val1 > val2) {return val2;} return val1;
}

float GetPieceValue(char piece) {

    switch (piece) {
            case 'p':
                return 1.0f;
            case 'K':
                return 200.0f;
            case 'Q':
                return 9.0f;
            case 'N':
                return 3.25f;
            case 'B':
                return 3.5f;
            case 'R':
                return 5.0f;
            default:
                break;
    }

}

void RegisterMove(int x, int y, int PiecePos[2], move * Buff, int * ind, Side * Opp_side, char piece) {

    if (!DoesSquareExist(x, y)) {return;}
    int ox = PiecePos[0]; int oy = PiecePos[1];

    int dind = *ind;
    
    if (y == Opp_side->backrank && piece=='p') {
            Opp_side->Attacks[x][y] = true;
            Opp_side->LVA[x][y] = Min(Opp_side->LVA[x][y], GetPieceValue('p'));

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

    Opp_side->Attacks[x][y] = true;
    Opp_side->LVA[x][y] = Min(Opp_side->LVA[x][y], GetPieceValue(piece));
    Buff[dind].x = x; Buff[dind].y = y;
    Buff[dind].ox = ox; Buff[dind].oy = oy;
    Buff[dind].promotion = 'a';
    Buff[dind].piece = Opp_side->PieceTypes[ox][oy];
    *ind = *ind + 1;

}

bool IsTheMove(char * movename, move Move) {
    //return (alphabet[Move.x] == movename[1] && (Move.y + 1) == atoi(movename[2]) && Move.piece == movename[0]);
}

bool GetPawnB(int PawnPos[2], Side SSide) {
    int x = PawnPos[0]; int y = PawnPos[1];

    if (!((x > -1 && x<8) && (y > -1 && y<8))) {return false;}

    return SSide.PieceTypes[x][y] == 'p' && SSide.Pieces[x][y];
}

bool GetAPawn(int x, int s, Side SSide) {
    if (!((x > -1 && x<8) && (s > -1 && s<8))) {return false;}

    int which_dire = SSide.direction;

    for (int sy = SSide.backrank; sy!=s; sy+=which_dire) {
        if (SSide.PieceTypes[x][sy] == 'p' && SSide.Pieces[x][sy]) {return true;}
    }

    return false;
}

int GetFurthestPawn(int x, Side SSide) {
    int which_dire = SSide.direction;

    if (!(x > -1 && x<8)) {return false;}

    int by = SSide.backrank + 1;

    for (int sy = SSide.backrank; sy!=(7-SSide.backrank); sy+=which_dire) {
        if (SSide.PieceTypes[x][sy] == 'p' && SSide.Pieces[x][sy]) {by = sy;}
    }

    return by;
}

bool DoesFHavePawns(int x, Side SSide) {
    if (!(x > -1 && x<8)) {return false;}
    return SSide.PawnFiles[x] > 0;
}

bool IsPawnPassed(int f, int l, Side Opp_side) {
  
  bool ep1 = GetAPawn(f+1, l, Opp_side);
  bool ep2 = GetAPawn(f-1, l, Opp_side);
  bool ep3 = GetAPawn(f, l, Opp_side);

  return !ep1 && !ep2 && !ep3;
}

void MovePawn(int x, int nx, Side SSide) {

    if (nx == x) {return;}

    SSide.PawnFiles[x] -= 1; SSide.PawnFiles[nx] += 1;

}

void DestroyPawn(int x, Side SSide) {
    SSide.PawnFiles[x] -= 1;
}

int GetAttack(int pos[2], Side Cur_side, Side Opp_side, int Increment[2], move * Squares, int control) {

    int x = pos[0]; int y = pos[1];
    int buf_x = x; int buf_y = y;
    int control_buf = control;
    char piece = Cur_side.PieceTypes[x][y];

    while (true) {

        buf_x += Increment[0]; buf_y += Increment[1];
        bool enemy_piece = Opp_side.Pieces[buf_x][buf_y];

        if (!DoesSquareExist(buf_x, buf_y) || Cur_side.Pieces[buf_x][buf_y]) {break;}

        RegisterMove(buf_x, buf_y, pos, Squares, &control_buf, &Opp_side, piece);
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

    if (piece == 'K') {
        Cur_side->KingPos[0] = x; Cur_side->KingPos[1] = y;
    }

    if (Opp_side->KingPos[0] == x && Opp_side->KingPos[1] == y) {
        Opp_side->KingPos[0] = -1;
    }

    return;

}

void Destroy_Piece(Side * Cur_side, Side * Opp_side, int piecePos[2]) {

    int x = piecePos[0]; int y = piecePos[1];

    Cur_side->PieceTypes[x][y] = 'a';
    Opp_side->PieceTypes[x][y] = 'a';
    Cur_side->Pieces[x][y] = false;
    Opp_side->Pieces[x][y] = false;

    return;

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

    int new_y2 = new_y + fileIncrement;
    if (y==PawnWall && !IsThereAPiece(Cur_side, Opp_side, x, new_y2) && !IsThereAPiece(Cur_side, Opp_side, x, new_y)) {
        RegisterMove(x, new_y2, PawnPos, Moves, &count, &Opp_side, 'p');
    }

    if (!IsThereAPiece(Cur_side, Opp_side, x, new_y)) {
        RegisterMove(x, new_y, PawnPos, Moves, &count, &Opp_side, 'p');
    }

    if (poscap1) {
        RegisterMove(x + 1, new_y, PawnPos, Moves, &count, &Opp_side, 'p');
    }
    if (poscap2) {
        RegisterMove(x - 1, new_y, PawnPos, Moves, &count, &Opp_side, 'p');
    }
    
    int rec_x = Opp_side.LastMove.x; int rec_y = Opp_side.LastMove.y;
    int dify = abs(rec_y - Opp_side.LastMove.oy); int difx = abs(rec_x - x);
    if (rec_y == y && Opp_side.PieceTypes[rec_x][rec_y] == 'p' && dify == 2 && difx == 1) {
        RegisterMove(rec_x, rec_y + fileIncrement, PawnPos, Moves, &count, &Opp_side, 'p');
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

    can_castle = 0.0f;

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

    return count;

}

bool IsBackRankAttacked(int rx, int kx, Side Cur_side) {
    bool check = false;

    int inc = (kx - rx > 0) ? 1 : -1;
    int backrank = Cur_side.backrank;

    for (int ind = rx; ind != kx+inc; ind += inc) {   
      check = check || Cur_side.Attacks[ind][backrank];
    }
    for (int ind = rx+inc; ind != kx; ind += inc) {   
      check = check || Cur_side.PieceTypes[ind][backrank] != 'a';
    }

    return check;
}

bool CanCastle(Side Cur_side, move * MoveBuf, int * ind) {
    
    int len = 0;
    bool check = false;
    int backrank = Cur_side.backrank;

    bool canshortcastle = false; bool canlongcastle = false;

    if (Cur_side.HasKingMoved) {;return check;}

    int rx = 0; int kx = Cur_side.KingPos[0];
    if (!Cur_side.HasHFrookMoved) {
      rx = 7;
      canshortcastle = !IsBackRankAttacked(rx, kx, Cur_side);
      if (canshortcastle) {

          move ShortCastle; ShortCastle.promotion = 'K';
          ShortCastle.piece = 'K'; ShortCastle.x = 6;
          ShortCastle.y = backrank; ShortCastle.oy = backrank;
          ShortCastle.ox = kx; MoveBuf[len] = ShortCastle; len++;
      }
    }
    if (!Cur_side.HasAFrookMoved) {
        rx = 0;
        canlongcastle = !IsBackRankAttacked(rx, kx, Cur_side);
        if (canlongcastle) {
          move LongCastle; LongCastle.promotion = 'K';
          LongCastle.piece = 'K'; LongCastle.x = 3;
          LongCastle.y = backrank; LongCastle.oy = backrank;
          LongCastle.ox = kx; MoveBuf[len] = LongCastle;
      }
    }

    *ind = *ind + (int)canshortcastle + (int)canlongcastle;

    return canshortcastle || canlongcastle;

    return false;

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

void MakeAMove(move Move, Side * Cur_side, Side * Opp_side) {
    char alphabet[] = "abcdefgh";

    int ox = Move.ox; int oy = Move.oy;
    int OgPos[2] = {ox, oy};
    int NewPos[2] = {Move.x, Move.y};
    int OgOtherPiece[2];
    int nx = Move.x; int ny = Move.y;
    char piece = Move.piece;

    Cur_side->LastMove = Move;

    switch (piece) {
        //There's probably a smarter way to do what I'm doing, but this'll work for now
        case 'K':
            Cur_side->KingPos[0] = nx; Cur_side->KingPos[1] = ny;
            Cur_side->HasKingMoved = true;
            if (Move.promotion == 'K') { //i.e: if the king castled

                int increment = 1;
                switch(nx) {
                    case 2:
                        increment = 1;
                        break;
                    case 6:
                        increment = -1;
                        break;
                }

                MovePiece(OgPos, NewPos, Cur_side, Opp_side, piece);

                int rx = (int)(3.5f - (3.5f * (float)increment));
                OgOtherPiece[0] = rx; OgOtherPiece[1] = Cur_side->backrank;
                Destroy_Piece(Cur_side, Opp_side, OgOtherPiece);

                NewPos[0] = nx+increment;
                MovePiece(OgPos, NewPos, Cur_side, Opp_side, 'R');
            } else {MovePiece(OgPos, NewPos, Cur_side, Opp_side, piece);
            }

            break;
        case 'p':
            OgOtherPiece[0] = nx; OgOtherPiece[1] = ny;

            if (abs(nx - ox) == abs(ny - oy) && Opp_side->PieceTypes[nx][ny] == 'a') {

                OgOtherPiece[1] = oy;

                Destroy_Piece(Opp_side, Cur_side, OgOtherPiece);
                MovePiece(OgPos, NewPos, Cur_side, Opp_side, piece);
                MovePawn(ox, nx, *Cur_side);
                DestroyPawn(ox, *Opp_side);
                
            } else if (ny == Opp_side->backrank) {
                Destroy_Piece(Cur_side, Opp_side, OgPos);
                DestroyPawn(ox, *Cur_side);
                if (Opp_side->PieceTypes[nx][ny] == 'p') {
                    DestroyPawn(nx, *Opp_side);
                }
                Create_Piece(Move.promotion, Cur_side, Opp_side, NewPos);
            } else {
                if (Opp_side->PieceTypes[nx][ny] == 'p') {
                    DestroyPawn(nx, *Opp_side);
                }
                MovePawn(ox, nx, *Cur_side);
                MovePiece(OgPos, NewPos, Cur_side, Opp_side, piece);
            }
            break;
        default:
            MovePiece(OgPos, NewPos, Cur_side, Opp_side, piece);
            if (Opp_side->PieceTypes[nx][ny] == 'p') {
                DestroyPawn(nx, *Opp_side);
            }
            break;

    }
    //This is really stupid but it'll work for now
    if (!Cur_side->HasAFrookMoved && !Cur_side->Pieces[0][Cur_side->backrank]) {
      Cur_side->HasAFrookMoved = true;
    }
    if (!Cur_side->HasHFrookMoved && !Cur_side->Pieces[7][Cur_side->backrank]) {
      Cur_side->HasHFrookMoved = true;
    }                                                         
    if (!Opp_side->HasAFrookMoved && !Opp_side->Pieces[0][Opp_side->backrank]) {
      Opp_side->HasAFrookMoved = true;
    }
    if (!Opp_side->HasHFrookMoved && !Opp_side->Pieces[7][Opp_side->backrank]) {
      Opp_side->HasHFrookMoved = true;
    }                                                         
}
