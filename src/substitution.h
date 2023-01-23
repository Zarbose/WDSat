#ifndef substitution_h
#define substitution_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "wdsat_utils.h"

#define CLEAR(ptr, size) memset((ptr), 0, (size)*sizeof(*ptr))

// substitution
#define __SZ_SUB__ (__MAX_DEGREE__ - 1)

extern int_t substitution_equivalency[__ID_SIZE__][__SZ_SUB__];
extern bool substitution_equivalent[__ID_SIZE__];

extern int_t substitution_up_stack[__ID_SIZE__];
extern int_t substitution_up_top_stack;
extern int_t substitution_index_stack[__ID_SIZE__];
extern int_t substitution_tag;

extern boolean_t substitution_assignment_buffer[__SIGNED_ID_SIZE__];
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

bool substitution_subt();

bool substitution_initiate_from_dimacs(void);

void substitution_add_check_stack(int_t v);
void substitution_reset_stack(void);

#endif