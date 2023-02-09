#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "wdsat_utils.h"
#include "substitution.h"
#include "dimacs.h"

// utils variables
static uint_t substitution_nb_of_var; // Ici substitution_nb_of_var = 325
static uint_t substitution_nb_unary_vars; // Ici substitution_nb_unary_vars = 25

// undo structures
int_t substitution_history[__ID_SIZE__];
int_t substitution_history_top;
int_t substitution_step[__ID_SIZE__];
int_t substitution_step_top;
static int_t substitution_history_top_it;

int_t **substitution_history_values_dynamic;
int_t **substitution_history_values_dynamic_buffer;
int_t *substitution_history_index_dynamic;
int_t *substitution_history_index_dynamic_buffer;
int_t substitution_history_step_top;

// stack structures
int_t substitution_up_stack[__ID_SIZE__];
int_t substitution_up_top_stack;
int_t *substitution_index_stack;
int_t substitution_tag;

// assignment structures
boolean_t substitution_assignment_buffer[__SIGNED_ID_SIZE__];
boolean_t *substitution_assignment;

// cnf module equivalence structures
int_t substitution_equivalency[__ID_SIZE__][__SZ_SUB__];
bool substitution_equivalent[__ID_SIZE__]; // index to read faster substitution_equivalency


// "main" structures
static int_t **substitution_values_static;
static int_t **substitution_values_static_buffer;
static int_t **substitution_values_dynamic;
static int_t **substitution_values_dynamic_buffer;

// index tables
static int_t *substitution_index_static;
static int_t *substitution_static_index_buffer;

static int_t *substitution_index_dynamic;
static int_t *substitution_dynamic_index_buffer;


// Regular print
void substitution_fprint_equivalency() {
	uint_t v;
	for(v = 0LL; v <= substitution_nb_of_var; ++v) {
        if (substitution_equivalent[v]){
            printf(" %ld <=> ",v);
            for (int_t j=0LL; j< __SZ_SUB__; j++) printf("%ld ",substitution_equivalency[v][j]);
            printf("\n");
        }
        
	}
}

void substitution_fprint_static_values(){
    const int_t _n_v = substitution_nb_of_var;
    printf("Printing substitution_values_static \n");
    for(int_t v = -_n_v; v <= _n_v; ++v) {
        if (!v) continue;
        if (substitution_index_static[v] == 0) continue;
        printf("%ld - ",v);
        for (int_t j=0LL; substitution_index_static[v] != 0 ; j++) printf("%ld ",substitution_values_static[v][j]);
        printf("\n");
    }
}

void substitution_fprint_dynamic_values(){
    const int_t _n_v = substitution_nb_of_var;
    printf("Printing substitution_values_dynamic \n");
    for(int_t v = -_n_v; v <= _n_v; ++v) {
        if (!v) continue;
        if (substitution_index_dynamic[v] == 0) continue;
        printf("%ld : ",v);
        for (int_t j=0LL; j < substitution_index_dynamic[v]; j++) printf("%ld ",substitution_values_dynamic[v][j]);
        printf("\n");
    }
}

