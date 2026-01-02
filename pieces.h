#include <stdio.h>
#include <stdbool.h>

#define black false
#define white true

#define pawn 'p';
#define King 'K';
#define Queen 'Q';
#define Knight 'N';
#define Bishop "B";
#define Rook "R";

typedef struct {
    int WSide[8][8];
    int Bside[8][8];
    int GenBoard[8][8]; //feels useful enough to define it as a value rather than just rederiving it
    char PiecePositions[8][8];
    bool HasBKingMoved;
    bool HasWKingMoved;
    bool HasAWrookMoved;
    bool HasHWrookMoved;
    bool HasABrookMoved; //Looks ugly, but is dramatically more efficient than checking the position each time,
    bool HasHBrookMoved; //And I can't be bothered to come up with a more clever way of doing this
    bool IsWBackrankAttacked;
    bool IsBBackrankAttacked;
    int LastMove[2];

} position;

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
    bool Which_side;
    int Pawns[8][2];
    int KingPos[2];
    int backrank;
    int direction;
} Side;

//README: Please treat "true" as 1 and "false" as 0

void GetGeneralBoard(int * BlackSide[], int * WhiteSide[], int * GeneralBoard[]) {

    for (int x = 0; x < 8; x++)  {
        for (int y = 0; y < 8; y++) {
            int IsThereAPiece = BlackSide[x][y] & WhiteSide[x][y];
            GeneralBoard[x][y] = IsThereAPiece;
        }
    }  


}

bool DoesSquareExist(int l, int f) {

    if ((l>0 || l<7) || (f>7 || f<0)) {return true;}
    return false;
}

int GetAttack(int pos[2], Side Cur_side, Side Opp_side, int Increment[2], move * Squares, int control) {

    int x = pos[0]; int y = pos[1];
    int buf_x = x; int buf_y = y;
    int control_buf = control;

    while (true) {

        buf_x += Increment[0]; buf_y += Increment[1];
        bool enemy_piece = Opp_side.Pieces[buf_x][buf_y];

        if (!DoesSquareExist(buf_x, buf_y)) {break;}

        Squares[control_buf].x = buf_x; Squares[control_buf].y = buf_y;
        Squares[control_buf].promotion = 'a';
        control_buf++;
        if (enemy_piece) {break;}

    }

    return control_buf;

}

void Create_Piece(char piece, bool which_side, position * cur_pos, int piecePos[2]) {

    int x = piecePos[0]; int y = piecePos[0];
    cur_pos->PiecePositions[x][y] = piece;
    cur_pos->GenBoard[x][y] = true;

    if (which_side == black) {cur_pos->Bside[x][y] = true;}
    else {cur_pos->WSide[x][y] = true;}

}

void Destroy_Piece(bool which_side, position * cur_pos, int piecePos[2]) {

    int x = piecePos[0]; int y = piecePos[0];
    cur_pos->PiecePositions[x][y] = 'a';
    cur_pos->GenBoard[x][y] = false;

    if (which_side == black) {cur_pos->Bside[x][y] = false;}
    else {cur_pos->WSide[x][y] = false;}

}

bool CanItPromote(Side Cur_side, int NewPos[2], move * Moves, int *control) {

    int en_backrank = Cur_side.backrank;

    char Promotions[] = "QRBN";

    int y = NewPos[0];
    if (y == en_backrank) {
        for (int a = 0; a < 4; a++) {
            Moves[*control].x = NewPos[0]; Moves[*control].y = y;
            Moves[*control].promotion = Promotions[a];
            control++;
        }
    }


}

void PawnMoves(Side Cur_side, Side Opp_side, int PawnPos[2], move * Moves) {
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
        bool CanIt = CanItPromote(Cur_side, NewPos, Moves, &count);
        if (!CanIt) {
            Moves[count].x = x; Moves[count].y = new_y; count++;
        }
    }

    int new_y2 = new_y + fileIncrement;
    if (y==PawnWall && !cur_pos->GenBoard[x][new_y2]) {
        Moves[count].x = x; Moves[count].y = new_y2; count++;
    }

    if (poscap1) {
        NewPos[0] = x + 1; NewPos[1] = new_y;
        bool CanIt = CanItPromote(which_side, cur_pos, NewPos, Moves, &count);
        if (!CanIt) {
            Moves[count].x = x + 1; Moves[count].y = new_y; count++;
        }
    }
    if (poscap2) {
        NewPos[0] = x - 1; NewPos[1] = new_y;
        bool CanIt = CanItPromote(which_side, cur_pos, NewPos, Moves, &count);
        if (!CanIt) {
            Moves[count].x = x - 1; Moves[count].y = new_y; count++;
        }
    }
    
    int rec_x = cur_pos->LastMove[0]; int rec_y = cur_pos->LastMove[1];
    if (rec_y == y && cur_pos->PiecePositions[rec_x][rec_y] == 'p') {
        Moves[count].x = rec_x; Moves[count].y = rec_y;
    }

}

