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

// new undo structures
int_t *substitution_history_inte_stack;
int_t substitution_history_inte_top;
int_t *substitution_history_main_stack;
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
        printf("%ld : ",v);
        for (int_t j=0LL; j < substitution_index_static[v] ; j++) printf("%ld ",substitution_values_static[v][j]);
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

void substitution_fprint_dynamic_index(){
    const int_t _n_v = substitution_nb_of_var;
    printf("Printing substitution_index_dynamic \n");
    for(int_t v = -_n_v; v <= _n_v; ++v) {
        printf("%ld ",substitution_index_dynamic[v]);
    }
    printf("\n");
}

void substitution_fprint_history_main_stack(){
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

// Utils functions
void substitution_reset_stack(){
    ++substitution_tag;
    substitution_up_top_stack=0;
}

void substitution_add_check_stack(int_t v){
    if ( !(substitution_index_stack[v] == substitution_tag) ){
        substitution_up_stack[substitution_up_top_stack++]=v;
        substitution_index_stack[v]=substitution_tag;
    }
}

void substitution_add_check_history_stack(int_t v){
     if ( !(substitution_history_inte_index_stack[v] == substitution_history_tag) ){
        substitution_history_inte_stack[substitution_history_inte_top++]=substitution_index_dynamic[v];
        substitution_history_inte_stack[substitution_history_inte_top++]=v;
        substitution_history_inte_index_stack[v]=substitution_history_tag;
    }
}

void substitution_increase_history_flag(){
    substitution_history_tag++;
}

inline void substitution_reset_boolean_vector(int_t *v, uint_t sz) {
	for(uint_t i = 0ULL; i < sz; ++i){
		v[i] = 0ULL;
    }
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

    free(substitution_history_inte_stack);
    free(substitution_history_main_stack);
    free(substitution_history_inte_index_stack_buffer);

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
            substitution_values_static[_y][substitution_index_static[_y]++]=_x1; // ICI
            substitution_values_static[_y][substitution_index_static[_y]++]=_x2; // ICI

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
                    substitution_add_check_history_stack(_y);
                    substitution_add_check_history_stack(_x);
                    substitution_add_check_history_stack(-_x);
                    substitution_add_check_history_stack(-_y);

                    substitution_values_dynamic[_x][substitution_index_dynamic[_x]++]=_y;

                    substitution_values_dynamic[_y][substitution_index_dynamic[_y]++]=_x;
                    
                    substitution_values_dynamic[-_y][substitution_index_dynamic[-_y]++]=-_x;
                    
                    substitution_values_dynamic[-_x][substitution_index_dynamic[-_x]++]=-_y;
                    
                }
            }
        }
    }
    // printf("add %ld to %ld\n",substitution_history_inte_top,substitution_history_main_top);
    // substitution_history_main_stack[substitution_history_main_top++]=substitution_history_inte_top;
}

// Init functions
bool substitution_initiate_from_dimacs() {
    const int_t _n_e = dimacs_nb_equations(); // Ici _n_e = 900
    const int_t _n_v = dimacs_nb_vars(); // Ici _n_v = 325
    const int_t _n_uv = dimacs_nb_unary_vars();
	substitution_nb_of_var = _n_v;
    substitution_nb_unary_vars = _n_uv;

    // printf("%d \n",__SZ_STACK__);

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

    // new undo structures
    substitution_step_top = substitution_history_top = substitution_history_top_it = substitution_history_tag =0LL;
    substitution_history_tag++;

    substitution_history_main_top = substitution_history_inte_top = 0LL;

    substitution_history_inte_stack = (int_t *)malloc((__SZ_STACK__)*sizeof(int_t));
    substitution_history_main_stack = (int_t *)malloc((__SIGNED_ID_SIZE__)*sizeof(int_t));
    CLEAR(substitution_history_inte_stack,__SIGNED_ID_SIZE__);
    CLEAR(substitution_history_main_stack,__SIGNED_ID_SIZE__);

    substitution_history_inte_index_stack_buffer = (int_t *)malloc((__SIGNED_ID_SIZE__)*sizeof(int_t));
    substitution_history_inte_index_stack = substitution_history_inte_index_stack_buffer +_n_v;

    // Filling in tables
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
        
        // Undo

        // Index
        substitution_index_stack[i] = substitution_index_static[i] = substitution_index_dynamic[i] = substitution_history_inte_index_stack[i] = 0;
    }
    // CLEAR(substitution_history_index_dynamic,__SIGNED_ID_SIZE__);
    // CLEAR(substitution_index_dynamic,__SIGNED_ID_SIZE__);
    // CLEAR(substitution_index_static,__SIGNED_ID_SIZE__);
    // CLEAR(substitution_index_stack,__SIGNED_ID_SIZE__);

    substitution_init_static_table();
    return (true);
}

