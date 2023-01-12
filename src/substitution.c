#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "wdsat_utils.h"
#include "substitution.h"
#include "dimacs.h"
#include "xorset.h"

#define __SIGNED_SZ_T__ ((__SZ_VAR__ + 1) << 1)

int_t substitution_up_stack[__SZ_VAR__];
int_t substitution_up_top_stack;

boolean_t substitution_assignment_buffer[__SIGNED_SZ_T__];
boolean_t *substitution_assignment;


// Structure pour connaitre les équivalences
int_t substitution_equivalency[__SZ_VAR__][__SZ_SUB__];
bool substitution_equivalent[__SZ_VAR__];


// uint_t substitution_values[__SIGNED_SZ_T__][__MAX_ID__];
// bool substitution_index[__SIGNED_SZ_T__];

/*static int_t substitution_main_buffer[__SZ_VAR__];
static int_t substitution_main_val_buffer;

static int_t *substitution_values_buffer[__SIGNED_SZ_T__];
static int_t **substitution_values;*/


static int_t substitution_values[__SZ_VAR__][__SZ_VAR__];

static uint_t substitution_nb_of_equations;

// substitution
void substitution_fprint_equivalency() {
	uint_t v;
	for(v = 0LL; v < substitution_nb_of_equations; ++v) {
        if (substitution_equivalent[v]){
            printf(" %lld <=> ",v);
            for (int_t j=0LL; j< __SZ_SUB__; j++) printf("%lld ",substitution_equivalency[v][j]);
            printf("\n");
        }
        
	}
}

void substitution_fprint_values() {
    uint_t v;

    // for(v = 1LL; v < substitution_nb_of_equations; ++v) {
    //     printf("%lld : %s \n",v, substitution_index[v] ? "T" : "U");
    // }
    // printf("\n");

    for(v = 1LL; v < substitution_nb_of_equations; ++v) {
        printf("%lld - ",v);
        for (int_t j=0LL; substitution_values[v][j] != 0; j++) printf("%lld ",substitution_values[v][j]);
        printf("\n");
    }

}

inline void substitution_reset_boolean_vector(int_t *v, uint_t sz) {
	for(uint_t i = 0ULL; i < sz; ++i){
		/// different writing for same aim
		v[i] = 0ULL;
		// v[i] ^= v[i];
    }
}

int_t substitution_end_vector(const int_t v){
    int_t i;
    for(i = 0LL; substitution_values[v][i] != 0; i++);

    return i;
}

bool substitution_initiate_from_dimacs() {
    // const int_t _n_v = dimacs_nb_vars();
    const int_t _n_e = dimacs_nb_equations(); // Ici _n_e = 900
    // int_t sz = _n_e;
	substitution_nb_of_equations = _n_e;

    // Init tab qui contient les équivalence : y <=> x1 x2
    for(int_t i = 0LL; i <= _n_e; ++i) {
        substitution_reset_boolean_vector(substitution_equivalency[i],__SZ_SUB__);
		substitution_equivalent[i] = false;
	}

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

    substitution_up_top_stack=0LL;

    substitution_assignment = substitution_assignment_buffer + _n_e + 1LL; // substitution_assignment_buffer = 1802, _n_e = 900
    substitution_assignment[0LL] = __UNDEF__;

    // Init tab qui permet de retenir les valeurs utiles à la substitution
    /*substitution_main_val_buffer = 0LL;
    substitution_values = substitution_values_buffer + _n_e +1LL;
    for(int_t i = 1LL; i <= _n_e; ++i) {
        substitution_values[i] = substitution_values[-i] = NULL;
    }

    for(int_t i = 1LL; i <= _n_e; ++i) {
        substitution_reset_boolean_vector(substitution_main_buffer,__SZ_VAR__);
        substitution_values[i] = substitution_main_buffer;
        substitution_main_val_buffer+=sz;
    }
    // printf("%lld ",substitution_main_val_buffer);
    for(int_t i = 1LL; i <= _n_e; ++i) {
        substitution_reset_boolean_vector(substitution_main_buffer,__SZ_VAR__);
        substitution_values[-i] = substitution_main_buffer;
        substitution_main_val_buffer+=sz;
    }*/

    // Print de test
    // for (int_t j = 1LL; j < _n_e; ++j) {
    //     printf("%lld|%lld ",substitution_values[j],substitution_values[-j]);
    // }
    return (true);
}

void substitution_undo() {

}

bool substitution_subt(const int_t v){ // Le problème vient de la déclaration du tableau ou de la ligne d'affectation dans le tableau
    const bool _tf = (v < 0) ? false : true;
    // substitution_index[v]=true;

    if (_tf){ // Si x1 == true
        // x2 subst y, x1 = true

        for(int_t w = 0LL; w < substitution_nb_of_equations; ++w) { // y <=> x1 x2
            if (substitution_equivalent[w]){ 
                int_t _y = w;
                for (int_t j=0LL; j< __SZ_SUB__; j++) { // x1 ? x2 ?
                    if (substitution_equivalency[_y][j] == v){
                        int_t _x;
                        int_t _end_y = substitution_end_vector(_y);

                        if ( j == 0LL )
                            _x = substitution_equivalency[_y][1];
                        else
                            _x = substitution_equivalency[_y][0];
                        substitution_values[_y][_end_y]=_x;
                    }
                }
            }
	    }

    }
    else{
        // y = false, x1 = false
    }
    return true;
}
