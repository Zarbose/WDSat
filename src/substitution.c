#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

#include "wdsat_utils.h"
#include "substitution.h"
#include "dimacs.h"
#include "cycle.h"

// utils variables
static uint_t substitution_nb_of_var;
static int_t substitution_nb_unary_vars;

// undo structures
int_t substitution_history[__ID_SIZE__];
int_t substitution_history_top;
int_t substitution_step[__ID_SIZE__];
int_t substitution_step_top;
static int_t substitution_history_top_it;

int_t *substitution_history_inte_stack;
int_t substitution_history_inte_top;
int_t substitution_history_main_stack[__ID_SIZE__];
int_t substitution_history_main_top;

int_t *substitution_history_inte_index_stack;
int_t *substitution_history_inte_index_stack_buffer;
int_t substitution_history_tag;

// stack structures
int_t substitution_up_stack[__ID_SIZE__];
int_t substitution_up_top_stack;
int_t *substitution_index_stack;
int_t substitution_tag;

// assignment structures
boolean_t substitution_assignment_buffer[__SIGNED_ID_SIZE__];
boolean_t *substitution_assignment;

// cnf module equivalence structures
int_t substitution_equivalency_all[__ID_SIZE__][__MAX_DEGREE__-1];
bool substitution_equivalent[__ID_SIZE__];

int_t substitution_equivalency_unary[__MAX_ANF_ID__][__MAX_ID__];
int_t substitution_equivalent_index_unary[__MAX_ANF_ID__];


// "main" structures
static int_t **substitution_values;
static int_t **substitution_values_buffer;

// index tables
static int_t *substitution_index;
static int_t *substitution_index_buffer;

// Regular print
void substitution_fprint_equivalency_all() {
	uint_t v;
	for(v = 0LL; v <= substitution_nb_of_var; ++v) {
        if (substitution_equivalent[v]){
            printf(" %ld <=> ",v);
            for (int_t j=0LL; j< __SZ_SUB__; j++) printf("%ld ",substitution_equivalency_all[v][j]);
            printf("\n");
        }

	}
}

void substitution_fprint_equivalency_unary(){
    for(int_t i = 1; i <= substitution_nb_unary_vars; ++i) {
        printf("[%ld]",i);
        for (int j = 0; j<substitution_equivalent_index_unary[i]; j+=2){
            printf(" (%ld %ld)",substitution_equivalency_unary[i][j],substitution_equivalency_unary[i][j+1]);
        }
        printf("\n");
	}
}

void substitution_fprint_assignment(){
    const int_t _v = substitution_nb_of_var;
    printf("Printing substitution_assignment \n");
    for (int_t i = 1LL; i <= _v; ++i){
        printf("%ld  %ld: %d %d\n",-i,i,substitution_assignment[-i],substitution_assignment[i]);
    }
}

void substitution_fprint_inline_assignment(){
    const int_t _v = substitution_nb_of_var;
    for (int_t i = 1LL; i <= _v; ++i){
        printf("%d",substitution_assignment[-i]);
    }
    printf("\n");
    for (int_t i = 1LL; i <= _v; ++i){
        printf("%d",substitution_assignment[i]);
    }
    printf("\n");
}

void substitution_fprint_unary_var_assignment(){
    const int_t _v = substitution_nb_unary_vars;
    for (int_t i = 1LL; i <= _v; ++i){
        printf("%d",substitution_assignment[i]);
    }
    printf("\n");
}

void substitution_fprint_history_inte_index_stack(){
    const int_t _n_v = substitution_nb_of_var;
    for(int_t v = -_n_v; v <= _n_v; ++v) {
        if (!v) continue;
        printf("%ld\n",substitution_history_inte_index_stack[v]);
    }
}

void substitution_fprint_history_inte_stack(){
    printf("Printing substitution_history_inte_index_stack \n");
    for (int_t i=0; i < substitution_history_inte_top; i+=2){
        if (substitution_history_inte_stack[i] >= 10)
            printf("%ld %ld\n",substitution_history_inte_stack[i],substitution_history_inte_stack[i+1]);
        else
            printf("%ld %ld\n",substitution_history_inte_stack[i],substitution_history_inte_stack[i+1]);
    }
}

