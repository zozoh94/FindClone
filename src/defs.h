/**
 * Definition commune au fichier c du FindClone
 *
 * @author Enzo Hamelin
 * @author Amirali Ghazi
 */

#include <stdbool.h>
#include <sys/stat.h>

typedef bool (*ptr_function_bool)();

struct predicate* insert_predicate(ptr_function_bool pred_func);
void apply_predicates(char* pathname, struct stat* file_stat);
void free_predicates();
ptr_function_bool find_parser (char *name);
char* find_name(ptr_function_bool function);
void usage (char *msg, char* pathname);
bool parse(char *argv[], int argc);

bool pred_type(char* pathname, struct stat* file_stat, struct predicate* info);
bool pred_true(char* pathname, struct stat* file_stat, struct predicate* info);
bool pred_false(char* pathname, struct stat* file_stat, struct predicate* info);
bool pred_print(char* pathname, struct stat* file_stat, struct predicate* info);
bool pred_ls(char* pathname, struct stat* file_stat, struct predicate* info);
bool pred_uid(char* pathname, struct stat* file_stat, struct predicate* info);
bool pred_gid(char* pathname, struct stat* file_stat, struct predicate* info);

bool parse_print(char *argv[], int *arg_ptr);

struct predicate* predicates;
struct predicate* last_pred;

char* program_name;

struct predicate {
	char*  name;
	union {
		char* str;
		mode_t type;
		long val;
	} args;
	ptr_function_bool function;
	struct predicate* next;
	bool no_default_print; //Si true le print par défaut ne doit pas etre exécuté
	bool execute; //Si true c'est un predicat qui execute une action
};
