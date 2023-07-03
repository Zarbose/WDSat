#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "gray.h"

int **gray_table;

void gray_fprint(int **gray_table){
    for (int i = 0; i < row; i++){
        for (int j = 0; j < col; j++){
            printf("%d",gray_table[i][j]);
        }
        printf("\n");
    }
}

void gray_decimal_to_binary(int num, int **gray_table, int pos) {
    int j = 0;
    for (int i = col - 1; i >= 0; i--) {
        int bit = (num >> i) & 1;
        gray_table[pos][j++]=bit;
    }
}

void gray_generate(int **gray_table){
    for (int i = 0; i < row; i++){
        int N = i;
        int N_2 = N << 1;
        int gray = (N ^ N_2) >> 1;
        gray_decimal_to_binary(gray,gray_table,i);
    }
}

void gray_initiate() {
    gray_table = malloc(sizeof(int*) * row);

    for (int i = 0; i < row; i++) {
        gray_table[i] = malloc(sizeof(int) * col);
    }

    gray_generate(gray_table);
    
}

int * gray_get_row(int id){
    return gray_table[id];
}


void gray_free_structure(){

    for (int i = 0; i < row; i++) {
        free(gray_table[i]);
    }
    free(gray_table);
}