void substitution_fprint_history_main_stack(){
    printf("Printing substitution_history_main_stack \n");
    for (int i = 0; i < substitution_history_main_top; i++)
        printf("%ld \n",substitution_history_main_stack[i]);
}

void substitution_fprint_substitution_up_stack(){
    printf("Printing substitution_up_stack \n");
    for (int_t i = 0; i < substitution_up_top_stack; i++){
        printf("%ld ",substitution_up_stack[i]);
    }
    printf("\n");
}

void substitution_fprint_substitution_index_stack(){
    printf("Printing substitution_index_stack \n");
    const int_t _n_v = substitution_nb_of_var;
    for(int_t v = -_n_v; v <= _n_v; ++v) {
        if (!v) continue;
        printf("%ld ",substitution_index_stack[v]);
    }
    printf("\n");
}

void substitution_fprint_substitution_index(){
    printf("Printing substitution_index \n");
    const int_t _n_v = substitution_nb_of_var;
    for(int_t v = -_n_v; v <= _n_v; ++v) {
        if (!v) continue;
        printf("%ld ",substitution_index[v]);
    }
    printf("\n");
}

void substitution_fprint_values(){
    const int_t _n_v = substitution_nb_of_var;
    printf("Printing substitution_values \n");
    for(int_t v = -_n_v; v <= _n_v; ++v) {
        if (!v) continue;
        if (substitution_index[v] == 0) continue;
        printf("[%ld] ",v);
        for (int_t j=0LL; j < substitution_index[v] ; j++) printf("%ld ",substitution_values[v][j]);
        printf("\n");
    }
}

// Utils functions
bool substitution_is_unary_var(const int_t _l){
    int_t abs_val = (_l < 0) ? -_l : _l;
    return abs_val <= substitution_nb_unary_vars;
}

void substitution_reset_stack(){
    ++substitution_tag;
    substitution_up_top_stack=0;
}

// Add to stack functions
void substitution_add_check_stack(int_t v){ // Ajout d'un élément dans la pile de propagation en testant s'il n'y est déjà pas présent
    if ( !(substitution_index_stack[v] == substitution_tag) ){
        substitution_up_stack[substitution_up_top_stack++]=v;
        substitution_index_stack[v]=substitution_tag;
    }
}

void substitution_add_check_history_stack(int_t v){ // Ajout d'un coulpe d'éléments dans l'une des piles de backtrack en testant s'il n'y est déjà pas présent
    if ( !(substitution_history_inte_index_stack[v] == substitution_history_tag) ){
        substitution_history_inte_stack[substitution_history_inte_top++]=substitution_index[v];
        substitution_history_inte_stack[substitution_history_inte_top++]=v;
        substitution_history_inte_index_stack[v]=substitution_history_tag;
    }
}

// Update "main" struture functions
void substitution_update_dynamic_part(const int_t _l){ // Mise à jour du tableau substitution_values en utilisant les règles dynamiques
    const uint_t _uv = (uint_t) ((_l < 0) ? -_l : _l);
    for (int j = 0; j<substitution_equivalent_index_unary[_uv]; j+=2){
        int_t _x = substitution_equivalency_unary[_uv][j];
        int_t _y = substitution_equivalency_unary[_uv][j+1];

        substitution_add_check_history_stack(_y);
        substitution_add_check_history_stack(-_y);
        substitution_add_check_history_stack(_x);
        substitution_add_check_history_stack(-_x);

        substitution_values[_x][substitution_index[_x]++]=_y;

        substitution_values[_y][substitution_index[_y]++]=_x;

        substitution_values[-_y][substitution_index[-_y]++]=-_x;

        substitution_values[-_x][substitution_index[-_x]++]=-_y;

    }
}

