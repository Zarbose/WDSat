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

int_t substitution_end_vector(const int_t v){
    int_t i;
    for(i = 0LL; substitution_values[v][i] != 0; i++);

    return i;
}

void substitution_set_true(const int_t v){
    uint_t w;
    substitution_index[v]=true;
    
	for(w = 0LL; w < substitution_nb_of_equations; ++w) {
        if (substitution_equivalent[w]){
            uint_t _y = w;
            for (int_t j=0LL; j< __SZ_SUB__; j++) {
                if (substitution_equivalency[_y][j] == v){
                    int_t _x, _end_x;
                    int_t _end_y = substitution_end_vector(_y);

                    if ( j == 0LL )
                        _x = substitution_equivalency[_y][1];
                    else
                        _x = substitution_equivalency[_y][0];

                    _end_x = substitution_end_vector(_x); 

                    substitution_values[_x][_end_x]=_y;
                    substitution_values[_y][_end_y]=_x;
                }
            }
        }
	}
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