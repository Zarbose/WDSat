#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "wdsat_utils.h"
#include "substitution.h"
#include "dimacs.h"
#include "xorset.h"

// utils variables
static uint_t substitution_nb_of_var; // Ici substitution_nb_of_var = 325
static uint_t substitution_nb_unary_vars; // Ici substitution_nb_unary_vars = 25

// undo structures
int_t substitution_history[__ID_SIZE__];
int_t substitution_history_top;
int_t substitution_step[__ID_SIZE__];
int_t substitution_step_top;
static int_t substitution_history_top_it;

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

// index for fast reading values table
static bool *substitution_index_static;
static bool *substitution_static_index_buffer;
static bool *substitution_index_dynamic;
static bool *substitution_dynamic_index_buffer;


// Print functions
void substitution_fprint_equivalency() {
	uint_t v;
	for(v = 0LL; v <= substitution_nb_of_var; ++v) {
        if (substitution_equivalent[v]){
            printf(" %lld <=> ",v);
            for (int_t j=0LL; j< __SZ_SUB__; j++) printf("%lld ",substitution_equivalency[v][j]);
            printf("\n");
        }
        
	}
}

void substitution_fprint_static_values(){
    const int_t _n_v = substitution_nb_of_var;
    printf("Printing substitution_values_static \n");
    for(int_t v = -_n_v; v <= _n_v; ++v) {
        if (!v) continue;
        if (substitution_index_static[v] != true) continue;
        printf("%lld - ",v);
        for (int_t j=0LL; substitution_values_static[v][j] != 0 ; j++) printf("%lld ",substitution_values_static[v][j]);
        printf("\n");
    }
}

void substitution_fprint_dynamic_values(){
    const int_t _n_v = substitution_nb_of_var;
    printf("Printing substitution_values_dynamic \n");
    for(int_t v = -_n_v; v <= _n_v; ++v) {
        if (!v) continue;
        if (substitution_index_dynamic[v] != true) continue;
        printf("%lld - ",v);
        for (int_t j=0LL; substitution_values_dynamic[v][j] != 0 ; j++) printf("%lld ",substitution_values_dynamic[v][j]);
        printf("\n");
    }
}

