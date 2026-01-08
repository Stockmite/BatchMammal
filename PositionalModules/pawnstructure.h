#include <stdio.h>
struct Node {
    int y;
    struct Node *next;
};

typedef struct Node PawnNode;

PawnNode * GetPawn(int PawnPos[2], PawnNode BaseNodes[8], PawnNode ReguNodes[8]) {
    int x = PawnPos[0]; int y = PawnPos[1];

    PawnNode * MainNode = &(BaseNodes[x]);
    while (MainNode->y != y) {MainNode = MainNode->next;}

    return MainNode;
}

PawnNode * GetLongPawn(int PawnPos[2], PawnNode BaseNodes[8], PawnNode ReguNodes[8]) {
    int x = PawnPos[0]; int y = PawnPos[1];

    PawnNode * MainNode = &(BaseNodes[x]);
    while (MainNode->next != NULL) {MainNode = MainNode->next;}

    return MainNode;
}

bool DoesFHavePawns(int x, PawnNode BaseNodes[8]) {
    return (BaseNodes[x].next != NULL);
}

void MovePawn(int OgPos[2], int NewPos[2], PawnNode BaseNodes[8], PawnNode ReguNodes[8]) {

    

}