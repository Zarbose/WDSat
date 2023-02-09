#ifndef substitution_h
#define substitution_h

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "wdsat_utils.h"

#define CLEAR(ptr, size) memset((ptr), 0, (size)*sizeof(*ptr))

// substitution
#define __SZ_SUB__ (2)

// cnf module equivalence structures
extern int_t substitution_equivalency[__ID_SIZE__][__SZ_SUB__];
extern bool substitution_equivalent[__ID_SIZE__]; // index to read faster substitution_equivalency


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

extern int_t **substitution_history_values_dynamic;
extern int_t *substitution_history_index_dynamic;
extern int_t substitution_history_step_top;


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
    substitution_step[substitution_step_top++] = substitution_history_top; \
    substitution_history_step_top++; \
}

/// @def _substitution_mergepoint
/// @brief merge last pushed context to previous one
#define _substitution_mergepoint \
{ \
    substitution_step_top && --substitution_step_top; \
    substitution_history_step_top && --substitution_history_step_top; \
}

// print functions
void substitution_fprint_values(void);
void substitution_fprint_equivalency(void);
void substitution_fprint_assignment(void);
void substitution_fprint_static_values(void);
void substitution_fprint_dynamic_values(void);
void substitution_fprint_history_values_dynamic(void);
void substitution_fprint_history_index_dynamic(void);

// utils functions
void substitution_reset_boolean_vector(int_t *, uint_t sz);
void substitution_add_check_stack(int_t v);
void substitution_reset_stack(void);
int_t substitution_last_assigned(int_t *up_stack);
void substitution_free_structure(void);
void substitution_reset_dynamic_table(void);

// init functions
bool substitution_initiate_from_dimacs(void);

// "main" functions
bool substitution_set_true(const int_t l);
bool substitution_subt(void);

// undo functions
void substitution_undo(void);
void substitution_history_save_index(void);

#endif