// "main" functions
bool substitution_set_true(const int_t l) {
    assert(abs((int) l) <= substitution_nb_of_var);
	
    // substitution_up_stack[substitution_up_top_stack++] = l;
    substitution_add_check_stack(l);

    const bool _tf = (l < 0) ? false : true;
    if (substitution_is_unary_var(l)){
        if (_tf){
            substitution_update_dynamic_values(l);
        }
    }
    else{
        if (!_tf){ /* La régle x26 = false */
            int_t _v = (l < 0) ? -l : l;
            if (substitution_equivalent[_v] == true){
                int_t x1 = substitution_equivalency[_v][0];
                int_t x2 = substitution_equivalency[_v][1];

                if (!_substitution_is_undef(x1) && !_substitution_is_undef(x2)){
                    /* x1 and x2 def */
                    if ( _substitution_is_true(x1) && _substitution_is_true(x2)){
                        // printf("x1 = %d x2 = %d\n",_substitution_is_true(x1),_substitution_is_true(x2));
                        return false;
                    }
                }
                else if (_substitution_is_undef(x1) && !_substitution_is_undef(x2)){
                    /* x1 undef */
                    // if (_substitution_is_true(x2)){ // Attention aux doublons
                    //     substitution_values_dynamic[_v][substitution_index_dynamic[_v]++]=-x1;  // A vérifier
                    //     substitution_values_dynamic[-_v][substitution_index_dynamic[-_v]++]=x1; // A vérifier
                    // }
                }
                else if (!_substitution_is_undef(x1) && _substitution_is_undef(x2)){
                    /* x2 undef */
                    // if (_substitution_is_true(x1)){ // Attention aux doublons
                    //     substitution_values_dynamic[_v][substitution_index_dynamic[_v]++]=-x2;  // A vérifier
                    //     substitution_values_dynamic[-_v][substitution_index_dynamic[-_v]++]=x2; // A vérifier
                    // }
                }
                else{
                    /* x1 and x2 undef */
                }
            }
        }
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
            substitution_history[substitution_history_top++] = l;

            // Static
            for (int_t i = 0; i != substitution_index_static[l]; ++i){
                const int_t _e = substitution_values_static[l][i];
                if(_substitution_is_false(_e)){
                    substitution_reset_stack();
                    return false;
                }
                else if(_substitution_is_undef(_e)){
                    substitution_add_check_stack(_e);
                }
            }

            // Dynamic
            for (int_t i = 0; i != substitution_index_dynamic[l]; ++i){
                const int_t _e = substitution_values_dynamic[l][i];
                if(_substitution_is_false(_e)){
                    substitution_reset_stack();
                    return false;
                }
                else if(_substitution_is_undef(_e)){
                    substitution_add_check_stack(_e);
                }
            }

            /*for (int_t i = 0; substitution_values_dynamic[-l][i] != 0; ++i){
                const int_t _e = substitution_values_dynamic[-l][i];
                if(_substitution_is_false(_e)){
                    printf("END 4\n");
                    substitution_reset_stack();
                    return false;
                }
                else if(_substitution_is_undef(_e)){
                    printf("add 3 %ld to stack\n",_e);
                    substitution_add_check_stack(_e);
                }
            }*/
            
        }
    }
    return true;
}

