#ifndef substitution_h
#define substitution_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "wdsat_utils.h"

// substitution
#define __SZ_SUB__ (__MAX_DEGREE__-1)
#define __SZ_STACK__ 8*(__MAX_ANF_ID__*__MAX_ANF_ID__)

// cnf module equivalence structures
extern int_t substitution_equivalency_all[__ID_SIZE__][__SZ_SUB__];
extern bool substitution_equivalent[__ID_SIZE__];

extern int_t substitution_equivalency_unary[__MAX_ANF_ID__][__MAX_ID__];
extern int_t substitution_equivalent_index_unary[__MAX_ANF_ID__];

// stack structures
extern int_t substitution_up_stack[__ID_SIZE__];
extern int_t substitution_up_top_stack;
extern int_t *substitution_index_stack;
extern int_t substitution_tag;

// assignment structures
extern boolean_t substitution_assignment_buffer[__SIGNED_ID_SIZE__];
extern boolean_t *substitution_assignment;

// undo structures
extern int_t substitution_history[__ID_SIZE__];
extern int_t substitution_history_top;
extern int_t substitution_step[__ID_SIZE__];
extern int_t substitution_step_top;

extern int_t *substitution_history_inte_stack;
extern int_t substitution_history_inte_top;

extern int_t substitution_history_main_stack[__ID_SIZE__];
extern int_t substitution_history_main_top;

extern int_t *substitution_history_inte_index_stack;
extern int_t substitution_history_tag;


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

/// @def _substitution_breakpoint
/// @brief set a breakpoint during resolution
#define _substitution_breakpoint \
{ \
    substitution_history_tag++; \
    substitution_tag++; \
    substitution_step[substitution_step_top++] = substitution_history_top; \
    substitution_history_main_stack[substitution_history_main_top++] = substitution_history_inte_top; \
}

/// @def _substitution_mergepoint
/// @brief merge last pushed context to previous one
#define _substitution_mergepoint \
{ \
    substitution_history_tag--; \
    substitution_tag--; \
    substitution_step_top && --substitution_step_top; \
    substitution_history_main_top && --substitution_history_main_top; \
}

// Print functions
void substitution_fprint_values(void);
void substitution_fprint_equivalency_all(void);
void substitution_fprint_equivalency_unary(void);
void substitution_fprint_assignment(void);
void substitution_fprint_history_inte_stack(void);
void substitution_fprint_history_main_stack(void);
void substitution_fprint_history_inte_index_stack(void);
void substitution_fprint_substitution_up_stack(void);
void substitution_fprint_substitution_index_stack(void);

// Utils functions
void substitution_add_check_stack(int_t v);
void substitution_reset_stack(void);
int_t substitution_last_assigned(int_t *up_stack);

// Init functions
bool substitution_initiate_from_dimacs(void);

// "main" functions
bool substitution_set_true(const int_t l);
bool substitution_infer(void);

// Undo functions
void substitution_undo(void);

// Free functions
void substitution_free_structure(void);
#endif
