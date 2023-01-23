#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "wdsat_utils.h"
#include "substitution.h"
#include "dimacs.h"
#include "xorset.h"


int_t substitution_up_stack[__ID_SIZE__];
int_t substitution_up_top_stack;
int_t substitution_index_stack[__ID_SIZE__];
int_t substitution_tag;

boolean_t substitution_assignment_buffer[__SIGNED_ID_SIZE__];
boolean_t *substitution_assignment;


// Structure pour connaitre les équivalences
int_t substitution_equivalency[__ID_SIZE__][__SZ_SUB__]; // Tableau qui contient les équivalences
bool substitution_equivalent[__ID_SIZE__]; // Index pour parcourir le tableau d'équivalence plus rapidement



// Structure de la table d'équivalence
static int_t **substitution_values;
static int_t **substitution_values_buffer;

// static int_t substitution_values[__ID_SIZE__][__SZ_SUB__]; // Table de transition pour la substitution

static bool substitution_index[__ID_SIZE__]; // Index pour parcourir plus vite substitution_values




static uint_t substitution_nb_of_var; // Ici substitution_nb_of_var = 325

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
    printf("x : i -i\n");
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
    const int_t _n_v = dimacs_nb_vars(); // Ici _n_v = 325
    // int_t sz = __SZ_SUB__;
	substitution_nb_of_var = _n_v;

    // Init tab qui contient les équivalence : y <=> x1 x2
    for(int_t i = 0LL; i <= _n_v; ++i) {
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
    substitution_tag=0LL;
    substitution_tag++;

    // Init tab assignment
    substitution_assignment = substitution_assignment_buffer + _n_v + 1LL; // substitution_assignment_buffer = 652, _n_v = 325
    substitution_assignment[0LL] = __UNDEF__;

    for (int_t i = 1LL; i < _n_v; ++i)
        _substitution_unset(i);



    // Init tab values
    // on suppose qu'il existe n litteraux, il faut donc 2n+1 dimensions dans un tableau T 
    substitution_values_buffer = (int_t **)malloc((__SIGNED_ID_SIZE__) * sizeof(int_t *));
    substitution_values = substitution_values_buffer + _n_v; // adressage à partir du centre du tableau

    for(int_t i = -_n_v; i <= _n_v; ++i) {
        if(!i) {
            substitution_values[i] = NULL;
            continue;
        }
        int_t _sub_size = 2;
        substitution_values[i] = (int_t*)malloc(_sub_size * sizeof(int_t));
        CLEAR(substitution_values[i],_sub_size);
    }

    // Print de test
    /*for (int_t j = 1LL; j <= _n_v; ++j) {
        printf("%lld : %lld|%lld \n",j,substitution_values[j][0],substitution_values[-j][0]);
    }*/
    return (true);
}

// Reset the stack
void substitution_reset_stack(){
    ++substitution_tag;
    substitution_up_top_stack=0;
}

// Add v inside the stack checking if it's not already inside
void substitution_add_check_stack(int_t v){
    if ( !(substitution_index_stack[v] == substitution_tag) ){
        printf("Oui %d \n",v);
        substitution_up_stack[substitution_up_top_stack++]=v;
        substitution_index_stack[v]=substitution_tag;
    }
    else
        printf("Non %d \n",v);
}

bool substitution_subt(){ // Le problème vient de la déclaration du tableau ou de la ligne d'affectation dans le tableau
    // const bool _tf = (v < 0) ? false : true;
    // const uint_t _uv = (uint_t) ((v < 0) ? -v : v);

    static int_t l;
    while(substitution_up_top_stack) {
        l = substitution_up_stack[--substitution_up_top_stack];    
        
        const bool _tf = (l < 0) ? false : true;
        const uint_t _uv = (uint_t) ((l < 0) ? -l : l);

        if (_tf){ // Si x1 == true
            // x2 subst y, x1 = true
            if (_substitution_is_false(_uv)){
                substitution_reset_stack();
                return false;
            }
            else if(_substitution_is_undef(_uv)){
                _substitution_set(_uv,__TRUE__) // x1 = true
            
                for(int_t w = 0LL; w < substitution_nb_of_var; ++w) { // y <=> x1 x2
                    if (substitution_equivalent[w]){ 
                        int_t _y = w;
                        for (int_t j=0LL; j< __SZ_SUB__; j++) { // x1 ? x2 ?
                            if (substitution_equivalency[_y][j] == _uv){
                                int_t _x;
                                int_t _end_y = substitution_end_vector(_y);

                                // Ici _x == _uv ou x2
                                if ( j == 0LL )
                                    _x = substitution_equivalency[_y][1];
                                else
                                    _x = substitution_equivalency[_y][0];

                                // x2 subst y
                                substitution_values[_y][_end_y]=_x;
                                substitution_index[_y]=true;
                            }
                        }
                    }
                }
            }
        }
        else{
            // y = false, x1 = false
            if (_substitution_is_true(_uv)){
                return false;
                substitution_reset_stack();
            }
            else if (_substitution_is_undef(_uv)){
                _substitution_set(_uv,__FALSE__)

                for(int_t w = 0LL; w < substitution_nb_of_var; ++w) { // y <=> x1 x2
                    if (substitution_equivalent[w]){ 
                        int_t _y = w;
                        for (int_t j=0LL; j< __SZ_SUB__; j++) { // x1 ? x2 ?
                            if (substitution_equivalency[_y][j] == _uv){
                                if (_substitution_is_true(_y)){
                                    return false;
                                    substitution_reset_stack();
                                }
                                else if (_substitution_is_undef(_y)){
                                    _substitution_set(_y,__FALSE__)
                                }
                            }
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