void substitution_fprint_assignement(){
    const int_t _v = substitution_nb_of_var;
    for (int_t i = 1LL; i <= _v; ++i){
        printf("%ld  %ld: %d %d\n",-i,i,substitution_assignment[-i],substitution_assignment[i]);
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

int_t substitution_end_vector_static(const int_t v){
    int_t i;
    for(i = 0LL; substitution_values_static[v][i] != 0; i++);
    return i;
}

int_t substitution_end_vector_dynamic(const int_t v){
    int_t i;
    for(i = 0LL; substitution_values_dynamic[v][i] != 0; i++);
    return i;
}

void substitution_free_structure(){
    const int_t _n_v = substitution_nb_of_var;
    for(int_t i = -_n_v; i <= _n_v; ++i) {
        if(!i) {
            free(substitution_values_static[i]);
            free(substitution_values_dynamic[i]);
            continue;
        }
        free(substitution_values_static[i]);
        free(substitution_values_dynamic[i]);
    }
    free(substitution_values_static_buffer);
    free(substitution_values_dynamic_buffer);

    free(substitution_dynamic_index_buffer);
    free(substitution_static_index_buffer);
}

void substitution_reset_dynamic_table(){
    const int_t _n_v = substitution_nb_of_var;
    for(int_t i = -_n_v; i <= _n_v; ++i) {
        if(!i) {
            continue;
        }
        int_t _sub_size = _n_v;
        CLEAR(substitution_values_dynamic[i],_sub_size);
    }
}

void substitution_init_static_table(){
	uint_t v;
    int_t end_vector;
	for(v = 1LL; v <= substitution_nb_of_var; ++v) {
        if (substitution_equivalent[v]){
            uint_t _y = v;
            uint_t _x1 = substitution_equivalency[v][0];
            uint_t _x2 = substitution_equivalency[v][1];

            // Rules with y is not an unary var and y = true
            end_vector = substitution_end_vector_static(_y);
            substitution_values_static[_y][end_vector]=_x1;
            substitution_index_static[_y]=true;

            end_vector = substitution_end_vector_static(_y);
            substitution_values_static[_y][end_vector]=_x2;

            // Rules with x1 is an unary var and x1 = false
            end_vector = substitution_end_vector_static(-_x1);
            substitution_values_static[-_x1][end_vector]=-_y;
            substitution_index_static[-_x1]=true;

            // Rules with x2 is an unary var and x2 = false
            end_vector = substitution_end_vector_static(-_x2);
            substitution_values_static[-_x2][end_vector]=-_y;
            substitution_index_static[-_x2]=true;
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
                    int_t _x,_end_y,_end_x;

                    // Ici _x == x2
                    if ( j == 0LL ){
                        _x = substitution_equivalency[_y][1];
                    } else {
                        _x = substitution_equivalency[_y][0];
                    }

                    _end_y = substitution_end_vector_dynamic(_y);
                    substitution_values_dynamic[_y][_end_y]=_x;
                    substitution_index_dynamic[_y]=true;

                    _end_x = substitution_end_vector_dynamic(_x);
                    substitution_values_dynamic[_x][_end_x]=_y;
                    substitution_index_dynamic[_x]=true;

                    _end_y = substitution_end_vector_dynamic(-_y);
                    substitution_values_dynamic[-_y][_end_y]=-_x;
                    substitution_index_dynamic[-_y]=true;

                    _end_x = substitution_end_vector_dynamic(-_x);
                    substitution_values_dynamic[-_x][_end_x]=-_y;
                    substitution_index_dynamic[-_x]=true;
                }
            }
        }
    }
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
    substitution_up_top_stack=0LL;
    substitution_tag=0LL;
    substitution_tag++;    
    substitution_index_stack = (int_t *)malloc((__SIGNED_ID_SIZE__)*sizeof(int_t));
    substitution_index_stack =  substitution_index_stack + _n_v;
    
    for (int_t i = -_n_v; i <= _n_v; ++i){
        substitution_index_stack[i]=0;
    }

    // init tab assignment
    substitution_assignment = (boolean_t *)malloc(__SIGNED_ID_SIZE__*sizeof(boolean_t));
    substitution_assignment[0LL] = __UNDEF__;
    for (int_t i = 1LL; i <= _n_v; ++i)
        _substitution_unset(i);

    // init index for fast reading tab
    substitution_static_index_buffer = (bool *)malloc((__SIGNED_ID_SIZE__)*sizeof(bool));
    substitution_index_static =  substitution_static_index_buffer + _n_v;

    for (int_t i = -_n_v; i <= _n_v; ++i){
        substitution_index_static[i]=false;
    }

    substitution_dynamic_index_buffer = (bool *)malloc((__SIGNED_ID_SIZE__)*sizeof(bool));
    substitution_index_dynamic =  substitution_dynamic_index_buffer + _n_v;

    for (int_t i = -_n_v; i <= _n_v; ++i){
        substitution_index_dynamic[i]=false;
    }

    // init "main" structures
    substitution_values_static_buffer = (int_t **)malloc((__SIGNED_ID_SIZE__) * sizeof(int_t *));
    substitution_values_static = substitution_values_static_buffer +_n_v;

    substitution_values_dynamic_buffer = (int_t**)malloc((__SIGNED_ID_SIZE__) * sizeof(int_t *));
    substitution_values_dynamic = substitution_values_dynamic_buffer +_n_v;

    for(int_t i = -_n_v; i <= _n_v; ++i) {
        if(!i) {
            substitution_values_static[i] = NULL;
            substitution_values_dynamic[i] = NULL;
            continue;
        }
        int_t _sub_size = _n_v;
        substitution_values_static[i] = (int_t*)malloc(_sub_size * sizeof(int_t));
        substitution_values_dynamic[i] = (int_t*)malloc(_sub_size * sizeof(int_t));
        CLEAR(substitution_values_static[i],_sub_size);
        CLEAR(substitution_values_dynamic[i],_sub_size);
    }

    substitution_init_static_table();

    // init undo 
    substitution_step_top = substitution_history_top = substitution_history_top_it = 0LL;

    return (true);
}

// "main" functions
inline bool substitution_set_true(const int_t l) {
    assert(abs((int) l) <= substitution_nb_of_var);

    substitution_reset_stack();
    substitution_reset_dynamic_table();
	
    substitution_up_stack[substitution_up_top_stack++] = l;

    const bool _tf = (l < 0) ? false : true;
    if (substitution_is_unary_var(l)){
        if (_tf){
            // printf("Ici\n");
            substitution_update_dynamic_values(l);
            substitution_fprint_dynamic_values();
        }
    }
    else{
        // La régle x26 = false
    }

    return(substitution_subt());
}

