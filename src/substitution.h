#ifndef substitution_h
#define substitution_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "wdsat_utils.h"

// substitution
#define __SZ_SUB__ (__MAX_DEGREE__ - 1)
#define __SZ_VAR__ (900)

extern uint_t substitution_equivalency[__SZ_VAR__][__SZ_SUB__];
extern bool substitution_equivalent[__SZ_VAR__];

extern uint_t substitution_values[__SZ_VAR__][__MAX_ID__];
extern bool substitution_index[__SZ_VAR__];

bool substitution_initiate_from_dimacs(void);
void substitution_fprint(void);
void substitution_reset_boolean_vector(uint_t *, uint_t sz);
#endif