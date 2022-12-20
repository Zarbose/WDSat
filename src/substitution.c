#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "wdsat_utils.h"
#include "substitution.h"
#include "dimacs.h"

// Structure pour connaitre les équivalences
uint_t substitution_equivalency[__SZ_VAR__][__SZ_SUB__];
bool substitution_equivalent[__SZ_VAR__];


uint_t substitution_values[__SZ_VAR__][__MAX_ID__];
bool substitution_index[__SZ_VAR__];

static uint_t substitution_nb_of_equations;

// substitution
void substitution_fprint() {
	uint_t v;
	for(v = 0LL; v <= substitution_nb_of_equations; ++v) {
        if (substitution_equivalent[v]){
            printf(" %lld <=> ",v);
            for (int_t j=0LL; j< __SZ_SUB__; j++) printf("%lld ",substitution_equivalency[v][j]);
            printf("\n");
        }
        
	}
}

inline void substitution_reset_boolean_vector(uint_t *v, uint_t sz) {
	for(uint_t i = 0ULL; i < sz; ++i){
		/// different writing for same aim
		/// v[i] = 0ULL;
		v[i] ^= v[i];
    }
}

bool substitution_initiate_from_dimacs() {
    const int_t _n_e = dimacs_nb_equations(); // Ici _n_e = 900
	substitution_nb_of_equations = _n_e;

    for(int_t i = 0LL; i <= _n_e; ++i) {
        substitution_reset_boolean_vector(substitution_equivalency[i],__SZ_SUB__);
		substitution_equivalent[i] = false;
        
        substitution_reset_boolean_vector(substitution_values[i],__SZ_SUB__);
		substitution_index[i] = false;
	}

    for(int_t i = 0LL; i < _n_e; ++i) {
        int_t _s_e = dimacs_size_of_equation(i);
        int_t * equation_buffer = dimacs_equation(i);
        if (_s_e == 3){
            substitution_equivalent[i]=true;
            substitution_equivalency[i][0LL] = -equation_buffer[0LL];
            substitution_equivalency[i][1LL] = -equation_buffer[1LL];
        }
	}
    return (true);
}

/*
void substitution_print_equivalency() {
	int_t i;
    const int_t _n_e = dimacs_nb_equations();

    for(i = 0LL; i < _n_e; ++i) {
        int_t _s_e = dimacs_size_of_equation(i);
        int_t * equation = dimacs_equation(i);
        if (_s_e == 3){
            printf(" %lld <=> %lld %lld",equation[2LL],-equation[0LL],-equation[1LL]);
        }
        else{
            continue;
        }
		printf("\n");
	}
}*/