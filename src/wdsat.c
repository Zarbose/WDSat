//
//  wdsat.c
//  WDSat
//
//  Created by Monika Trimoska on 15/12/2018.
//  Copyright © 2018 Monika Trimoska. All rights reserved.
//

#include <string.h>
#include<stdio.h>
#include<time.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include "wdsat.h"
#include "cnf.h"
#include "xorset.h"
#include "xorgauss.h"
#define VAR_GLOBALES
#include "dimacs.h"
#include "substitution.h"
#include "cycle.h"

// int_t nb_var = 0;

#define TEST_SUBST

// #define ENABLE_PRINT

// #define STAT
#define XOR_CONSTR

/// @var uint_t nb_of_vars;
/// @brief number of variables
// static int_t nb_of_vars; // Not used

/// @var int_t wdsat_cnf_up_stack[__ID_SIZE__];
/// @brief unit propagation cnf stack
static int_t wdsat_cnf_up_stack[__ID_SIZE__];

/// @var int_t wdsat_cnf_up_top_stack;
/// @brief unit propagation cnf stack top
static int_t wdsat_cnf_up_top_stack;

/// @var int_t wdsat_xorset_up_stack[__ID_SIZE__];
/// @brief unit propagation xorset stack
static int_t wdsat_xorset_up_stack[__ID_SIZE__];

/// @var int_t wdsat_xorset_up_top_stack;
/// @brief unit propagation xorset stack top
static int_t wdsat_xorset_up_top_stack;

/// @var int_t wdsat_substitution_up_stack[__ID_SIZE__];
/// @brief unit propagation subst stack
static int_t wdsat_substitution_up_stack[__ID_SIZE__];

/// @var int_t wdsat_substitution_up_top_stack;
/// @brief unit propagation subst stack top
static int_t wdsat_substitution_up_top_stack;

static int_t set[__ID_SIZE__];

void wdsat_save_result(int debut,ticks clockcycles_init ,int_t conf[],char *filename){
	clock_t fin = clock();
	int duree_ml = 1000*(fin-debut)/CLOCKS_PER_SEC;

	ticks clockcycles_last;
	clockcycles_last = getticks();
	float total_ticks = elapsed(clockcycles_last, clockcycles_init);
	// printf("OUIII %d\n",total_ticks);

	FILE* fichier = NULL;
	char path_file[1000];

	int seed=0;

	for(int i = 0; filename[i] != '\0'; i++){
		if (filename[i] == 's' && filename[i+1] == '_'){
			i+=2;
			int j=0;
			for (j = i; filename[j] != '_'; j++);
			filename[j]='\0';
			seed=atoi(filename+i);
			break;
		}
	}


	sprintf(path_file,"perfo_solveur/result_constr_xorgauss.csv");
	fichier=fopen(path_file,"a+");
	if (fichier != NULL){
		fprintf(fichier, "%d;%ld;%ld;%f;\n",seed,conf[0],duree_ml,total_ticks);
        fclose(fichier);
    }
    else{
        printf("Impossible d'ouvrir le fichier : %s\n",path_file);
		exit(2);
    }
	printf("End seed %d\n",seed);
}

void wdsat_fprint_result(int_t conf[], int debut, ticks clockcycles_init){

	clock_t fin = clock();
	int duree_ml = 1000*(fin-debut)/CLOCKS_PER_SEC;

	ticks clockcycles_last;
	clockcycles_last = getticks();

	float total_ticks = elapsed(clockcycles_last, clockcycles_init);

	printf("cnf_assignment:");
	for(int j = 1; j <= dimacs_nb_unary_vars(); j++)
	{
		printf("%d", cnf_assignment[j]);
	}
	printf("\n");

	printf("sub_assignment:");
	for(int j = 1; j <= dimacs_nb_unary_vars(); j++)
	{
		printf("%d", substitution_assignment[j]);
	}
	printf("\n");

	printf("conf:%ld\n",conf[0]);
	printf("temps_ml:%d\n",duree_ml);
	printf("ticks:%f\n",total_ticks);
}

