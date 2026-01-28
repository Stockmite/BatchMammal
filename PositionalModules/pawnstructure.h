#include <stdio.h>
#include <math.h>
struct Node {
    int y;
    struct Node *next;
    struct Node *prev;
};

typedef struct Node PawnNode;

PawnNode * GetPawn(int PawnPos[2], PawnNode BaseNodes[8]) {
    int x = PawnPos[0]; int y = PawnPos[1];

    if (!((x > -1 && x<8) && (y > -1 && y<8))) {return NULL;}

    PawnNode * MainNode = &(BaseNodes[x]);
    while (MainNode->y != y && MainNode->next != NULL) {MainNode = MainNode->next;}

    return MainNode;
}

bool GetPawnB(int PawnPos[2], PawnNode BaseNodes[8]) {
    int x = PawnPos[0]; int y = PawnPos[1];

    if (!((x > -1 && x<8) && (y > -1 && y<8))) {return false;}

    PawnNode * MainNode = &(BaseNodes[x]);
    while (MainNode->y != y && MainNode->next != NULL) {MainNode = MainNode->next;}

    if (MainNode->y != y) {return false;}

    return true;
}

PawnNode * GetLongPawn(int x, PawnNode BaseNodes[8]) {

    PawnNode * MainNode = &(BaseNodes[x]);
    while (MainNode->next != NULL) {MainNode = MainNode->next;}

    return MainNode;
}

bool GetAPawn(int x, int s, PawnNode BaseNodes[8], int which_dire) {
    if (!((x > -1 && x<8) && (s > -1 && s<8))) {return false;}

    PawnNode * MainNode = &(BaseNodes[x]);

    bool FoundAPawn = false;

    while (!FoundAPawn) {
        MainNode = MainNode->next;
        if (MainNode == NULL) {return false;}
        FoundAPawn = (which_dire == 1) ? (MainNode->y < s) : (MainNode->y > s);
    }

    return true;
}

int GetFurthestPawn(int x, PawnNode BaseNodes[8], bool which_dire) {

    PawnNode * MainNode = &(BaseNodes[x]);
    PawnNode * Farther = &(BaseNodes[x]);

    while (MainNode->next != NULL) {
        MainNode = MainNode->next;
        int conv_y = (which_dire == 1) ? MainNode->y : (7 - MainNode->y);
        if (Farther->y < conv_y) {Farther = MainNode;}
    }

    return Farther->y;


}

bool DoesFHavePawns(int x, PawnNode BaseNodes[8]) {
    if (x < 0 || x > 7) {return false;}
    return (BaseNodes[x].next != NULL);
}

void MovePawn(int OgPos[2], int NewPos[2], PawnNode BaseNodes[8]) {
    int nx = NewPos[0]; int ny = NewPos[1]; int x = OgPos[0];

    PawnNode * CurPawn = GetPawn(OgPos, BaseNodes);
    if (nx == x) {CurPawn->y = ny; return;}

    PawnNode * LastPawn = CurPawn->prev;
    PawnNode * NewPartner = GetLongPawn(nx, BaseNodes);

    LastPawn->next = CurPawn->next;
    NewPartner->next = CurPawn;

    CurPawn->y = ny;
    CurPawn->prev = NewPartner;

}

int HowManyPawnsInF(int x, PawnNode BaseNodes[8]) {

    int amount = 0;
    PawnNode * N = &(BaseNodes[x]);

    while (N->next != NULL) {amount++; N = N->next;}
    return amount;

}

void DestroyPawn(int PawnPos[2], PawnNode BaseNodes[8]) {
    PawnNode * Pawn = GetPawn(PawnPos, BaseNodes);
    Pawn->y = -1;
    (Pawn->prev)->next = Pawn->next;
    (Pawn->next)->prev = Pawn->prev;

}