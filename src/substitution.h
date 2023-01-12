#ifndef substitution_h
#define substitution_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "wdsat_utils.h"

// substitution
#define __SZ_SUB__ (__MAX_DEGREE__ - 1)
#define __SZ_VAR__ (900)
#define __SIGNED_SZ_T__ ((__SZ_VAR__ + 1) << 1)

extern int_t substitution_equivalency[__SZ_VAR__][__SZ_SUB__];
extern bool substitution_equivalent[__SZ_VAR__];

extern int_t substitution_up_stack[__SZ_VAR__];
extern int_t substitution_up_top_stack;

extern boolean_t substitution_assignment_buffer[__SIGNED_SZ_T__];
extern boolean_t *substitution_assignment;


#define _substitution_set(_v, _tv) \
{ \
    substitution_assignment[_v] = (boolean_t) _tv; \
    substitution_assignment[-_v] = (boolean_t) _tv ^ (boolean_t) __TRUE__; \
}

#define _substitution_unset(_v) \
{ \
    substitution_assignment[_v] = substitution_assignment[-_v] = __UNDEF__; \
}

#define _substitution_is_true(_v) (substitution_assignment[_v] & (boolean_t) 1)
#define _substitution_is_false(_v) (!substitution_assignment[_v])
#define _substitution_is_undef(_v) (substitution_assignment[_v] & 2)


void substitution_fprint_values(void);
void substitution_fprint_equivalency(void);

void substitution_reset_boolean_vector(int_t *, uint_t sz);

bool substitution_subt(const int_t v);

bool substitution_initiate_from_dimacs(void);

#endif