// assign and propagate l to true using CNF and XORSET modules.
bool wdsat_set_true(const int_t l) {

    bool _next_loop;
    int_t _l;

    wdsat_xorset_up_top_stack = 0LL;
    wdsat_xorset_up_stack[wdsat_xorset_up_top_stack++] = l;
	wdsat_substitution_up_top_stack = 0LL;
	wdsat_substitution_up_stack[wdsat_substitution_up_top_stack++] = l;

    _next_loop = true;
    while(_next_loop) {
		_next_loop = false;

		while (wdsat_substitution_up_top_stack) {
			_l = wdsat_substitution_up_stack[--wdsat_substitution_up_top_stack];
			// printf("sub %ld\n",_l);
			if(_substitution_is_undef(_l)) _next_loop = true;
			if(!substitution_set_true(_l)) { /** printf("subt contr %lld\n",_l);/**/ return false; }
		}

		while(wdsat_xorset_up_top_stack) {
			_l = wdsat_xorset_up_stack[--wdsat_xorset_up_top_stack];
			// printf("xorset %ld\n",_l);
			if(_xorset_is_undef(_l)) _next_loop = true;

			if(!xorset_set_true(_l)) { /** printf("xor contr %lld\n",_l);/**/ return false; }
		}

		wdsat_xorset_up_top_stack = substitution_last_assigned(wdsat_xorset_up_stack);
		wdsat_substitution_up_top_stack = xorset_last_assigned(wdsat_substitution_up_stack);

	}
    return true;
}

bool wdsat_solve_rest(int_t l, int_t set_end, int_t conf[]/**/, int_t dec /**/) {

	if(l > set_end)
	{
#ifdef __FIND_ALL_SOLUTIONS__
		printf("SAT:\n");
		for(int i = 1; i <= dimacs_nb_unary_vars(); i++)
			printf("%d", cnf_assignment[i]);
		printf("\nconf:%ld\n", conf[0]);
		return false;
#endif
		return true;
	}
	#ifndef TEST_SUBST
		if(!_cnf_is_undef(set[l])) { // set[l] = l+1 == > l+1 est défine ?
			return wdsat_solve_rest(l + 1, set_end,conf, dec +1);
		}
	#else
		// set[l] = set[l] désigne le littéral en cours d'affectation
		if(!_substitution_is_undef(set[l])) {  // Si set[l] est définie
			return wdsat_solve_rest(l + 1, set_end,conf, dec + 1); // Alors je passe au littéral suivant
		}
	#endif

	#ifndef TEST_SUBST
		_cnf_breakpoint;
	#endif
	_xorset_breakpoint;
	#ifdef TEST_SUBST
		_substitution_breakpoint;
	#endif
	conf[0]++;

	/**
	printf("%d\n", l);
	/**/

	if(!wdsat_set_true(-set[l])) // ligne 5 et 5 algo 4.1
	{
		#ifndef TEST_SUBST
			cnf_undo();
		#endif
		xorset_undo();
		#ifdef TEST_SUBST
			substitution_undo();
		#endif

        /**
	    printf("1 %d\n", (l+1));
	    /**/

		if(!wdsat_set_true(set[l])) return false;
		return wdsat_solve_rest(l + 1, set_end,conf, dec + 1);

	}
	else
	{
		if(!wdsat_solve_rest(l + 1, set_end,conf, dec + 1))
		{
			#ifndef TEST_SUBST
				cnf_undo();
			#endif
			xorset_undo();
			#ifdef TEST_SUBST
				substitution_undo();
			#endif

            /**
	        printf("2 %d\n", (l+1));
	        /**/

			if(!wdsat_set_true(set[l])) return false;
			return wdsat_solve_rest(l + 1, set_end,conf, dec + 1);
		}
		else
		{
			#ifndef TEST_SUBST
				_cnf_mergepoint;
			#endif
			_xorset_mergepoint;
			#ifdef TEST_SUBST
				_substitution_mergepoint;
			#endif
			return true;
		}
	}

}

