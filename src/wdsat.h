//
//  wdsat.h
//  WDSat
//
//  Created by Monika Trimoska on 15/12/2018.
//  Copyright © 2018 Monika Trimoska. All rights reserved.
//

#ifndef wdsat_h
#define wdsat_h

#include <stdio.h>
#include <stdbool.h>

#include "wdsat_utils.h"

bool wdsat_infer(const int_t l, int_t conf[], int_t d);
bool wdsat_solve(int_t n, int_t l, int_t new_m, char *irr, char *X3, int_t xg, char mvc_graph[1000], char thread[1000],int S, char *filename);
bool wdsat_solve_rest(int_t l, int_t set_end, int_t conf[], int_t dec, FILE* flux);
bool wdsat_solve_rest_XG(int_t l, int_t set_end, int_t conf[], int_t d);

#endif