void substitution_fprint_assignment(){
    const int_t _v = substitution_nb_of_var;
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

// History print
void substitution_fprint_history_values_dynamic(){
    const int_t _n_v = substitution_nb_of_var;
    printf("Printing substitution_history_values_dynamic \n");
    for(int_t v = -_n_v; v <= _n_v; ++v) {
        if (!v) continue;
        if (substitution_history_values_dynamic[v][substitution_history_step_top] == 0) continue;
        printf("%ld : ",v);
        for (int_t j=0LL; j <= substitution_history_step_top; j++) printf("%ld ",substitution_history_values_dynamic[v][j]);
        printf("\n");
    }
}

void substitution_fprint_history_index_dynamic(){
    const int_t _n_v = substitution_nb_of_var;
    printf("Printing substitution_history_index_dynamic \n");
    for(int_t v = -_n_v; v <= _n_v; ++v) {
        if (!v) continue;
        if(substitution_history_index_dynamic[v] == 0) continue;
        printf("%ld : %ld \n",v,substitution_history_index_dynamic[v]);
    }
}

// Utils functions
void substitution_reset_stack(){
    ++substitution_tag;
    substitution_up_top_stack=0;
}

void substitution_add_check_stack(int_t v){
    if ( !(substitution_index_stack[v] == substitution_tag) ){
        // printf("Oui %d \n",v);
        substitution_up_stack[substitution_up_top_stack++]=v;
        substitution_index_stack[v]=substitution_tag;
    }
    // else
    //     printf("Non %d \n",v);
}

inline void substitution_reset_boolean_vector(int_t *v, uint_t sz) {
	for(uint_t i = 0ULL; i < sz; ++i){
		/// different writing for same aim
		v[i] = 0ULL;
		// v[i] ^= v[i];
    }
}

void substitution_free_structure(){
    const int_t _n_v = substitution_nb_of_var;
    for(int_t i = -_n_v; i <= _n_v; ++i) {
        if(!i) {
            free(substitution_values_static[i]);
            free(substitution_values_dynamic[i]);
            free(substitution_history_values_dynamic[i]);
            continue;
        }
        free(substitution_values_static[i]);
        free(substitution_values_dynamic[i]);
        free(substitution_history_values_dynamic[i]);
    }
    free(substitution_values_static_buffer);
    free(substitution_values_dynamic_buffer);

    free(substitution_dynamic_index_buffer);
    free(substitution_static_index_buffer);

    free(substitution_history_index_dynamic_buffer);
    free(substitution_history_values_dynamic_buffer);
}

void substitution_reset_dynamic_table(){
    const int_t _n_v = substitution_nb_of_var;
    for(int_t i = -_n_v; i <= _n_v; ++i) {
        if(!i) {
            continue;
        }
        int_t _sub_size = _n_v;
        CLEAR(substitution_values_dynamic[i],_sub_size);
        substitution_index_dynamic[i]=0;
    }
}

void substitution_init_static_table(){
	uint_t v;
	for(v = 1LL; v <= substitution_nb_of_var; ++v) {
        if (substitution_equivalent[v]){
            uint_t _y = v;
            uint_t _x1 = substitution_equivalency[v][0];
            uint_t _x2 = substitution_equivalency[v][1];

            // Rules with y is not an unary var and y = true
            substitution_values_static[_y][substitution_index_static[_y]++]=_x1;
            substitution_values_static[_y][substitution_index_static[_y]++]=_x2;

            // Rules with x1 is an unary var and x1 = false
            substitution_values_static[-_x1][substitution_index_static[-_x1]++]=-_y;

            // Rules with x2 is an unary var and x2 = false
            substitution_values_static[-_x2][substitution_index_static[-_x2]++]=-_y;
        }
        
	}
}

bool substitution_is_unary_var(const int_t _l){
    int_t abs_val = (_l < 0) ? -_l : _l;
    if( abs_val > substitution_nb_unary_vars)
        return false;

    return true;
}

void substitution_update_dynamic_values(const int_t _l){
    const uint_t _uv = (uint_t) ((_l < 0) ? -_l : _l);
    for(int_t w = 0LL; w < substitution_nb_of_var; ++w) { // On cherche y <=> x1 x2
        if (substitution_equivalent[w]){ 
            int_t _y = w;
            for (int_t j=0LL; j< __SZ_SUB__; j++) { // x1 ? x2 ?
                if (substitution_equivalency[_y][j] == _uv){
                    int_t _x;

                    // Ici _x == x2
                    if ( j == 0LL ){
                        _x = substitution_equivalency[_y][1];
                    } else {
                        _x = substitution_equivalency[_y][0];
                    }

                    substitution_values_dynamic[_y][substitution_index_dynamic[_y]++]=_x;
                    substitution_history_index_dynamic[_y]++;

                    substitution_values_dynamic[_x][substitution_index_dynamic[_x]++]=_y;
                    substitution_history_index_dynamic[_x]++;

                    substitution_values_dynamic[-_y][substitution_index_dynamic[-_y]++]=-_x;
                    substitution_history_index_dynamic[-_y]++;

                    substitution_values_dynamic[-_x][substitution_index_dynamic[-_x]++]=-_y;
                    substitution_history_index_dynamic[-_x]++;
                }
            }
        }
    }
    substitution_history_step_top++;
    substitution_history_save_index();
}

// Init functions
bool substitution_initiate_from_dimacs() {
    const int_t _n_e = dimacs_nb_equations(); // Ici _n_e = 900
    const int_t _n_v = dimacs_nb_vars(); // Ici _n_v = 325
    const int_t _n_uv = dimacs_nb_unary_vars();
	substitution_nb_of_var = _n_v;
    substitution_nb_unary_vars = _n_uv;

    // init index for substitution_equivalency
    for(int_t i = 0LL; i <= _n_v; ++i) {
        substitution_reset_boolean_vector(substitution_equivalency[i],__SZ_SUB__);
		substitution_equivalent[i] = false;
	}

    // init substitution_equivalency
    for(int_t i = 0LL; i < _n_e; ++i) {
        int_t _s_e = dimacs_size_of_equation(i);
        int_t * equation_buffer = dimacs_equation(i);
        if (_s_e == 3){
            int_t j=equation_buffer[2LL];
            substitution_equivalent[j]=true;
            substitution_equivalency[j][0LL] = -equation_buffer[0LL];
            substitution_equivalency[j][1LL] = -equation_buffer[1LL];
        }
	}

    // init stack
    substitution_up_top_stack = substitution_tag = 0LL;
    substitution_tag++;    
    substitution_index_stack = (int_t *)malloc((__SIGNED_ID_SIZE__)*sizeof(int_t));
    substitution_index_stack =  substitution_index_stack + _n_v;

    // init tab assignment
    substitution_assignment = (boolean_t *)malloc(__SIGNED_ID_SIZE__*sizeof(boolean_t));
    substitution_assignment[0LL] = __UNDEF__;
    for (int_t i = 1LL; i <= _n_v; ++i)
        _substitution_unset(i);

    // init index tables
    substitution_static_index_buffer = (int_t *)malloc((__SIGNED_ID_SIZE__)*sizeof(int_t));
    substitution_index_static =  substitution_static_index_buffer + _n_v;

    substitution_dynamic_index_buffer = (int_t *)malloc((__SIGNED_ID_SIZE__)*sizeof(int_t));
    substitution_index_dynamic =  substitution_dynamic_index_buffer + _n_v;

    // init "main" structures
    substitution_values_static_buffer = (int_t **)malloc((__SIGNED_ID_SIZE__) * sizeof(int_t *));
    substitution_values_static = substitution_values_static_buffer +_n_v;

    substitution_values_dynamic_buffer = (int_t**)malloc((__SIGNED_ID_SIZE__) * sizeof(int_t *));
    substitution_values_dynamic = substitution_values_dynamic_buffer +_n_v;

    // init undo 
    substitution_step_top = substitution_history_top = substitution_history_top_it = 0LL;

    substitution_history_values_dynamic_buffer = (int_t **)malloc((__SIGNED_ID_SIZE__)*sizeof(int_t));
    substitution_history_values_dynamic =  substitution_history_values_dynamic_buffer + _n_v;

    substitution_history_index_dynamic_buffer = (int_t *)malloc((__SIGNED_ID_SIZE__)*sizeof(int_t));
    substitution_history_index_dynamic =  substitution_history_index_dynamic_buffer + _n_v;

    substitution_history_step_top=0LL;

    // Filling in tables
    for(int_t i = -_n_v; i <= _n_v; ++i) {
        if(!i) {
            substitution_values_static[i] = NULL;
            substitution_values_dynamic[i] = NULL;

            // Undo
            substitution_history_values_dynamic[i] = NULL;
            continue;
        }
        int_t _sub_size = _n_v;
        substitution_values_static[i] = (int_t*)malloc(_sub_size * sizeof(int_t));
        substitution_values_dynamic[i] = (int_t*)malloc(_sub_size * sizeof(int_t));
        CLEAR(substitution_values_static[i],_sub_size);
        CLEAR(substitution_values_dynamic[i],_sub_size);
        
        // Undo
        substitution_history_values_dynamic[i] = (int_t*)malloc(_sub_size * sizeof(int_t));
        CLEAR(substitution_history_values_dynamic[i],_sub_size);

        // Index
        substitution_index_stack[i] = substitution_index_static[i] = substitution_index_dynamic[i] = substitution_history_index_dynamic[i] = 0;
    }

    substitution_init_static_table();
    // substitution_fprint_static_values();
    return (true);
}

// "main" functions
bool substitution_set_true(const int_t l) {
    assert(abs((int) l) <= substitution_nb_of_var);
    // substitution_fprint_static_values();

    // substitution_reset_stack();
    // substitution_reset_dynamic_table();
	
    substitution_up_stack[substitution_up_top_stack++] = l;

    const bool _tf = (l < 0) ? false : true;
    if (substitution_is_unary_var(l)){
        if (_tf){
            substitution_update_dynamic_values(l);
        }
    }
    else{
        
        if (!_tf){ /* La régle x26 = false */ }
    }

    return(substitution_subt());
}

bool substitution_subt(){
    static int_t l;
    while(substitution_up_top_stack) {
        l = substitution_up_stack[--substitution_up_top_stack];
        if (_substitution_is_true(l)) continue;
        else if (_substitution_is_false(l)){
            // printf("Return false : %ld is set to false\n",l);
            substitution_reset_stack();
            return false;
        }
        else{
            _substitution_set(l,__TRUE__)
            substitution_history[substitution_history_top++] = l;

            // Static
            for (int_t i = 0; substitution_values_static[l][i] != 0; ++i){
                const int_t _e = substitution_values_static[l][i];
                if(_substitution_is_false(_e)){
                    // printf("Return false : %ld is set to false\n",l);
                    substitution_reset_stack();
                    return false;
                }
                else if(_substitution_is_undef(_e)){
                    substitution_add_check_stack(_e);
                }

            }

            // Dynamic
            for (int_t i = 0; substitution_values_dynamic[l][i] != 0; ++i){
                const int_t _e = substitution_values_dynamic[l][i];
                if(_substitution_is_false(_e)){
                    // printf("%d %d\n",substitution_assignment[-_e],substitution_assignment[_e]);
                    // printf("Return false : %ld is set to false\n",l);
                    substitution_reset_stack();
                    return false;
                }
                else if(_substitution_is_undef(_e)){
                    substitution_add_check_stack(_e);
                }

            }

            for (int_t i = 0; substitution_values_dynamic[-l][i] != 0; ++i){
                const int_t _e = substitution_values_dynamic[-l][i];
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
void substitution_history_save_index(){
    const int_t _n_v = substitution_nb_of_var;
    for(int_t v = -_n_v; v <= _n_v; ++v) {
        if(!v) continue;
        substitution_history_values_dynamic[v][substitution_history_step_top]=substitution_history_index_dynamic[v];
    }
}

void substitution_undo() {
    static int_t _l;
    const int_t top_step = (substitution_step_top) ? substitution_step[--substitution_step_top] : 0;
    while(substitution_history_top != top_step) {
        _l = substitution_history[--substitution_history_top];
        _substitution_unset(_l);
    }
	substitution_history_top_it = substitution_history_top;

    substitution_history_step_top--;
    const int_t _n_v = substitution_nb_of_var;
    for(int_t v = -_n_v; v <= _n_v; ++v) {
        if(!v) continue;
        substitution_history_index_dynamic[v]=substitution_history_values_dynamic[v][substitution_history_step_top];
    }
    memcpy(substitution_dynamic_index_buffer, substitution_history_index_dynamic_buffer, sizeof(int_t)*__SIGNED_ID_SIZE__);

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