bool wdsat_solve_rest_XG(int_t l, int_t nb_min_vars, int_t conf[], int_t d) {
	int apro = (int) ((__MAX_ANF_ID__-1)-sqrt(2*__MAX_XEQ__));

	if(l > nb_min_vars)
	{
#ifdef __FIND_ALL_SOLUTIONS__
		printf("SAT:\n");
		for(int i = 1; i <= dimacs_nb_unary_vars(); i++)
			printf("%d", xorgauss_assignment[i]);
		printf("\nconf:%lld\n", conf[0]);
		return false;
#endif
		return true;
	}

	if ( nb_var >= apro){
		substitution_write_new_systeme();
		return false;
	}

#ifdef __DEBUG__
	printf("\nSetting:%d\n",set[l]);
	for(int i = 1; i <= dimacs_nb_unary_vars(); i++)
		printf("%d", xorgauss_assignment[i]);
	printf("\n");
#endif

	if(!_substitution_is_undef(set[l])) {
		// printf("A\n");
		return wdsat_solve_rest_XG(l + 1, nb_min_vars, conf, d + 1);
	}
	_substitution_breakpoint;
	_xorset_breakpoint;
	_xorgauss_breakpoint;
	conf[0]++;

	/**
	printf("%d\n", l);
	/**/
	if(!wdsat_infer(-set[l],conf,d))
	{
		// printf("B\n");
		substitution_undo();
		xorset_undo();
		xorgauss_undo();

#ifdef __DEBUG__
		printf("lev:%d--undo on 0\n",set[l]);
		for(int i = 1; i <= dimacs_nb_unary_vars(); i++)
			printf("%d", xorgauss_assignment[i]);
		printf("\n");
#endif
	}
	else
	{
		if(!wdsat_solve_rest_XG(l + 1, nb_min_vars, conf, d + 1))
		{
			// printf("C\n");
			substitution_undo();
			xorset_undo();
			xorgauss_undo();

#ifdef __DEBUG__
			printf("lev:%d--undo on 0 profond\n",set[l]);
			for(int i = 1; i <= dimacs_nb_unary_vars(); i++)
				printf("%d", xorgauss_assignment[i]);
			printf("\n");
#endif
		}
		else
		{
#ifdef __DEBUG__
			printf("lev:%d--ok on 0\n",set[l]);
#endif
			// printf("D\n");
			_substitution_mergepoint;
			_xorset_mergepoint;
			_xorgauss_mergepoint;
			return true;
		}
	}
	if(!wdsat_infer(set[l],conf,d))
	{
#ifdef __DEBUG__
		printf("lev:%d--undo on 1\n",set[l]);
#endif
		// printf("E\n");
		return false;
	}
#ifdef __DEBUG__
	for(int i = 1; i <= dimacs_nb_unary_vars(); i++)
		printf("%d", xorgauss_assignment[i]);
	printf("\n");
#endif

	// printf("F\n");
	return wdsat_solve_rest_XG(l + 1, nb_min_vars, conf, d + 1);
}

int start = 0;
int cpt=0;

bool wdsat_infer(const int_t l, int_t conf[], int_t d) {
	int apro = (int) ((__MAX_ANF_ID__-1)-sqrt(2*__MAX_XEQ__));
	
	// printf("--- %ld ---\n",l);

	bool _loop_pass = true;
	bool _continue;

	int_t substitution_history_it;
	int_t substitution_history_last = substitution_history_top;
	int_t xorgauss_history_it;
	int_t xorgauss_history_last = xorgauss_history_top;
	int_t _l;

	int v=0;

	if ( nb_var <= apro+v){
		if(!wdsat_set_true(l)){ return false; }
	}

	/**/
	if(nb_var >= apro+v+1){
		// printf("%d %ld %d\n",conf[0],nb_var, apro);
		// substitution_write_new_systeme();
		// sleep(1);
		// return false;
		// exit(0);
	// }
	
	/**/

		while(_loop_pass) {
			_continue = false;
			substitution_history_it = substitution_history_top;
			while(substitution_history_it > substitution_history_last) {
				_l = substitution_history[--substitution_history_it];
				if(_xorgauss_is_undef(_l)) {
					if(!xorgauss_set_true(_l)) { /** printf("gauss contr %lld\n",_l); /**/ return false; }
					_continue = true;
				}
			}
			substitution_history_last = substitution_history_top;

			_loop_pass = false;
			if(_continue) {
				xorgauss_history_it = xorgauss_history_top;
				while(xorgauss_history_it > xorgauss_history_last) {
					_l = xorgauss_history[--xorgauss_history_it];
					if(_substitution_is_false(_l)) { /** printf("subt contr %lld\n",_l); /**/ return false; }
					if(_substitution_is_undef(_l)) {
						_loop_pass = true;
						if(!wdsat_set_true(_l)) {
							return false;
						}
					}
				}
				xorgauss_history_last = xorgauss_history_top;
			}
		}
		// return false;
	}
	return true;
}

