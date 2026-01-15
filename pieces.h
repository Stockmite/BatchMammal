#include <stdio.h>
#include <stdbool.h>

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
    char promotion;
} move;

typedef struct {
    int Pieces[8][8];
    char PieceTypes[8][8];
    bool HasKingMoved;
    bool HasAFrookMoved;
    bool HasHFrookMoved;
    bool IsBackrankAttacked; //easier than checking each time
    bool Which_side;
    PawnNode BaseNodes[8];
    PawnNode PawnNodes[8];
    int KingPos[2];
    int backrank;
    int direction;
} Side;

typedef struct {
    int move;
    bool turn;
    int Last_move[2];
} Pendulum;

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


int GetAttack(int pos[2], Side Cur_side, Side Opp_side, int Increment[2], move * Squares, int control) {

    int x = pos[0]; int y = pos[1];
    int buf_x = x; int buf_y = y;
    int control_buf = control;

    while (true) {

        buf_x += Increment[0]; buf_y += Increment[1];
        bool enemy_piece = Opp_side.Pieces[buf_x][buf_y];

        if (!DoesSquareExist(buf_x, buf_y) || Cur_side.Pieces[buf_x][buf_y]) {break;}

        Squares[control_buf].x = buf_x; Squares[control_buf].y = buf_y;
        Squares[control_buf].promotion = 'a';
        control_buf++;
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

    int x = piecePos[0]; int y = piecePos[0];
    Cur_side->PieceTypes[x][y] = 'a';
    Opp_side->PieceTypes[x][y] = 'a';
    Cur_side->Pieces[x][y] = false;
    Opp_side->Pieces[x][y] = false;

}

bool CanItPromote(Side Opp_side, int NewPos[2], move * Moves, int *control) {

    int en_backrank = Opp_side.backrank;

    char Promotions[] = "QRBN";

    int y = NewPos[1];
    if (y == en_backrank) {
        for (int a = 0; a < 4; a++) {
            Moves[*control].x = NewPos[0]; Moves[*control].y = y;
            Moves[*control].promotion = Promotions[a];
            *control = *control + 1;
        }

        return true;
    }

    return false;

}

int PawnMoves(Side Cur_side, Side Opp_side, int PawnPos[2], move * Moves, Pendulum MoveOrder) {
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
        NewPos[0] = x; NewPos[1] = new_y;
        bool CanIt = CanItPromote(Opp_side, NewPos, Moves, &count);
        if (!CanIt) {
            Moves[count].x = x; Moves[count].y = new_y; count++;
        }
    }

    int new_y2 = new_y + fileIncrement;
    if (y==PawnWall && !IsThereAPiece(Cur_side, Opp_side, x, new_y2)) {
        Moves[count].x = x; Moves[count].y = new_y2; count++;
    }

    if (poscap1) {
        NewPos[0] = x + 1; NewPos[1] = new_y;
        bool CanIt = CanItPromote(Opp_side, NewPos, Moves, &count);
        if (!CanIt) {
            Moves[count].x = x + 1; Moves[count].y = new_y; count++;
        }
    }
    if (poscap2) {
        NewPos[0] = x - 1; NewPos[1] = new_y;
        bool CanIt = CanItPromote(Opp_side, NewPos, Moves, &count);
        if (!CanIt) {
            Moves[count].x = x - 1; Moves[count].y = new_y; count++;
        }
    }
    
    int rec_x = MoveOrder.Last_move[0]; int rec_y = MoveOrder.Last_move[1];
    if (rec_y == y && Opp_side.PieceTypes[rec_x][rec_y] == 'p') {
        Moves[count].x = rec_x; Moves[count].y = rec_y;
        count++;
    }

    if (count < 12) {
        Moves = realloc(Moves, sizeof(move) * count);
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
                    
                    Moves[count].x = x1; Moves[count].y = y1;
                    count++;
                }
                if (DoesSquareExist(x2, y2) && (!Cur_side.Pieces[x2][y2] || enemy_piece2)) {
                    Moves[count].x = x2; Moves[count].y = y2;
                    count++;
                }
            }
        }

    if (count < 8) {
        Moves = realloc(Moves, sizeof(move) * count);
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

            Moves[count].x = new_x; Moves[count].y = new_y;
            count++;
        }
    }

    if (!(Cur_side.HasKingMoved && Cur_side.IsBackrankAttacked)) {
        if (!Cur_side.HasAFrookMoved) {
            count++;
            Moves[count].x = 3;Moves[count].y = Cur_side.backrank;
        }
        if (!Cur_side.HasHFrookMoved) {
            count++;
            Moves[count].x = 6;Moves[count].y = Cur_side.backrank;
        }
    }

    if (count < 8) {
        Moves = realloc(Moves, sizeof(move) * count);
    }

    return count;

}

