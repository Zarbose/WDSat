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
#include <time.h>

#include "wdsat.h"
#include "cnf.h"
#include "xorset.h"
#include "xorgauss.h"
#include "dimacs.h"
#include "substitution.h"

// #define SMALL_TEST  // Activation ou non de la zone de test
#define TEST_SUBST  // Utilisation ou non du module substitution
#define NO_CNF // Utilisation ou non du module CNF

int cpt = 0;

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

/// @var int_t wdsat_cnf_up_top_stack;
/// @brief unit propagation cnf stack top
static int_t wdsat_xorset_up_top_stack;


static int_t wdsat_substitution_up_stack[__ID_SIZE__];
static int_t wdsat_substitution_up_top_stack;

static int_t set[__ID_SIZE__];

int_t nb_activation=0LL;
int d_arbre=0;

void save_result(int duree_ml, int_t conf[]){
	FILE* fichier = NULL;

	char path_file[1000];
	// sprintf(&path_file,"result/Rainbow/result_N_25_M_53_K1_%d_K2_%d_K3_%d.csv",K1,K2,K3);
	sprintf(path_file,"result/Rainbow/experimentation_in_progress/to_analyse/result_N_25_M_53_K1_%d_K2_%d_K3_%d.csv",K1,K2,K3);

	fichier=fopen(path_file,"a+");
	if (fichier != NULL){
		fprintf(fichier, "%ld;%ld;%d;%d;%d;%d;%d\n",conf[0],nb_activation,duree_ml,K1,K2,K3,d_arbre);
        fclose(fichier);
    }
    else{
        printf("Impossible d'ouvrir le fichier : %s\n",path_file);
		exit(2);
    }
}