// Undo functions
void substitution_undo() {
    static int_t _l;
    const int_t top_step = (substitution_step_top) ? substitution_step[--substitution_step_top] : 0;
    while(substitution_history_top != top_step) {
        _l = substitution_history[--substitution_history_top];
        _substitution_unset(_l);
    }
	substitution_history_top_it = substitution_history_top;

    // new undo
    const int_t top_step_main = (substitution_history_main_top > 0) ? substitution_history_main_stack[--substitution_history_main_top] : 0;
    while ( top_step_main != substitution_history_inte_top) {
        int_t l = substitution_history_inte_stack[--substitution_history_inte_top];
        int_t indx = substitution_history_inte_stack[--substitution_history_inte_top];
        substitution_index_dynamic[l]=indx;
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

void substitution_reset_string(char* str){
    str[8]='\0';
}

void substitution_testing_vars(bool writing_status){
    char path[100]="testing/";
    char var[100]="";
    FILE* fichier;

    // substitution_values_dynamic
    strcpy(var,"substitution_values_dynamic");
    strcat(path,var);
    if (writing_status == true)
        strcat(path,"_before.txt");
    else
        strcat(path,"_after.txt");

    fichier = fopen(path,"a");
    if (fichier == NULL){
        printf( "Cannot open file %s\n", path );
        exit(2);
    }
    else {
        const int_t _n_v = substitution_nb_of_var;
        for(int_t v = -_n_v; v <= _n_v; ++v) {
            if (!v) continue;
            if (substitution_index_dynamic[v] == 0) continue;
            fprintf(fichier,"%ld : ",v);
            for (int_t j=0LL; j < substitution_index_dynamic[v]; j++) fprintf(fichier,"%ld ",substitution_values_dynamic[v][j]);
            fprintf(fichier,"\n");
        }
        substitution_reset_string(path);
        fclose(fichier);
    }

    // substitution_index_dynamic
    strcpy(var,"substitution_index_dynamic");
    strcat(path,var);
    if (writing_status == true)
        strcat(path,"_before.txt");
    else
        strcat(path,"_after.txt");

    fichier = fopen(path,"a");
    if (fichier == NULL){
        printf( "Cannot open file %s\n", path );
        exit(2);
    }
    else {
        const int_t _n_v = substitution_nb_of_var;
        for(int_t v = -_n_v; v <= _n_v; ++v) {
            if (!v) continue;
            if (substitution_index_dynamic[v] == 0) continue;
            fprintf(fichier,"%ld : ",v);
            for (int_t j=0LL; j < substitution_index_dynamic[v]; j++) fprintf(fichier,"%ld ",substitution_values_dynamic[v][j]);
            fprintf(fichier,"\n");
        }
        substitution_reset_string(path);
        fclose(fichier);
    }

    // substitution_assignment
    strcpy(var,"substitution_assignment");
    strcat(path,var);
    if (writing_status == true)
        strcat(path,"_before.txt");
    else
        strcat(path,"_after.txt");

    fichier = fopen(path,"a");
    if (fichier == NULL){
        printf( "Cannot open file %s\n", path );
        exit(2);
    }
    else {
        const int_t _v = substitution_nb_of_var;
        for (int_t i = 1LL; i <= _v; ++i){
            fprintf(fichier,"%ld  %ld: %d %d\n",-i,i,substitution_assignment[-i],substitution_assignment[i]);
        }
        substitution_reset_string(path);
        fclose(fichier);
    }

    // substitution_history_inte_stack
    strcpy(var,"substitution_history_inte_stack");
    strcat(path,var);
    if (writing_status == true)
        strcat(path,"_before.txt");
    else
        strcat(path,"_after.txt");

    fichier = fopen(path,"a");
    if (fichier == NULL){
        printf( "Cannot open file %s\n", path );
        exit(2);
    }
    else {
        for (int_t i=0; i < substitution_history_inte_top; i+=2){
            if (substitution_history_inte_stack[i] >= 10)
                fprintf(fichier,"%ld %ld\n",substitution_history_inte_stack[i],substitution_history_inte_stack[i+1]);
            else
                fprintf(fichier,"%ld %ld\n",substitution_history_inte_stack[i],substitution_history_inte_stack[i+1]);
        }

        substitution_reset_string(path);
        fclose(fichier);
    }

    // substitution_history_main_stack
    strcpy(var,"substitution_history_main_stack");
    strcat(path,var);
    if (writing_status == true)
        strcat(path,"_before.txt");
    else
        strcat(path,"_after.txt");

    fichier = fopen(path,"a");
    if (fichier == NULL){
        printf( "Cannot open file %s\n", path );
        exit(2);
    }
    else {
        for (int i = 0; i < substitution_history_main_top; i++)
            fprintf(fichier,"%ld \n",substitution_history_main_stack[i]);

        substitution_reset_string(path);
        fclose(fichier);
    }

    // substitution_history_inte_index_stack
    // strcpy(var,"substitution_history_inte_index_stack");
    // strcat(path,var);
    // if (writing_status == true)
    //     strcat(path,"_before.txt");
    // else
    //     strcat(path,"_after.txt");

    // fichier = fopen(path,"a");
    // if (fichier == NULL){
    //     printf( "Cannot open file %s\n", path );
    //     exit(2);
    // }
    // else {
    // const int_t _n_v = substitution_nb_of_var;
    //     for(int_t v = -_n_v; v <= _n_v; ++v) {
    //         if (!v) continue;
    //         fprintf(fichier,"%ld\n",substitution_history_inte_index_stack[v]);
    //     }

    //     substitution_reset_string(path);
    //     fclose(fichier);
    // }

    // substitution_up_stack
    // strcpy(var,"substitution_up_stack");
    // strcat(path,var);
    // if (writing_status == true)
    //     strcat(path,"_before.txt");
    // else
    //     strcat(path,"_after.txt");

    // fichier = fopen(path,"a");
    // if (fichier == NULL){
    //     printf( "Cannot open file %s\n", path );
    //     exit(2);
    // }
    // else {
    //     for (int_t i = 0; i < substitution_up_top_stack; i++){
    //         fprintf(fichier,"%ld\n",substitution_up_stack[i]);
    //     }

    //     substitution_reset_string(path);
    //     fclose(fichier);
    // }

    // substitution_index_stack
    // strcpy(var,"substitution_index_stack");
    // strcat(path,var);
    // if (writing_status == true)
    //     strcat(path,"_before.txt");
    // else
    //     strcat(path,"_after.txt");

    // fichier = fopen(path,"a");
    // if (fichier == NULL){
    //     printf( "Cannot open file %s\n", path );
    //     exit(2);
    // }
    // else {
    //     const int_t _n_v = substitution_nb_of_var;
    //     for(int_t v = -_n_v; v <= _n_v; ++v) {
    //         if (!v) continue;
    //         fprintf(fichier,"%ld ",substitution_index_stack[v]);
    //     }
    //     fprintf(fichier,"\n");

    //     substitution_reset_string(path);
    //     fclose(fichier);
    // }
}