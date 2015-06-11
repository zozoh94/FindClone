/**
 * Programme principal du FindClone
 *
 * @author Enzo Hamelin
 * @author Amirali Ghazi
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

void process_top_path(char* pathname);
void process_path(char* pathname);
void process_dir(char* pathname);

void apply_predicates(char* pathname, struct stat* file_stat) {
	struct predicate* pred = predicates;
	bool ok = true;
	while(pred != NULL && ok) {	
	        ok = (*(pred->function))(pathname, file_stat, pred);
		pred = pred->next;
	}
}

int main(int argc, char *argv[])
{	
	locale = newlocale(LC_CTYPE_MASK|LC_NUMERIC_MASK|LC_TIME_MASK|
			LC_COLLATE_MASK|LC_MONETARY_MASK|LC_MESSAGES_MASK,
			"",(locale_t)0);
	
        if (locale == (locale_t)0) {	    
		return EXIT_FAILURE;
        }

	program_name = argv[0];

	bool path = parse(argv, argc);

	if(!path) 
		process_top_path(".");
	else
		process_top_path(argv[1]);
	
	free_predicates();
	
	return EXIT_SUCCESS;
}

void process_top_path(char* pathname)
{
	char *path_process;
	bool free_path = false;
 	if(pathname[strlen(pathname)-1] == '/') 
		path_process = pathname;
	else {
		free_path = true;
		path_process = malloc(strlen(pathname)+1);
		memcpy(path_process, pathname, strlen(pathname));
		path_process[strlen(pathname)] = '/';
	}
	if(chdir(path_process) != -1) {
		struct stat stat_file;
		if(lstat(pathname, &stat_file) != -1) {
			apply_predicates(pathname, &stat_file);
			if(!S_ISLNK(stat_file.st_mode))
				process_path(path_process);
		}
	}
	else {  
		if(errno == EACCES) {
			struct stat stat_file;
			if(lstat(path_process, &stat_file) != -1) {
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
	
	while((file = readdir(dir))) {
		if(strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
			char* name = file->d_name;
			int length = (strlen(name)+strlen(pathname)+2);
			char* path = malloc(length);					
			if ( path == NULL ) {
				errno = ENOMEM;
				fprintf(stderr,"%s \n", strerror_l(errno, locale));
				exit(EXIT_FAILURE);
			}
						
			memcpy(path, pathname, strlen(pathname));
			memcpy(path+strlen(pathname), name, strlen(name));
			path[length-2] = '\0';
			path[length-1] = '\0'; 
			if(lstat(path, &stat_file) != -1) {
				apply_predicates(path, &stat_file);
				if(S_ISDIR(stat_file.st_mode) && !S_ISLNK(stat_file.st_mode)) {
					path[length-2] = '/';
					path[length-1] = '\0'; 
					process_dir(path);
					
				}	       
			}
			free(path);
		}
	}
	closedir(dir);	
}

void process_dir(char* pathname)
{
	DIR *dir = opendir(pathname);
	if(dir == NULL)
		fprintf(stderr,"%s: \"%s\": %s\n", program_name, pathname, strerror_l(errno, locale));
	else
		process_path(pathname);

	closedir(dir);
}