// assign and propagate l to true using CNF and XORSET modules.
bool wdsat_set_true(const int_t l) {
	printf("Nouveau : %ld\n",l);
    bool _next_loop;
    int_t _l;
	#ifndef NO_CNF
		wdsat_cnf_up_top_stack = 0LL;
		wdsat_cnf_up_stack[wdsat_cnf_up_top_stack++] = l;
	#endif
    wdsat_xorset_up_top_stack = 0LL;
    wdsat_xorset_up_stack[wdsat_xorset_up_top_stack++] = l;
	#ifdef TEST_SUBST
		wdsat_substitution_up_top_stack = 0LL;
		wdsat_substitution_up_stack[wdsat_substitution_up_top_stack++] = l;
	#endif

    _next_loop = true;
    while(_next_loop) {
		_next_loop = false;
		#ifndef NO_CNF
		while(wdsat_cnf_up_top_stack) {
			_l = wdsat_cnf_up_stack[--wdsat_cnf_up_top_stack];
			if(_cnf_is_undef(_l)) _next_loop = true;
			if(!cnf_set_true(_l)) {/*printf("ter contr %lld\n",_l);*/return false;} // assign and propagate _l to true.
		}
		#endif
		while(wdsat_xorset_up_top_stack) {
			_l = wdsat_xorset_up_stack[--wdsat_xorset_up_top_stack];
			// printf("xorset : %ld\n",_l);
			if(_xorset_is_undef(_l)) _next_loop = true;
			if(!xorset_set_true(_l)) {/*printf("xor contr %lld\n",_l);*/return false;} // assign and propagate _l to true.
		}
		#ifdef TEST_SUBST
			while (wdsat_substitution_up_top_stack) {
				_l = wdsat_substitution_up_stack[--wdsat_substitution_up_top_stack];
				if(_substitution_is_undef(_l)) _next_loop = true;
				if(!substitution_set_true(_l)) {/*printf("subt contr %lld\n",_l);*/return false;}
		}
		#endif
		#ifndef NO_CNF
			wdsat_cnf_up_top_stack = xorset_last_assigned(wdsat_cnf_up_stack);
		#endif
		wdsat_xorset_up_top_stack = cnf_last_assigned(wdsat_xorset_up_stack);
		#ifdef TEST_SUBST
			wdsat_substitution_up_top_stack = substitution_last_assigned(wdsat_substitution_up_stack);
		#endif
	}

	// printf("Return true for %ld \n",l);
    return true;
}
int r = 0;
bool wdsat_solve_rest(int_t l, int_t set_end, int_t conf[]) {
	// printf("Nouveau : %ld\n",l);

	// for(int j = 1; j <= dimacs_nb_unary_vars(); j++)
	// {
	// 	printf("%d", cnf_assignment[j]);
	// }
	// printf("\n");
	// printf("l = %lld\n",l);

	if(l > set_end) // C'est la fin ?
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
	#ifndef NO_CNF
		if(!_cnf_is_undef(set[l])) { // set[l] = l+1 == > l+1 est défine ?
			printf("Ici avec l = %ld et set[l] = %ld\n",l,set[l]);
			return wdsat_solve_rest(l + 1, set_end,conf);
		} 
	#else
		// set[l] = set[l] désigne le littéral en cours d'affectation
		if(!_substitution_is_undef(set[l])) {  // Si set[l] est définie
			return wdsat_solve_rest(l + 1, set_end,conf); // Alors je passe au littéral suivant
		} 
	#endif

	#ifndef NO_CNF
		_cnf_breakpoint;
	#endif
	_xorset_breakpoint;
	#ifdef TEST_SUBST
		_substitution_breakpoint;
	#endif
	conf[0]++;
	if(!wdsat_set_true(-set[l]))
	{
		printf("Backtrack 1\n");
		#ifndef NO_CNF
			cnf_undo();
		#endif
		xorset_undo();
		#ifdef TEST_SUBST
			substitution_undo();
		#endif
		if(!wdsat_set_true(set[l])) return false;
		return wdsat_solve_rest(l + 1, set_end,conf);
		
	}
	else
	{
		if(!wdsat_solve_rest(l + 1, set_end,conf))
		{
			printf("Backtrack 2\n");
			#ifndef NO_CNF
				cnf_undo();
			#endif
			xorset_undo();
			#ifdef TEST_SUBST
				// system("/bin/bash script/testing_vars/clean.sh");
				// substitution_testing_vars(true);
				substitution_undo();
				// substitution_testing_vars(false);
				// exit(3);
			#endif
			if(!wdsat_set_true(set[l])) return false;
			return wdsat_solve_rest(l + 1, set_end,conf);
		}
		else // Retour récursif ??? (non)
		{
			#ifndef NO_CNF
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
    // if (d < 10) 
	// printf("%d\n", d);

	if(l > nb_min_vars)
	{
#ifdef __FIND_ALL_SOLUTIONS__
		printf("SAT:\n");
		for(int i = 1; i <= dimacs_nb_unary_vars(); i++)
			printf("%d", xorgauss_assignment[i]);
		printf("\nnb_activation = %lld\n",nb_activation);
		printf("conf:%lld\n", conf[0]);
		return false;
#endif
		return true;
	}
#ifdef __DEBUG__
	printf("\nSetting:%d\n",set[l]);
	for(int i = 1; i <= dimacs_nb_unary_vars(); i++)
		printf("%d", xorgauss_assignment[i]);
	printf("\n");
#endif
	if(!_cnf_is_undef(set[l])) return wdsat_solve_rest_XG(l + 1, nb_min_vars, conf, d + 1);
	#ifndef NO_CNF
		_cnf_breakpoint;
	#endif
	#ifdef TEST_SUBST
		_substitution_breakpoint;
	#endif
	_xorset_breakpoint;
	_xorgauss_breakpoint;
	conf[0]++;
	if(!wdsat_infer(-set[l],conf,d))
	{
		#ifndef NO_CNF
			cnf_undo();
		#endif
		#ifdef TEST_SUBST
			substitution_undo();
		#endif
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
			#ifndef NO_CNF
				cnf_undo();
			#endif
			#ifdef TEST_SUBST
				substitution_undo();
			#endif
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
			#ifndef NO_CNF
				_cnf_mergepoint;
			#endif
			#ifdef TEST_SUBST
				_substitution_mergepoint;
			#endif
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
		return false;
	}
#ifdef __DEBUG__
	for(int i = 1; i <= dimacs_nb_unary_vars(); i++)
		printf("%d", xorgauss_assignment[i]);
	printf("\n");
#endif

	return wdsat_solve_rest_XG(l + 1, nb_min_vars, conf, d + 1);
}

bool wdsat_infer(const int_t l, int_t conf[], int_t d) {
	bool _loop_pass = true;
	bool _continue;
	int_t cnf_history_it;
	int_t cnf_history_last = cnf_history_top;
	int_t xorgauss_history_it;
	int_t xorgauss_history_last = xorgauss_history_top;
	int_t _l;

	if (d > d_arbre) d_arbre=d;
	// printf("%d\n",d);

	if(!wdsat_set_true(l)) return false;
	// xorgauss_fprint_nb_equationxor();

	// xorgauss_count_nb_var_nb_equation();

	// if (xorgauss_count_xorequation >= xorgauss_count_nb_var_xor) printf("%lld %lld\n",xorgauss_count_xorequation,xorgauss_count_nb_var_xor);

	// if ( xorgauss_count_xorequation == xorgauss_count_nb_var_xor){
	// if ( (d >= K1 && d <= K2) || d >= K3 ){
	// if ( (d >= K1 && d <= K2)){



	// if ( d >= K3 ){
		nb_activation++;

		while(_loop_pass) {
			// finalyse with XORGAUSS
			_continue = false;
			cnf_history_it = cnf_history_top;
			while(cnf_history_it > cnf_history_last) {
				_l = cnf_history[--cnf_history_it];
				if(_xorgauss_is_undef(_l)) {
					if(!xorgauss_set_true(_l)) return false;
					_continue = true;
				}
			}
			cnf_history_last = cnf_history_top;
			_loop_pass = false;
			if(_continue) {
				// get list of literal set thanks to XORGAUSS
				xorgauss_history_it = xorgauss_history_top;
				while(xorgauss_history_it > xorgauss_history_last) {
					_l = xorgauss_history[--xorgauss_history_it];
					if(_cnf_is_false(_l)) return false;
					if(_cnf_is_undef(_l)) {
						_loop_pass = true;
						if(!wdsat_set_true(_l)) return false;
					}
				}
				xorgauss_history_last = xorgauss_history_top;
			}
		}
	// }
	// if(d == 1){
	// 	xorgauss_fprint_for_xorset();
	// 	exit(0);
	// }
	return true;
}

/// @fn solve();
/// @return false if formula is unsatisfiable and true otherwise
bool wdsat_solve(int_t n, int_t new_l, int_t new_m, char *irr, char *X3, int_t xg, char mvc_graph[1000], char thread[1000],int S) {
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

	// wdsat_infer_unitary();

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

	// xorgauss_count_nb_var_nb_equation();

	// xorgauss_fprint_for_xorset();
	// xorgauss_fprint();

	// xorset_fprint();
	// xorset_index_structure_fprintf();
	// exit(5);

	// dimacs_print_formula();
	// dimacs_print_equivalency();
	// dimacs_print_table();

	#ifdef SMALL_TEST // Une zone de test
		/*srand( time( NULL ) );
		int tab[26];
		int stop = true;
		while(stop == true){
			printf("--- New gen ---\n");
			int searchedValue = rand() % 101;

			tab[0]=-1;
			for (int i=1; i<26; i++){
				tab[i]=searchedValue % 2;
				searchedValue = rand() % 101;
			}

			// int tab[26]={-1,1,1,0,1,0,0,1,0,1,0,0,0,1,0,1,1,1,0,0,1,0,1,0,0,0};
			// int tab[26]={-1,1,1,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1,0,1,1,1,1,1};

			for (int i=1; i<26; i++){
				int val = (tab[i] == 1) ? i : -i;
				if (i != 1)
					substitution_increase_history_flag();
				if(substitution_set_true(val) == true) printf("Success %d\n",val);
				else {printf("Failure %d\n",val);stop=false;}

			}
			substitution_reset_dynamic_table();
			substitution_reset_stack();
		}*/

		// for (int i =1; i <=25; i++){
		// 	if (i != 1)
		// 		substitution_increase_history_flag();
		// 	int val=i;
		// 	if(substitution_set_true(val) == true) printf("Success %d\n",val);
		// 	else printf("Failure %d\n",val);
		// }

		_substitution_breakpoint;
		int val;
		val=1;
		if(substitution_set_true(val) == true) printf("Success %d\n",val);
		else printf("Failure %d\n",val);

		val=3;
		if(substitution_set_true(val) == true) printf("Success %d\n",val);
		else printf("Failure %d\n",val);

		val=5;
		if(substitution_set_true(val) == true) printf("Success %d\n",val);
		else printf("Failure %d\n",val);
		
		// substitution_fprint_dynamic_values();
		// substitution_fprint_substitution_index_stack();

		// substitution_testing_vars(true);

		_substitution_breakpoint;
		val=6;
		substitution_increase_history_flag();
		if(substitution_set_true(val) == true) printf("Success %d\n",val);
		else printf("Failure %d\n",val);

		substitution_fprint_history_main_stack();
		substitution_undo();
		substitution_fprint_history_main_stack();
		// substitution_testing_vars(false);
		// system("/bin/bash script/testing_vars/main.sh");

		substitution_free_structure();
		return true;
	#endif


	int_t d=0;
	clock_t debut = clock();
	if(xg == 0)
	{
		if(!wdsat_solve_rest(0, nb_min_vars - 1, conf)) {
			printf("UNSAT\n");
			printf("%ld\n",conf[0]);
			printf("Avant\n");
			substitution_free_structure();
			printf("Après\n");
			return false;
		}
		// substitution_free_structure();
	}
	if(xg == 1)
	{
		
		if(!wdsat_solve_rest_XG(0, nb_min_vars - 1, conf, 0)) {
			printf("UNSAT\n");
			printf("nb_activation = %ld\n",nb_activation);
			printf("conf:%ld\n",conf[0]);

			clock_t fin = clock();
			int duree_ml = 1000*(fin-debut)/CLOCKS_PER_SEC;

			if (S == 1) save_result(duree_ml,conf);
			
			substitution_free_structure();
			return false;
		}
	}
	
	printf("Cnf assignment : ");
	for(j = 1; j <= dimacs_nb_unary_vars(); j++)
	{
		printf("%d", cnf_assignment[j]);
	}
	printf("\n");

	printf("Sub assignment : ");
	for(j = 1; j <= dimacs_nb_unary_vars(); j++)
	{
		printf("%d", substitution_assignment[j]);
	}
	printf("\n");
	
	printf("conf:%ld\n",conf[0]);

	substitution_free_structure();
	return (true);

}
