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
    bool IsBackrankAttacked; //easier than checking each time
    bool Which_side;
    int Pawns[8][2];
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

bool IsThereAPiece(Side Cur_side, Side Opp_side, int x, int y) {

    return Opp_side.Pieces[x][y] || Cur_side.Pieces[x][y];

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
            *control++;
        }

        return true;
    }

    return false;

}

void PawnMoves(Side Cur_side, Side Opp_side, int PawnPos[2], move * Moves, Pendulum MoveOrder) {
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
    if (y==PawnWall && !IsThereAPiece(Cur_side, Opp_side, x, new_y2)) {
        Moves[count].x = x; Moves[count].y = new_y2; count++;
    }

    if (poscap1) {
        NewPos[0] = x + 1; NewPos[1] = new_y;
        bool CanIt = CanItPromote(Cur_side, NewPos, Moves, &count);
        if (!CanIt) {
            Moves[count].x = x + 1; Moves[count].y = new_y; count++;
        }
    }
    if (poscap2) {
        NewPos[0] = x - 1; NewPos[1] = new_y;
        bool CanIt = CanItPromote(Cur_side, NewPos, Moves, &count);
        if (!CanIt) {
            Moves[count].x = x - 1; Moves[count].y = new_y; count++;
        }
    }
    
    int rec_x = MoveOrder.Last_move[0]; int rec_y = MoveOrder.Last_move[1];
    if (rec_y == y && Opp_side.PieceTypes == 'p') {
        Moves[count].x = rec_x; Moves[count].y = rec_y;
        count++;
    }

}

void KnightMoves(Side Cur_side, Side Opp_side, int KnightPos[2], move * Moves) {

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

}

void KingMoves(Side Cur_side, Side Opp_side, int KingPos[2], move * Moves) {

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

void QueenMoves(Side Cur_side, Side Opp_side, int QueenPos[2], move * Moves) {

    int control = RookMoves(Cur_side, Opp_side, QueenPos, Moves);
    BishopMoves(Cur_side, Opp_side, QueenPos, Moves+control);

}