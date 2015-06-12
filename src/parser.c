/**
 * Fonction de parsage des arguments de la ligne de commande du FindClone
 *
 * Par Amirali Ghazi et Enzo Hamelin
 */

#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "defs.h"

/* Différents prédicats prenant en arguments les arguments de la ligne de commande
   ainsi que l'index de départ dans argv de ces arguments*/
bool parse_type(char *argv[], int *arg_ptr);
bool parse_true(char *argv[], int *arg_ptr);
bool parse_false(char *argv[], int *arg_ptr);
bool parse_print(char *argv[], int *arg_ptr);
bool parse_ls(char *argv[], int *arg_ptr);
bool parse_uid(char *argv[], int *arg_ptr);
bool parse_gid(char *argv[], int *arg_ptr);
bool parse_ctime(char *argv[], int *arg_ptr);
bool parse_exec(char *argv[], int *arg_ptr);

struct parser_table {
	char* name;
	ptr_function_bool function;
};
	
static struct parser_table const parse_table[] = {
	{ "type", parse_type},
	{ "true", parse_true},
	{ "false", parse_false},
	{ "print", parse_print},
	{ "ls", parse_ls},
	{ "uid", parse_uid},
	{ "gid", parse_gid},
	{ "ctime", parse_ctime},
	{ "exec", parse_exec},
	{ 0, 0 }
};

ptr_function_bool find_parser (char *name)
{
	int i;
	if (name[0] == '-')
		name++;
	for (i = 0; parse_table[i].name != 0; i++)
		if (strcmp (parse_table[i].name, name) == 0)
			return (parse_table[i].function);
	return (NULL);
}

bool parse_type(char *argv[], int *arg_ptr) {
	mode_t type;
	struct predicate* pred;
        if(argv == NULL || argv[*arg_ptr] == NULL || strlen(argv[*arg_ptr]) != 1)
		return false;
	switch(argv[*arg_ptr][0]) {
	case 'b':		
		type = S_IFBLK;
		break;
	case 'c':		
		type= S_IFCHR;
		break;
	case 'd':			
		type = S_IFDIR;
		break;
	case 'f':		
		type = S_IFREG;
		break;
	case 'l':		
		type = S_IFLNK;
		break;
	case 'p':		
		type = S_IFIFO;
		break;
	case 's':		
		type = S_IFSOCK;
		break;
	default:
		return false;
	}
	pred = insert_predicate(pred_type);
	pred->args.type = type;
	(*arg_ptr)++; //On oublies pas d'incrementer arg_ptr pour que le parser n'interprete pas les arguments de parse_type
	return true;
}

bool parse_true(char *argv[], int *arg_ptr) {
	(void) argv;
	(void) arg_ptr;
	
	insert_predicate(pred_true);
	return true;
}

bool parse_false(char *argv[], int *arg_ptr) {
	(void) argv;
	(void) arg_ptr;
	
        insert_predicate(pred_true);
	return true;
}

bool parse_print(char *argv[], int *arg_ptr) {
	(void) argv;
	(void) arg_ptr;
	
	struct predicate* pred;
	pred = insert_predicate(pred_print);
	pred->execute = true;
	return true;
}

bool parse_ls(char *argv[], int *arg_ptr) {
	(void) argv;
	(void) arg_ptr;
	
	struct predicate* pred;
	pred = insert_predicate(pred_ls);
	pred->execute = true;
	pred->no_default_print = true;
	return true;
}

bool parse_uid(char *argv[], int *arg_ptr) {
	struct predicate* pred;
	long val;
	char *ptr;
	val = strtol(argv[*arg_ptr], &ptr, 10);
	if(errno == EINVAL)
		return false;
	pred = insert_predicate(pred_uid);
	pred->args.val = val;
	(*arg_ptr)++;
	return true;
}

bool parse_gid(char *argv[], int *arg_ptr) {
	struct predicate* pred;
	long val;
	char *ptr;
	val = strtol(argv[*arg_ptr], &ptr, 10);
	if(errno == EINVAL)
		return false;
	pred = insert_predicate(pred_gid);
	pred->args.val = val;
	(*arg_ptr)++;
	return true;
}

bool parse_ctime(char *argv[], int *arg_ptr) {
	struct predicate* pred;
	long val;
	char *ptr;
	char* number_str = argv[*arg_ptr]+sizeof(char);
	pred = insert_predicate(pred_ctime);
	switch(argv[*arg_ptr][0]) {
	case '+':
		pred->comp = GREATER_THAN;
		break;
	case '-':
		pred->comp = LOWER_THAN;
		break;
	default:
		number_str = argv[*arg_ptr];
	}
	val = strtol(number_str, &ptr, 10);
	if(errno == EINVAL)
		return false;
	pred->args.val = val;
	(*arg_ptr)++;
	return true;
}

bool parse_exec(char *argv[], int *arg_ptr) {
	struct predicate* pred;
	pred = insert_predicate(pred_exec);
	pred->args_set = true;
	pred->execute = true;
	int i;
	int len = 1;
	for(i = *arg_ptr; argv[i] != NULL && argv[i][0] != ';'; i++) {
		if(argv[i][0] == '-')
			return false;
		len++;
	}
	//On crée un tableau de chaines correspondant aux arguements de execvp
	char **args = malloc(len*sizeof(char*));
	if(args == NULL)
		exit(EXIT_FAILURE);	
	int j = 0;
	for(i = *arg_ptr; argv[i] != NULL && argv[i][0] != ';'; i++) {
		args[j] = malloc((strlen(argv[i])+1)*sizeof(char));
		if(args[j] == NULL)
			exit(EXIT_FAILURE);;
		args[j][strlen(argv[i])-1] = '\0';
		memcpy(args[j], argv[i], strlen(argv[i]));
		j++;
	}
	args[len-1] = NULL;
	pred->args.args = args;
	*arg_ptr += i;
	return true;
}
