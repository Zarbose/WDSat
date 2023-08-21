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
// no_unary_var <=> unary_var
extern int_t substitution_equivalency_all[__ID_SIZE__][__SZ_SUB__]; // Tableau qui contient des équivalences du type no_unary_var <=> unary_var
extern bool substitution_equivalent[__ID_SIZE__]; // Tableau qui permet de savoir si une équivalence existe pour une variable donnée dans le tableau substitution_equivalency_all 

// unary_var <=> no_unary_var
extern int_t substitution_equivalency_unary[__MAX_ANF_ID__][__MAX_ID__]; // Tableau qui contient des équivalences du type unary_var <=> no_unary_var
extern int_t substitution_equivalent_index_unary[__MAX_ANF_ID__]; // Index qui permet de connaître la longueur d'une équivalence (ex : 1 <=> 26 27 28) dans le tableau substitution_equivalency_unary 

// stack structures
extern int_t substitution_up_stack[__ID_SIZE__]; // Pile de propagation du module
extern int_t substitution_up_top_stack; // Haut de la pile de propagation du module
extern int_t *substitution_index_stack; // Index pour empêcher les doublons dans la pile de propagation du module
extern int_t substitution_tag; // Tag utilisé dans l'index qui empêche les doublons

// assignment structures
extern boolean_t substitution_assignment_buffer[__SIGNED_ID_SIZE__]; // Buffer du tableau d'assignement
extern boolean_t *substitution_assignment; // Pointeur sur le buffert du tableau d'assignement

// undo structures
// undo assignement
extern int_t substitution_history[__ID_SIZE__];
extern int_t substitution_history_top;
extern int_t substitution_step[__ID_SIZE__];
extern int_t substitution_step_top;

// undo substitution_values
extern int_t *substitution_history_inte_stack; // Pile qui contient des couple de valeur (index, colonne concernée)
extern int_t substitution_history_inte_top; // Haut de la pile substitution_history_inte_stack

extern int_t substitution_history_main_stack[__ID_SIZE__]; // Pile qui permet de savoir jusqu'ou dépiler substitution_history_inte_stack
extern int_t substitution_history_main_top;

extern int_t *substitution_history_inte_index_stack; // Index pour empêcher les doublons dans substitution_history_inte_stack
extern int_t substitution_history_tag; // Tag utilisé dans substitution_history_inte_index_stack pour empêcher les doublons


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