bool substitution_update_tables(const int_t l){ // Mise à jour du tableau substitution_values
    const bool _tf = (l < 0) ? false : true;
    if (substitution_is_unary_var(l)){
        if (_tf) substitution_update_dynamic_part(l);
    }
    else{
        if (!_tf){ /* La régle x26 = false */
            int_t _v = (l < 0) ? -l : l;
            if (substitution_equivalent[_v] == true){
                int_t x1 = substitution_equivalency_all[_v][0];
                int_t x2 = substitution_equivalency_all[_v][1];

                if (!_substitution_is_undef(x1) && !_substitution_is_undef(x2)){
                    /* x1 and x2 def */
                    if ( _substitution_is_true(x1) && _substitution_is_true(x2)){
                        return false;
                    }
                }
                else if (_substitution_is_undef(x1) && !_substitution_is_undef(x2)){
                    /* x1 undef */
                    if (_substitution_is_true(x2)){ // Warning : duplicated values inside the structure
                        substitution_add_check_stack(-x1);
                    }
                }
                else if (!_substitution_is_undef(x1) && _substitution_is_undef(x2)){
                    /* x2 undef */
                    if (_substitution_is_true(x1)){ // Warning : duplicated values inside the structure
                        substitution_add_check_stack(-x2);
                    }
                }
                else{
                    /* x1 and x2 undef */
                }
            }
        }
    }
    return true;
}

// Init functions
void substitution_init_static_part(){ // Initialisation du tableau substitution_values en utilisant les règles statiques
    for(int_t i = 1; i <= substitution_nb_unary_vars; ++i) {
        for (int j = 0; j<substitution_equivalent_index_unary[i]; j+=2){
            int_t _x1 = i;
            int_t _x2 = substitution_equivalency_unary[i][j];
            int_t _y = substitution_equivalency_unary[i][j+1];

            if (_x2 < i)
                continue;

            // Rules with y is not an unary var and y = true
            substitution_values[_y][substitution_index[_y]++]=_x1;
            substitution_values[_y][substitution_index[_y]++]=_x2;

            // Rules with x1 is an unary var and x1 = false
            substitution_values[-_x1][substitution_index[-_x1]++]=-_y;

            // Rules with x2 is an unary var and x2 = false
            substitution_values[-_x2][substitution_index[-_x2]++]=-_y;
        }
	}
}

bool substitution_initiate_from_dimacs() {
    const int_t _n_e = dimacs_nb_equations();
    const int_t _n_v = dimacs_nb_vars();
    const int_t _n_uv = dimacs_nb_unary_vars();
	substitution_nb_of_var = _n_v;
    substitution_nb_unary_vars = _n_uv;

    // init index for substitution_equivalency_all
    for(int_t i = 0LL; i <= _n_v; ++i) {
        _clear_mem(substitution_equivalency_all[i],__SZ_SUB__);
		substitution_equivalent[i] = false;
	}

    // init substitution_equivalency_all
    for(int_t i = 0LL; i < _n_e; ++i) {
        int_t _s_e = dimacs_size_of_equation(i);
        int_t * equation_buffer = dimacs_equation(i);
        if (_s_e == 3){
            int_t j=equation_buffer[2LL];
            substitution_equivalent[j]=true;
            substitution_equivalency_all[j][0LL] = -equation_buffer[0LL];
            substitution_equivalency_all[j][1LL] = -equation_buffer[1LL];
        }
	}

    // init substitution_equivalency_unary
    for(int_t i = 0LL; i <= _n_uv; ++i) {
		substitution_equivalent_index_unary[i] = 0;
	}

    for(int_t i = 0LL; i < _n_e; ++i) {
        int_t _s_e = dimacs_size_of_equation(i);
        int_t * equation_buffer = dimacs_equation(i);
        if (_s_e == 3){
            int_t y = equation_buffer[2LL];
            int_t x1 = -equation_buffer[0LL];
            int_t x2 = -equation_buffer[1LL];
            substitution_equivalency_unary[x1][substitution_equivalent_index_unary[x1]++]=x2;
            substitution_equivalency_unary[x1][substitution_equivalent_index_unary[x1]++]=y;

            substitution_equivalency_unary[x2][substitution_equivalent_index_unary[x2]++]=x1;
            substitution_equivalency_unary[x2][substitution_equivalent_index_unary[x2]++]=y;
        }
	}

    // init stack
    substitution_up_top_stack = substitution_tag = 0LL;
    substitution_tag++;
    substitution_index_stack = (int_t *)malloc((__SIGNED_ID_SIZE__)*sizeof(int_t));
    substitution_index_stack =  substitution_index_stack + _n_v;

    // init assignment tables
    substitution_assignment = substitution_assignment_buffer + _n_v + 1LL;
    substitution_assignment[0LL] = __UNDEF__;
    for (int_t i = 1LL; i <= _n_v; ++i)
        _substitution_unset(i);

    // init index tables
    substitution_index_buffer = (int_t *)malloc((__SIGNED_ID_SIZE__)*sizeof(int_t));
    substitution_index =  substitution_index_buffer + _n_v;

    // init "main" structures
    substitution_values_buffer = (int_t **)malloc((__SIGNED_ID_SIZE__) * sizeof(int_t *));
    substitution_values = substitution_values_buffer + _n_v;

    // init undo structures
    substitution_step_top = substitution_history_top = substitution_history_top_it = substitution_history_tag =0LL;
    substitution_history_tag++;

    substitution_history_main_top = substitution_history_inte_top = 0LL;

    substitution_history_inte_stack = (int_t *)malloc((__SZ_STACK__)*sizeof(int_t));
    _clear_mem(substitution_history_inte_stack,__SIGNED_ID_SIZE__);

    substitution_history_inte_index_stack_buffer = (int_t *)malloc((__SIGNED_ID_SIZE__)*sizeof(int_t));
    substitution_history_inte_index_stack = substitution_history_inte_index_stack_buffer +_n_v;


    // Filling in tables
    for(int_t i = -_n_v; i <= _n_v; ++i) {
        if(!i) {
            substitution_values[i] = NULL;
            continue;
        }
        int_t _sub_size = _n_v;
        substitution_values[i] = (int_t*)malloc(_sub_size * sizeof(int_t));
        _clear_mem(substitution_values[i],_sub_size);

        // Index
        substitution_index_stack[i] = substitution_index[i] = substitution_history_inte_index_stack[i] = 0;
    }
    substitution_init_static_part();
    return (true);
}

