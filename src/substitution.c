#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "wdsat_utils.h"
#include "substitution.h"
#include "dimacs.h"


uint_t substitution_equivalency[__ID_SIZE__][__SZ_SUB__];
bool substitution_equivalent[__ID_SIZE__];

static uint_t substitution_nb_of_vars;
static uint_t substitution_nb_of_equations;


void substitution_fprint() {
	uint_t v;
	for(v = 1ULL; v <= substitution_nb_of_vars; ++v) {
        if (substitution_equivalent[v]){
            printf(" %lls <=> ",v);
            for (int_t j=0LL; j< __SZ_SUB__; j++) printf("%lld %lld\n",substitution_equivalency[v][0LL],substitution_equivalency[v][1LL]);
        }
	}
}

// substitution
bool substitution_initiate_from_dimacs() {
    const int_t _n_e = dimacs_nb_equations();
	substitution_nb_of_equations = _n_e;

	// assert(substitution_nb_of_vars <= __MAX_ID__);
	// assert(substitution_nb_of_equations <= __MAX_XEQ__);

    for(int_t i = 0LL; i <= _n_e; ++i) {
        // Reset vector
		substitution_equivalent[i] = false;
	}

    for(int_t i = 0LL; i < _n_e; ++i) {
        int_t _s_e = dimacs_size_of_equation(i);
        int_t * equation = dimacs_equation(i);
        if (_s_e == 3){
            substitution_equivalent[i]=true;
            printf("1\n");
            substitution_equivalency[i][0LL] = -equation[0LL]; // Ici erreur de segmentation
            printf("2\n");
            substitution_equivalency[i][1LL] = -equation[1LL];
            printf("3\n");
        }
	}
    printf("FIN\n");
}

// Ok
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
}