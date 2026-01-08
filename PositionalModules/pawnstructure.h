struct Node {
    int y;
    struct Node *next;
};

typedef struct Node PawnNode;

PawnNode * GetPawn(int PawnPos[2], PawnNode BaseNodes[8], PawnNode ReguNodes[8]) {
    int x = PawnPos[0]; int y = PawnPos[1];

    PawnNode * MainNode = &(BaseNodes[8]);
    while (MainNode->y != y) {MainNode = MainNode->next;}

    return MainNode;
}