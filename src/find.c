/**
 * Programme principal du FindClone
 *
 * Par Amirali Ghazi et Enzo Hamelin
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <locale.h>
#include <stdbool.h>
#include "defs.h"

static  locale_t locale;

/* Recherche sur le premier repertoire */
void process_top_path(char* pathname);
/* Recherche sur repertoire */
void process_path(char* pathname);
/*  Permet de rappeler process_path sur les dossiers */
void process_dir(char* pathname);
/* Libere les ressources */
void error_mem();

/* Chemin du dossier parent */
char *path_process;

int main(int argc, char *argv[])
{
	//On initialise la locale pour recuperer les erreurs de errno dans la langue adéquate */
	locale = newlocale(LC_CTYPE_MASK|LC_NUMERIC_MASK|LC_TIME_MASK|
			LC_COLLATE_MASK|LC_MONETARY_MASK|LC_MESSAGES_MASK,
			"",(locale_t)0);
        if (locale == (locale_t)0) {	    
		return EXIT_FAILURE;
        }
	
	program_name = argv[0];
	current_level = 0;
	stop_at_current_level = false;
	mindepth = -1;
	maxdepth = -1;
	sort = false;
	
	//On parse tous les arguments
	bool path = parse(argv, argc);

	//Si le chemin n'est pas donné on execute le programme sur le repertoire courant
	if(!path) 
		process_top_path(".");
	else
		process_top_path(argv[1]);

	//On libere la memoire
	free_predicates();
	freelocale(locale);
	
	return EXIT_SUCCESS;
}

void process_top_path(char* pathname)
{
	bool free_path = false;
	//On ajoute un / à la fin du pathname si il n'en contient pas déjà un
 	if(pathname[strlen(pathname)-1] == '/') 
		path_process = pathname;
	else {
		free_path = true;
		path_process = malloc(strlen(pathname)+1);
		if(path_process == NULL) {
			error_mem();
			exit(EXIT_FAILURE);
		}
		memcpy(path_process, pathname, strlen(pathname));
		path_process[strlen(pathname)] = '/';
	}
	/*On se place dans le dossier de départ pour que les commandes suivantes comprennent
	  les chemins relatifs */
	if(chdir(path_process) != -1) { //Si l'ouverture s'est bien passé on execute process_path
		struct stat stat_file;
		if(lstat(pathname, &stat_file) != -1) {
			if(current_level > mindepth)
				apply_predicates(pathname, &stat_file);
			if(!S_ISLNK(stat_file.st_mode))
				process_path(path_process);
		}
	}
	else {  
		if(errno == EACCES) { //Si l'ouveture ne s'est pas bien passé et que c'est un problème de permissions
			struct stat stat_file;
			if(lstat(path_process, &stat_file) != -1 && current_level > mindepth) {
				apply_predicates(pathname, &stat_file);
			}
		}
		fprintf(stderr,"%s: \"%s\": %s\n", program_name, pathname, strerror_l(errno, locale));
	}
	if(free_path)
		free(path_process);
}

void process_path(char* pathname)
{
	DIR *dir = opendir(pathname);
	struct dirent *file;
	struct stat stat_file;
	char** files_sorted = NULL;

	stop_at_current_level = maxdepth >= 0 && current_level >= maxdepth;

	if(sort == false) {
		while((file = readdir(dir))) {
			// On ne prends pas en comptes les liens '.' et '..'
			if(strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
				char* name = file->d_name;
				int length = (strlen(name)+strlen(pathname)+2);
				char* path = malloc(length);					
				if ( path == NULL ) {
					closedir(dir);
					error_mem();
					exit(EXIT_FAILURE);
				}
				//On crée une chaine fusionnant le chemin et le fichier qu'on est en train de lire
				memcpy(path, pathname, strlen(pathname));
				memcpy(path+strlen(pathname), name, strlen(name));
				path[length-2] = '\0';
				path[length-1] = '\0'; 
				if(lstat(path, &stat_file) != -1) {
					apply_predicates(path, &stat_file);
					//Si le fichier est un dossier on lance process_dir
					if(!stop_at_current_level && S_ISDIR(stat_file.st_mode) && !S_ISLNK(stat_file.st_mode) && current_level > mindepth) {
						path[length-2] = '/';
						path[length-1] = '\0';
						process_dir(path);
					}	       
				}
				free(path);
			}
		}
	}
	else {
		int i = 0;
		while((file = readdir(dir))) {
			if(strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
				if(files_sorted == NULL)
					files_sorted = malloc((100)*sizeof(char*));
				else if(i > 99) {
					char **files_sorted_tmp = realloc(files_sorted, (i+1)*sizeof(char*));
					if(files_sorted_tmp == NULL || files_sorted != files_sorted_tmp) {
						closedir(dir);
						error_mem();
						free(files_sorted);
						exit(EXIT_FAILURE);
					}
					files_sorted = files_sorted_tmp;
				}
				if ( files_sorted == NULL ) {
					closedir(dir);
					error_mem();
					exit(EXIT_FAILURE);
				}
				
				char* name = file->d_name;
				int length = (strlen(name)+strlen(pathname)+2);
				files_sorted[i] = malloc(length*sizeof(char));

				if ( files_sorted[i] == NULL ) {
					errno = ENOMEM;
					fprintf(stderr,"%s \n", strerror_l(errno, locale));
					for(int j=0; j<i; j++)
						free(files_sorted[j]);
					free(files_sorted);
					exit(EXIT_FAILURE);
				}
				files_sorted[i][0] = '\0';

			        strcat(files_sorted[i], pathname);
				strcat(files_sorted[i], name);
				files_sorted[i][length-2] = '\0';
				files_sorted[i][length-1] = '\0'; 

				i++;
			}
		}
		//On trie les chemins
		if(files_sorted != NULL) {
			qsort(files_sorted, i, sizeof(char*), compare_pathname);
			//On parcourt les chemins
			for(int j=0; j<i; j++) {
				if(lstat(files_sorted[j], &stat_file) != -1) {
					apply_predicates(files_sorted[j], &stat_file);
					//Si le fichier est un dossier on lance process_dir
					if(!stop_at_current_level && S_ISDIR(stat_file.st_mode) && !S_ISLNK(stat_file.st_mode) && current_level > mindepth) {
						int length = strlen(files_sorted[j]);
						files_sorted[j][length] = '/';
						files_sorted[j][length+1] = '\0';
						process_dir(files_sorted[j]);
					}	       
				}
			}
			//On libere la memoire des chemins
			for(int j=0; j<i; j++)
				free(files_sorted[j]); 
			free(files_sorted);
		}
	}
	closedir(dir);	
}

void process_dir(char* pathname)
{
	DIR *dir = opendir(pathname);
	if(dir == NULL)
		fprintf(stderr,"%s: \"%s\": %s\n", program_name, pathname, strerror_l(errno, locale));
	else {
		++current_level;
		process_path(pathname);
		--current_level;
	}

	closedir(dir);
}

void error_mem() {
	//On libere la memoire
	errno = ENOMEM;
	fprintf(stderr,"%s \n", strerror_l(errno, locale));
	free_predicates();
	freelocale(locale);
	free(path_process);
}
