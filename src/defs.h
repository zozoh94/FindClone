/**
 * Definition commune au fichier c du FindClone
 *
 * Par Amirali Ghazi et Enzo Hamelin
 */

#include <stdbool.h>
#include <sys/stat.h>

//Definition d'un pointeur sur une fonction retournant un booleen
typedef bool (*ptr_function_bool)();

/* Permet d'inserer un predicat dans la liste chainée */
struct predicate* insert_predicate(ptr_function_bool pred_func);
/* Permet d'appliquer les predicats sur un chemin donnée*/
void apply_predicates(char* pathname, struct stat* file_stat);
/* Permet de liberer la mémoire alloué sur le tas */
void free_predicates();
/* Permet de trouver le parseur correspondant à un élément de la ligne de commande donné */
ptr_function_bool find_parser (char *name);
/* Permet de trouver le nom d'un predicat à partir de son nom */
char* find_name(ptr_function_bool function);
/* Permet d'afficher comment le logiciel doit etre utilisé */
void usage (char *msg, char* pathname);
/* Permet de parser tout les arguments */
bool parse(char *argv[], int argc);

/*
 * Différents prédicats prenant en paramètre le chemin du fichier à afficher si besoin,
 * la structure stat associée et la structure predicat associée
 */
bool pred_type(char* pathname, struct stat* file_stat, struct predicate* info);
bool pred_true(char* pathname, struct stat* file_stat, struct predicate* info);
bool pred_false(char* pathname, struct stat* file_stat, struct predicate* info);
bool pred_print(char* pathname, struct stat* file_stat, struct predicate* info);
bool pred_ls(char* pathname, struct stat* file_stat, struct predicate* info);
bool pred_uid(char* pathname, struct stat* file_stat, struct predicate* info);
bool pred_gid(char* pathname, struct stat* file_stat, struct predicate* info);
bool pred_ctime(char* pathname, struct stat* file_stat, struct predicate* info);
bool pred_exec(char* pathname, struct stat* file_stat, struct predicate* info);

/* Fonction de parsage du print dispo depuis l'exterieur 
 * car on peut forcer l'utilisation de ce predicat */
bool parse_print(char *argv[], int *arg_ptr);

/* Debut de la liste chainée de predicat */
struct predicate* predicates;
/* Dernier predicat crée */
struct predicate* last_pred;

/* Nom du programme argv[0] */
char* program_name;

/* Enumeration des types de comparaison (utiles pour certains predicats) */
enum comparison_type
{
	GREATER_THAN,
	LOWER_THAN,
	EQUAL
};

/* Structure chainée initialisée lors de l'analyse syntaxique */ 
struct predicate {
	char*  name; //Nom du predicat
	//L'uninon permet d'initialiser les arguments du predicats de differents manière
	union {
		char* str;
		mode_t type;
		long val;
		char** args; //Arguments pour les execvp
	} args;
	bool args_set; //Si true args de l'union est initialisé
	enum comparison_type comp; //Type de comparaison
	ptr_function_bool function; //Pointeur sur la fonction correspondante au predicat
	struct predicate* next; //Pointeur sur le predicat suivant
	bool no_default_print; //Si true le print par défaut ne doit pas etre exécuté
	bool execute; //Si true c'est un predicat qui execute une action
};
