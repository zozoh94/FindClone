/**
 * Fonction utlitaires mettant en relation les différentes structures du programmes
 * 
 * Par Amirali Ghazi et Enzo Hamelin 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"

struct predicate* insert_predicate(ptr_function_bool pred_func) {
	struct predicate* pred;
	if(predicates == NULL) { //Si on veux entrer le premier predicat
		predicates = malloc(sizeof(struct predicate));
		pred = predicates;
	}
	else {
		pred = malloc(sizeof(struct predicate));	
		last_pred->next = pred;
	}
	last_pred = pred;
	last_pred->function = pred_func; 
	last_pred->name = find_name(pred_func);
	last_pred->args.str = NULL;
	last_pred->args_set = false;
	last_pred->next = NULL;
	last_pred->no_default_print = false;
	return last_pred;
}

void apply_predicates(char* pathname, struct stat* file_stat) {
	//On applique un ET logique entre les différents prédicats
	//Encore beaucoup de travail sur cette fonction pour gérer les ou ...
	struct predicate* pred = predicates;
	bool ok = true;
	while(pred != NULL && ok) {	
	        ok = (*(pred->function))(pathname, file_stat, pred);
		pred = pred->next;
	}
}

void free_predicates() {
	struct predicate* pred = predicates;
	struct predicate* pred_next;
	while(pred != NULL) {
		pred_next = pred->next;
		if(pred->args.args != NULL && pred->args_set == true) {
			for(int i = 0; pred->args.args[i] != NULL; i++)
				free(pred->args.args[i]);
			free(pred->args.args);
		}
		free(pred);
		pred = pred_next;
	}	
}

bool no_default_print() {
	//On regardes si un predicat nous dit de ne pas faire de print pas défaut (ex: ls)
	struct predicate* pred = predicates;
	while(pred != NULL) {
		if(pred->no_default_print)
			return true;
		pred = pred->next;
	}
	return false;
}

bool parse(char *argv[], int argc) {
	predicates = NULL;
	last_pred = NULL;

	ptr_function_bool parse_function;
	char* predicate_name;

	bool presence_path = true;
	
	int i;
	//On cherche où les predicats commencent.
	for (i = 1; i < argc && strchr("-", argv[i][0]) == NULL; i++);
	if(i == 1)
		presence_path = false;
	
	while(i < argc) {
		// Si on trouve autre chose qu'une chaine commencant par -
		if(strchr("-", argv[i][0]) == NULL)
			usage("les chemins doivent précéder l'expression", argv[i]);
		predicate_name = argv[i];
		parse_function = find_parser(predicate_name);
		//Si le predicat est inconnu
		if (parse_function == NULL) {
		        fprintf(stderr, "%s : prédicat inconnu « %s »\n", program_name, predicate_name);
			exit(EXIT_FAILURE);
		}
		i++;
		//On applique le parsage des arguments de chaque predicat
		if (!(*parse_function) (argv, &i))
		{
			if (argv[i] == NULL) {
				fprintf(stderr, "%s : Parmètre manquant pour « %s »\n", program_name, predicate_name);
				exit(EXIT_FAILURE);
			}
			else {
				fprintf(stderr, "%s : Argument inconnu pour %s : %s\n", program_name, predicate_name, argv[i]);
				exit(EXIT_FAILURE);
			}
		}
	}
	//Maintenant notre liste de predicat est prete
	if(!no_default_print(predicates)) { // Si aucune fonction ne fait de l'affichage
		parse_print(argv, &i);
	}
	//Fonction de debugage pour afficher les predicats détecté
	/* struct predicate* pred = predicates; */
	/* while(pred != NULL) { */
	/* 	printf("%s\n", pred->name); */
	/* 	pred = pred->next; */
	/* } */

	return presence_path;
}

void usage (char *msg, char* pathname) {
	if (msg)
		fprintf (stderr, "%s: %s : %s\n", program_name, msg, pathname);
	fprintf (stderr, "Usage: %s [path...] [expression]\n", program_name);
	exit(EXIT_FAILURE);
}