void KnightMoves(bool which_side, position * cur_pos, int KnightPos[2], move * Moves) {

    int x = KnightPos[0]; int y = KnightPos[1];

    int count = 0;
    for (int b = -2; b <= 2; b = b + 4) {
            for (int a = -1; a <= 1; a = a + 2) {
                int x1 = x+a; int x2 = x+b;
                int y1 = y+b; int y2 = y+a;

                bool enemy_piece1 = DoesSquareHaveEnemyPiece(cur_pos, x1, y1, which_side);
                bool enemy_piece2 = DoesSquareHaveEnemyPiece(cur_pos, x2, y2, which_side);

                if (DoesSquareExist(x1, y1) && (!cur_pos->GenBoard[x1][y1] || enemy_piece1)) {
                    
                    Moves[count].x = x1; Moves[count].y = y1;
                    count++;
                }
                if (DoesSquareExist(x2, y2) && (!cur_pos->GenBoard[x2][y2] || enemy_piece2)) {
                    Moves[count].x = x2; Moves[count].y = y2;
                    count++;
                }
            }
        }

}

void KingMoves(bool which_side, position * cur_pos, int KingPos[2], move * Moves) {

    int x = KingPos[0]; int y = KingPos[1];
    int count = 0;

    for (int a = -1; a < 2; a++) {
        for (int b = -1; b < 2; b++) {
            int new_x = x+a; int new_y =y+b;
            bool does_square_exist = DoesSquareExist(new_x, new_y);
            bool does_square_have_enemy_piece = DoesSquareHaveEnemyPiece(cur_pos, new_x, new_y, which_side);

            if ((new_x== x && new_y == y) || !does_square_exist) {continue;}
            if (!does_square_have_enemy_piece && cur_pos->GenBoard[new_x][new_y]) {continue;}

            Moves[count].x = new_x; Moves[count].y = new_y;
            count++;
        }
    }

    bool HasSKingMoved = cur_pos->HasWKingMoved;
    bool IsSBackrakAttacked = cur_pos->IsWBackrankAttacked;
    bool HasSARookMoved = cur_pos->HasAWrookMoved;
    bool HasSHRookMoved = cur_pos->HasHWrookMoved;
    int backrank = 0;

    if (which_side == black) {
        HasSKingMoved = cur_pos->HasBKingMoved;
        HasSARookMoved = cur_pos->HasABrookMoved;
        HasSHRookMoved = cur_pos->HasHBrookMoved;
        IsSBackrakAttacked = cur_pos->IsBBackrankAttacked;
        backrank = 7;
    }

    if (!(HasSKingMoved && IsSBackrakAttacked)) {
        if (!HasSARookMoved) {
            count++;
            Moves[count].x = 3;Moves[count].y = backrank;
        }
        if (!HasSHRookMoved) {
            count++;
            Moves[count].x = 6;Moves[count].y = backrank;
        }
    }

}

int BishopMoves(bool which_side, position * cur_pos, int BishopPos[2], move * Moves) {

    int control = 0;
    int Increment[2] = {1,1}; control = GetAttack(BishopPos, cur_pos, Increment, Moves, control);
    Increment[0] = -1; control = GetAttack(BishopPos, cur_pos, Increment, Moves, control);
    Increment[1] = -1; control = GetAttack(BishopPos, cur_pos, Increment, Moves, control);
    Increment[0] = 1; control = GetAttack(BishopPos, cur_pos, Increment, Moves, control);

    return control;

}

int RookMoves(bool which_side, position * cur_pos, int RookPos[2], move * Moves) {

    int control = 0;
    int Increment[2] = {1,0}; control = GetAttack(RookPos, cur_pos, Increment, Moves, control);
    Increment[0] = -1; control = GetAttack(RookPos, cur_pos, Increment, Moves, control);
    Increment[0] = 0;
    Increment[1] = 1; control = GetAttack(RookPos, cur_pos, Increment, Moves, control);
    Increment[1] = -1; control = GetAttack(RookPos, cur_pos, Increment, Moves, control);

    return control;

}

void QueenMoves(bool which_side, position * cur_pos, int QueenPos[2], move * Moves) {

    int control = RookMoves(which_side, cur_pos, QueenPos, Moves);
    BishopMoves(which_side, cur_pos, QueenPos, Moves+control);

}