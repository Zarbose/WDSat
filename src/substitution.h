#ifndef substitution_h
#define substitution_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "wdsat_utils.h"

// substitution
#define __SZ_SUB__ (3 - 1)
#define __SZ_VAR__ (900)

extern uint_t substitution_equivalency[__SZ_VAR__][__SZ_SUB__];
extern bool substitution_equivalent[__SZ_VAR__];

extern uint_t substitution_values[__SZ_VAR__][__MAX_ID__];
extern bool substitution_index[__SZ_VAR__];

void substitution_fprint_values(void);
void substitution_fprint_equivalency(void);

void substitution_reset_boolean_vector(uint_t *, uint_t sz);

bool substitution_subt(const int_t v);
bool substitution_replace(const int_t v_bin, const int_t v_mon);

bool substitution_initiate_from_dimacs(void);

#endif