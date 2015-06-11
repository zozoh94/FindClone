/**
 * Fonction de parsage des arguments de la ligne de commande du FindClone
 *
 * @author Enzo Hamelin
 * @author Amirali Ghazi
 */

#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "defs.h"

bool parse_type(char *argv[], int *arg_ptr);
bool parse_true(char *argv[], int *arg_ptr);
bool parse_false(char *argv[], int *arg_ptr);
bool parse_print(char *argv[], int *arg_ptr);
bool parse_ls(char *argv[], int *arg_ptr);
bool parse_uid(char *argv[], int *arg_ptr);
bool parse_gid(char *argv[], int *arg_ptr);

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
	(*arg_ptr)++;
	return true;
}

bool parse_true(char *argv[], int *arg_ptr) {
	(void) argv;
	(void) arg_ptr;
	
	struct predicate* pred;
	pred = insert_predicate(pred_true);
	return true;
}

bool parse_false(char *argv[], int *arg_ptr) {
	(void) argv;
	(void) arg_ptr;
	
	struct predicate* pred;
	pred = insert_predicate(pred_true);
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
