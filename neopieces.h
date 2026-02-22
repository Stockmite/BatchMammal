
#include <stdlib.h>

typedef struct {
    unsigned int NewPos;
    unsigned int OldPos;
    char piece;
    char promotion;
} Move;

typedef Move * MoveList;