bool wdsat_solve(int_t n, int_t new_l, int_t new_m, char *irr, char *X3, int_t xg, char mvc_graph[1000], char thread[1000],int S, char *filename) {
	int_t j;
	int_t nb_min_vars;
	int_t conf[1]={0};

	cnf_initiate_from_dimacs();
	xorset_initiate_from_dimacs();
	substitution_initiate_from_dimacs();

	if(!xorgauss_initiate_from_dimacs())
	{
		printf("UNSAT on XORGAUSS init\n");
		return false;
	}
	cpy_from_dimacs();

	//check allocated memory
	if(dimacs_nb_vars() < __MAX_ID__)
		printf("\n!!! Running times are not optimal with these parameters. Set the __MAX_ID__ constant to %d !!!\n\n", dimacs_nb_vars());

	//code for multithread: assign prefix for this thread
	if(strlen(thread) > 0)
	{
		char *str_l;
		int_t l;
		str_l = strtok (thread, ",");
		while(str_l != NULL)
		{
			l = atoi(str_l);
			cnf_up_stack[cnf_up_top_stack++] = l;
			assert(cnf_up_top_stack < __ID_SIZE__);
			str_l = strtok (NULL, ",");
		}
	}
	// end code for multithread (this has to be done before wdsat_infer_unitary();

	if(strlen(mvc_graph) > 0)
	{
		nb_min_vars = 0;
		char *str_l;
		j = 0;
		str_l = strtok (mvc_graph, ",");
		while(str_l != NULL)
		{
			set[j] = atoi(str_l);
			str_l = strtok (NULL, ",");
			nb_min_vars++;
			j++;
		}
	}
	else
	{
#ifdef __XG_ENHANCED__
		nb_min_vars = dimacs_nb_unary_vars();
#else
		nb_min_vars = dimacs_nb_vars();
#endif
		for(j = 1; j <= nb_min_vars; j++)
		{
			set[j - 1] = j;
		}
	}


	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
		// xorgauss_write_origin();
	/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */


	
	// xorgauss_fprint_for_xorset();
	// xorgauss_fprint();
	// xorgauss_fprint_system();

	// xorset_fprint();
	// xorset_index_structure_fprintf();

	// dimacs_print_formula();
	// dimacs_print_equivalency();
	// dimacs_print_table();

	// cnf_fprint();

	// substitution_fprint_equivalency_unary();

	clock_t debut = clock();

	ticks clockcycles_init;
	clockcycles_init = getticks();

	if(xg == 0)
	{
		if(!wdsat_solve_rest(0, nb_min_vars - 1, conf,0)) {
			printf("UNSAT\n");
			#ifdef ENABLE_PRINT
				wdsat_fprint_result(conf,debut,clockcycles_init);
			#endif

			if (S == 1) wdsat_save_result(debut,clockcycles_init,conf,filename);

			substitution_free_structure();
			return false;
		}
	}
	if(xg == 1)
	{
		ticks clockcycles_init, clockcycles_last;
		clockcycles_init = getticks();
		// printf("init : %d\n",clockcycles_init);
		if(!wdsat_solve_rest_XG(0, nb_min_vars - 1, conf, 0)) {
			printf("UNSAT\n");
			#ifdef ENABLE_PRINT
				wdsat_fprint_result(conf,debut,clockcycles_init);
			#endif

			if (S == 1) wdsat_save_result(debut,clockcycles_init,conf,filename);

			#ifdef STAT
				clockcycles_last = getticks();
				float diff_ticks = elapsed(clockcycles_last, clockcycles_init);
				printf("total;%d\n",diff_ticks);
			#endif

			substitution_free_structure();
			return false;
		}
		#ifdef STAT
			clockcycles_last = getticks();
			float diff_ticks = elapsed(clockcycles_last, clockcycles_init);
			printf("total;%d\n",diff_ticks);
		#endif
	}
	#ifdef ENABLE_PRINT
		wdsat_fprint_result(conf,debut,clockcycles_init);
	#endif
	if (S == 1) wdsat_save_result(debut,clockcycles_init,conf,filename);

	// substitution_fprint_values();
	// cnf_fprint();

	// xorset_index_structure_fprintf();

	// dimacs_print_formula();

	substitution_free_structure();
	return (true);

}