// "main" functions
bool substitution_set_true(const int_t l) {
    assert(abs((int) l) <= substitution_nb_of_var);
    substitution_add_check_stack(l);

    return(substitution_infer());
}

bool substitution_infer(){
    static int_t l;
    while(substitution_up_top_stack) {
        l = substitution_up_stack[--substitution_up_top_stack];
        if(!substitution_update_tables(l)){
            return false;
        }

        if (_substitution_is_true(l)) continue;
        else if (_substitution_is_false(l)){
            substitution_reset_stack();
            return false;
        }
        else{
            _substitution_set(l,__TRUE__)
            substitution_history[substitution_history_top++] = l;

            for (int_t i = 0; i != substitution_index[l]; ++i){
                const int_t _e = substitution_values[l][i];
                if(_substitution_is_false(_e)){
                    substitution_reset_stack();
                    return false;
                }
                else if(_substitution_is_undef(_e)){
                    substitution_add_check_stack(_e);
                }
            }
        }
    }
    return true;
}

// Undo functions
void substitution_undo() {
    // Disassigning literals
    static int_t _l;
    const int_t top_step = (substitution_step_top) ? substitution_step[--substitution_step_top] : 0;
    while(substitution_history_top != top_step) {
        _l = substitution_history[--substitution_history_top];
        _substitution_unset(_l);
    }
	substitution_history_top_it = substitution_history_top;

    // Backtrack of dynamic table and indexes
    const int_t top_step_main = (substitution_history_main_top > 0) ? substitution_history_main_stack[--substitution_history_main_top] : 0;
    while ( top_step_main != substitution_history_inte_top) {
        int_t l = substitution_history_inte_stack[--substitution_history_inte_top];
        int_t indx = substitution_history_inte_stack[--substitution_history_inte_top];
        substitution_index[l]=indx;
    }
    substitution_tag++;
    substitution_history_tag++;
}

int_t substitution_last_assigned(int_t *up_stack) {
	int_t up_stack_top = 0LL;
	int_t _l;
	while(substitution_history_top_it < substitution_history_top) {
		_l = substitution_history[substitution_history_top_it++];
		up_stack[up_stack_top++] = _l;
	}
	return up_stack_top;
}

// Free functions
void substitution_free_structure(){
    const int_t _n_v = substitution_nb_of_var;
    for(int_t i = -_n_v; i <= _n_v; ++i) {
        _free_mem(substitution_values[i]);
    }
    _free_mem(substitution_values_buffer);

    _free_mem(substitution_index_buffer);

    _free_mem(substitution_history_inte_stack);
    _free_mem(substitution_history_inte_index_stack_buffer);
}