int BishopMoves(Side Cur_side, Side Opp_side, int BishopPos[2], move * Moves, bool Reallocate) {

    int control = 0;
    int Increment[2] = {1,1}; control = GetAttack(BishopPos, Cur_side, Opp_side, Increment, Moves, control);
    Increment[0] = -1; control = GetAttack(BishopPos, Cur_side, Opp_side, Increment, Moves, control);
    Increment[1] = -1; control = GetAttack(BishopPos, Cur_side, Opp_side, Increment, Moves, control);
    Increment[0] = 1; control = GetAttack(BishopPos, Cur_side, Opp_side, Increment, Moves, control);

    if (control < 13 && Reallocate) {
        Moves = realloc(Moves, sizeof(move) * control);
    }

    return control;

}

int RookMoves(Side Cur_side, Side Opp_side, int RookPos[2], move * Moves, bool Reallocate) {

    int control = 0;
    int Increment[2] = {1,0}; control = GetAttack(RookPos, Cur_side, Opp_side, Increment, Moves, control);
    Increment[0] = -1; control = GetAttack(RookPos, Cur_side, Opp_side, Increment, Moves, control);
    Increment[0] = 0;
    Increment[1] = 1; control = GetAttack(RookPos, Cur_side, Opp_side, Increment, Moves, control);
    Increment[1] = -1; control = GetAttack(RookPos, Cur_side, Opp_side, Increment, Moves, control);

    if (control < 14 && Reallocate) {
       Moves = realloc(Moves, sizeof(move) * control);
    } 

    return control;

}

int QueenMoves(Side Cur_side, Side Opp_side, int QueenPos[2], move * Moves) {

    int control = RookMoves(Cur_side, Opp_side, QueenPos, Moves, false);
    control += BishopMoves(Cur_side, Opp_side, QueenPos, Moves+control, false);

    if (control < 20) {
        Moves = realloc(Moves, sizeof(move) * control);
    }

    return control;

}

int GetFirstRook(int dire, Side Cur_side) {

    for (int x = 0; x < 8; x++) {
        
    }

}

void MovePiece(int ox, int oy, move Move, Side * Cur_side, Side * Opp_side, char piece) {
    
    int nx = Move.x; int ny = Move.y;

    Cur_side->Pieces[nx][ny] = true;
    Cur_side->Pieces[ox][oy] = false;
    Opp_side->Pieces[nx][ny] = false;

    Cur_side->PieceTypes[ox][oy] = 'a';
    Opp_side->PieceTypes[ox][oy] = 'a';
    Cur_side->PieceTypes[nx][ny] = piece;
    Opp_side->PieceTypes[nx][ny] = piece;

}

void MakeAMove(move Move, int OgPos[2], Side * Cur_side, Side * Opp_side, char piece) {
    int ox = OgPos[0]; int oy = OgPos[1];
    int nx = Move.x; int ny = Move.y;

    switch (piece) {
        case 'K':
            if (Move.promotion = 'K') { //i.e: if the king castled
                switch(ny) {
                    
                }
            }
            break;
        default:
            MovePiece(ox, oy, Move, Cur_side, Opp_side, piece);
            break;

    }

}
