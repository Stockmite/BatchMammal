#include <stdio.h>
#include <stdlib.h>

#include "pieces.h"

float sum_material(int S_side[8][8], char Piecepositions[8][8]) {
    float sum = 0;

    for (int x = 0; x < 8; x++) {
        for (int y = 0; y < 8; y++) {
            if (S_side[x][y]) {
                char piece = Piecepositions[x][y];

                switch (piece) {
                    case 'p':
                        sum++;
                        break;
                    case 'K':
                        sum += 200.0;
                        break;
                    case 'Q':
                        sum += 9.0;
                        break;
                    case 'N':
                        sum += 3.25;
                        break;
                    case 'B':
                        sum += 3.5;
                        break;
                    case 'R':
                        sum += 5;
                        break;
                    default:
                        break;
                }
            }
        }
    }

    return sum;

}

float Evaluate(position * cur_pos) {
    float wvalue, bvalue = 0;


}