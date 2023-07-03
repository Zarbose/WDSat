#ifndef gray_h
#define gray_h

#include "config.h"

#define col __MAX_ANF_ID__-1
#define raw pow(2,col)

extern int **gray_table;

void gray_initiate(void);
void gray_free_structure(void);
void gray_decimal_to_binary(int num, int **gray_table, int pos);
int * gray_get_row(int id);

#endif