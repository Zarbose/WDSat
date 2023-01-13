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


/*static int_t substitution_main_top_buffer;
static int_t substitution_main_buffer[__MAX_BUFFER_SIZE__];

static int_t *substitution_values_buffer[__SZ_SUB__];
static int_t **substitution_values;*/


static int_t substitution_values[__SZ_VAR__][__SZ_SUB__];
static bool substitution_index[__SZ_VAR__];

static uint_t substitution_nb_of_var;

// substitution
void substitution_fprint_equivalency() {
	uint_t v;
	for(v = 0LL; v < substitution_nb_of_var; ++v) {
        if (substitution_equivalent[v]){
            printf(" %lld <=> ",v);
            for (int_t j=0LL; j< __SZ_SUB__; j++) printf("%lld ",substitution_equivalency[v][j]);
            printf("\n");
        }
        
	}
}

void substitution_fprint_values() {
    printf("Printing substitution_values \n");
    for(int_t v = 1LL; v < substitution_nb_of_var; ++v) {
        if (substitution_index[v] != true) continue;
        printf("%lld - ",v);
        for (int_t j=0LL; substitution_values[v][j] != 0 ; j++) printf("%lld ",substitution_values[v][j]);
        printf("\n");
    }

    printf("\nPrinting substitution_assignment \n");
    for (int i = 1; i < substitution_nb_of_var; ++i){
        if (substitution_assignment[i] == 2 && substitution_assignment[-i] == 2) continue;
        printf("%d : %d %d\n",i,substitution_assignment[i],substitution_assignment[-i]);
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
    int_t sz = __SZ_SUB__;
	substitution_nb_of_var = _n_e;

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

    // Init tab assignment
    substitution_assignment = substitution_assignment_buffer + _n_e + 1LL; // substitution_assignment_buffer = 1802, _n_e = 900
    substitution_assignment[0LL] = __UNDEF__;

    for (int_t i = 1LL; i < _n_e; ++i)
        _substitution_unset(i)

    // Init tab qui permet de retenir les valeurs utiles à la substitution
    /*substitution_main_top_buffer = 0LL;
    substitution_values = substitution_values_buffer + _n_e +1LL;
    for(int_t i = 1LL; i <= _n_e; ++i) {
        substitution_values[i] = substitution_values[-i] = NULL;
    }

    for(int_t i = 1LL; i <= _n_e; ++i) {
        substitution_values[i] = substitution_main_buffer + substitution_main_top_buffer;
        for (int_t j = 0; j < __SZ_SUB__; ++j){
            substitution_main_buffer[substitution_main_top_buffer++] = 0;
        }

        substitution_values[-i] = substitution_main_buffer + substitution_main_top_buffer;
        for (int_t j = 0; j < __SZ_SUB__; ++j){
            substitution_main_buffer[substitution_main_top_buffer++] = 0;
        }
    }*/
    
    // for(int_t i = 1LL; i <= _n_e; ++i) {
    //     substitution_values[-i] = substitution_main_buffer + substitution_main_top_buffer;
    //     substitution_main_top_buffer+=sz;
    // }

    // Print de test
    // for (int_t j = 1LL; j < _n_e; ++j) {
    //     // printf("%lld|%lld ",substitution_values[j][0],substitution_values[-j][1]);
    //     printf("%lld|%lld ",substitution_values[j],substitution_values[-j]);
    // }
    // for (int i = 1; i < _n_e; ++i){
    //     printf("%d %d\n",substitution_assignment[i],substitution_assignment[-i]);
    // }
    return (true);
}

void substitution_check(const int_t v){ // Utile ?
    // Je dépile xorset_up_stack Ou je garde la variable v (faire un choix)

    for(int_t w = 1LL; w < substitution_nb_of_var; ++w) {
        if (substitution_index[w]){ 
            int_t _y = w;
            for (int_t j=0LL; j< __SZ_SUB__; j++) {
                if (substitution_values[_y][j] == v){
                    // j'empile la nouvelle valeur dans xorset_up_stack
                }
            }
        }
	}
    // La variable n'a pas de substitut 
}


bool substitution_subt(const int_t v){ // Le problème vient de la déclaration du tableau ou de la ligne d'affectation dans le tableau
    const bool _tf = (v < 0) ? false : true;

    if (_tf){ // Si x1 == true
        // x2 subst y, x1 = true
        for(int_t w = 0LL; w < substitution_nb_of_var; ++w) { // y <=> x1 x2
            if (substitution_equivalent[w]){ 
                int_t _y = w;
                for (int_t j=0LL; j< __SZ_SUB__; j++) { // x1 ? x2 ?
                    if (substitution_equivalency[_y][j] == v){
                        int_t _x;
                        int_t _end_y = substitution_end_vector(_y);

                        // Ici _x == v
                        if ( j == 0LL )
                            _x = substitution_equivalency[_y][1];
                        else
                            _x = substitution_equivalency[_y][0];

                        if (_substitution_is_true(_x)) continue; // Génère une erreur avec les 2 lignes d'affectation de la fin ?
                        else if (_substitution_is_false(_x)){
                            return false;
                        }
                        else {
                            _substitution_set(_x,__TRUE__)
                        }

                        substitution_index[_y]=true;
                        substitution_values[_y][_end_y]=_x;
                    }
                }
            }
	    }
    }
    else{
        // printf("ICI\n");
        // y = false, x1 = false
        if (_substitution_is_true(v)){
            return false;
        }
        else if (_substitution_is_undef(v)){
            _substitution_set(v,__FALSE__)
        }

        for(int_t w = 0LL; w < substitution_nb_of_var; ++w) { // y <=> x1 x2
            if (substitution_equivalent[w]){ 
                int_t _y = w;
                for (int_t j=0LL; j< __SZ_SUB__; j++) { // x1 ? x2 ?
                    if (substitution_equivalency[_y][j] == v){
                        if (_substitution_is_false(_y)) continue;
                        else if (_substitution_is_true(_y)){
                            return false;
                        }
                        else {
                            _substitution_set(_y,__FALSE__)
                        }
                    }
                }
            }
	    }
    }
    return true;
}


void substitution_undo() {

}
