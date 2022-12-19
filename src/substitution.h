#ifndef substitution_h
#define substitution_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "wdsat_utils.h"

#define __SZ_SUB__ (__MAX_DEGREE__ - 1)

extern uint_t substitution_equivalency[__ID_SIZE__][__SZ_SUB__];
extern bool substitution_equivalent[__ID_SIZE__];

// substitution
bool substitution_initiate_from_dimacs(void);
void substitution_print_equivalency(void);
void substitution_fprint(void);





#endif