bool substitution_subt(){

    static int_t l;
    while(substitution_up_top_stack) {
        l = substitution_up_stack[--substitution_up_top_stack];
        if (_substitution_is_true(l)) continue;
        else if (_substitution_is_false(l)){
            substitution_reset_stack();
            return false;
        }
        else{
            _substitution_set(l,__TRUE__)

            for (int_t i = 0; substitution_values_static[l][i] != 0; ++i){
                const int_t _e = substitution_values_static[l][i];
                if(_substitution_is_false(_e)){
                    substitution_reset_stack();
                    return false;
                }
                else if(_substitution_is_undef(_e)){
                    substitution_add_check_stack(_e);
                }

            }

            for (int_t i = 0; substitution_values_dynamic[l][i] != 0; ++i){
                const int_t _e = substitution_values_dynamic[l][i];
                if(_substitution_is_false(_e)){
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


/*
bool substitution_subt(){ // Le problème vient de la déclaration du tableau ou de la ligne d'affectation dans le tableau
    // const bool _tf = (v < 0) ? false : true;
    // const uint_t _uv = (uint_t) ((v < 0) ? -v : v);

    static int_t l;
    while(substitution_up_top_stack) {
        l = substitution_up_stack[--substitution_up_top_stack];    
        // printf("Propa : %lld\n",l);

        if (_substitution_is_true(l)) continue;
        else if (_substitution_is_false(l)){
            printf("Return false : %ld is set to false\n",l);
            substitution_reset_stack();
            return false;
        }
        else{
            // printf("Setting %lld to true and %lld to false\n",l,-l);
            _substitution_set(l,__TRUE__)
            substitution_history[substitution_history_top++] = l;

            const bool _tf = (l < 0) ? false : true;
            const uint_t _uv = (uint_t) ((l < 0) ? -l : l);

            if (_tf){ // Si x1 == true
                for(int_t w = 0LL; w < substitution_nb_of_var; ++w) { // On cherche y <=> x1 x2
                    if (substitution_equivalent[w]){ 
                        int_t _y = w;
                        for (int_t j=0LL; j< __SZ_SUB__; j++) { // x1 ? x2 ?
                            if (substitution_equivalency[_y][j] == _uv){
                                int_t _x1,_end_y,_end_x;

                                // Ici _x == x2
                                if ( j == 0LL ){
                                    _x1 = substitution_equivalency[_y][1];
                                    // _x2 = substitution_equivalency[_y][0];
                                } else {
                                    _x1 = substitution_equivalency[_y][0];
                                    // _x2 = substitution_equivalency[_y][1];
                                }



                                // x2 subst y
                                _end_y = substitution_end_vector(_y);
                                substitution_values[_y][_end_y]=_x1;
                                substitution_index[_y]=true;

                                _end_x = substitution_end_vector(_x1);
                                substitution_values[_x1][_end_x]=_y;
                                substitution_index[_x1]=true;

                                // _end_x = substitution_end_vector(_x2);
                                // substitution_values[_x2][_end_x]=_y;
                                // substitution_index[_x2]=true;


                                _end_y = substitution_end_vector(-_y);
                                substitution_values[-_y][_end_y]=-_x1;
                                substitution_index[-_y]=true;

                                _end_x = substitution_end_vector(-_x1);
                                substitution_values[-_x1][_end_x]=-_y;
                                substitution_index[-_x1]=true;

                                // _end_x = substitution_end_vector(-_x2);
                                // substitution_values[-_x2][_end_x]=-_y;
                                // substitution_index[-_x2]=true;
                                
                                // printf("add [%lld] = %lld    [%lld] = %lld \n",_y,_x,-_y,-_x);
                            }
                        }
                    }
                }
            }
            else{ // Si x1 == false
                for(int_t w = 0LL; w < substitution_nb_of_var; ++w) { // On cherche y <=> x1 x2
                    if (substitution_equivalent[w]){ 
                        int_t _y = w;
                        for (int_t j=0LL; j< __SZ_SUB__; j++) { // x1 ? x2 ?
                            if (substitution_equivalency[_y][j] == _uv){ // x1 = undef x2 = 1
                                if (_substitution_is_undef(-_y)){
                                    substitution_add_check_stack(-_y); // on ajoute y

                                    int_t _x1 = substitution_equivalency[_y][0]; // 1
                                    int_t _x2 = substitution_equivalency[_y][1]; // 2

                                    if(_substitution_is_undef(_x1)){
                                        if( _substitution_is_true(_x2))
                                            substitution_add_check_stack(-_x1);

                                        continue;
                                    }
                                    else if(_substitution_is_undef(_x2)){
                                        if( _substitution_is_true(_x1))
                                            substitution_add_check_stack(-_x2);

                                        continue;
                                    } else { // x1 et x2 sont deff 
                                        _x1 = (int_t) (_substitution_is_true(_x1) ? _x1 : -_x1);
                                        _x2 = (int_t) (_substitution_is_true(_x2) ? _x2 : -_x2);

                                        if ( !( (_substitution_is_false(_x1) && _substitution_is_false(_x2)) || (_substitution_is_true(_x1) && _substitution_is_false(-_x2)) ||  (_substitution_is_true(-_x1) && _substitution_is_false(_x2)) ) ){
                                            printf("For : %ld with x1 = %ld{%d,%d} x2 = %ld{%d,%d}\n",_y,_x1,substitution_assignment[_x1],substitution_assignment[-_x1],_x2,substitution_assignment[_x2],substitution_assignment[-_x2]);
                                            substitution_reset_stack();
                                            return false;
                                        }
                                    }

                                }
                            }
                        }
                    }
                }
            }
            // printf("Testing : %lld\n",l);
            int_t i =0;
            while (substitution_values[l][i] != 0){
                int_t _v = substitution_values[l][i];
                if (_substitution_is_undef(_v)){
                    substitution_add_check_stack(_v);
                }
                i++;
            }

        }
    }
    return true;
}*/

// Undo functions
void substitution_undo() {
    static int_t _l;
    const int_t top_step = (substitution_step_top) ? substitution_step[--substitution_step_top] : 0;
    while(substitution_history_top != top_step) {
        _l = substitution_history[--substitution_history_top];
        _substitution_unset(_l);
    }
	substitution_history_top_it = substitution_history_top;
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
