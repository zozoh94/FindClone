/**
 * @author Enzo Hamelin 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defs.h"

struct predicate* insert_predicate(ptr_function_bool pred_func) {
	struct predicate* pred;
	if(predicates == NULL) {
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
	last_pred->next = NULL;
	last_pred->no_default_print = false;
	return last_pred;
}

void free_predicates() {
	struct predicate* pred = predicates;
	struct predicate* pred_next;
	while(pred != NULL) {
		pred_next = pred->next;
		free(pred);
		pred = pred_next;
	}	
}

bool no_default_print() {
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
		if(strchr("-", argv[i][0]) == NULL)
			usage("les chemins doivent précéder l'expression", argv[i]);
		predicate_name = argv[i];
		parse_function = find_parser(predicate_name);
		if (parse_function == NULL) {
		        fprintf(stderr, "%s : prédicat inconnu « %s »\n", program_name, predicate_name);
			exit(EXIT_FAILURE);
		}
		i++;
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
	if(!no_default_print(predicates)) {
		parse_print(argv, &i);
	}
	struct predicate* pred = predicates;
	while(pred != NULL) {
		pred = pred->next;
	}

	return presence_path;
}

void usage (char *msg, char* pathname) {
	if (msg)
		fprintf (stderr, "%s: %s : %s\n", program_name, msg, pathname);
	fprintf (stderr, "Usage: %s [path...] [expression]\n", program_name);
	exit(EXIT_FAILURE);
}
