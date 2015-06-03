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

void process_top_path(char* pathname);
void process_path(char* pathname);
void process_dir(char* pathname, struct dirent* file);

int main(int argc, char *argv[])
{
	if(argc < 2)
		process_top_path(".");
	
	process_top_path(argv[1]);
	
	return EXIT_SUCCESS;
}

void process_top_path(char* pathname)
{
	char *path_print;
	char *path_process;
 	if(pathname[strlen(pathname)-1] == '/') {
		path_print = malloc(strlen(pathname)-1);
		memcpy(path_print, pathname, strlen(pathname)-1);
		path_process = pathname;
	}
	else {
		path_print = pathname;
		path_process = malloc(strlen(pathname)+1);
		memcpy(path_process, pathname, strlen(pathname));
		path_process[strlen(pathname)] = '/';
	}
	chdir(path_process);
	DIR *dir = opendir(path_process);
	if(dir != NULL) 
		process_path(path_process);
	else {
		printf("%s : Permission denied\n", path_print);
	}
}

void process_path(char* pathname)
{
	DIR *dir = opendir(pathname);
	struct dirent *file;
	struct stat stat_file;
	
	while((file = readdir(dir))) {
		if(chdir(pathname) != -1) {
			if(strcmp(file->d_name, ".") != 0 && strcmp(file->d_name, "..") != 0) {
				if(lstat(file->d_name, &stat_file) != -1) {
					printf("%s%s", pathname, file->d_name);
					if(S_ISDIR(stat_file.st_mode) && !S_ISLNK(stat_file.st_mode)) {
						process_dir(pathname, file);
					} else {
						printf("\n");
					}
				}
			}
		}
	}
	closedir(dir);	
}

void process_dir(char* pathname, struct dirent* file)
{
	int length = (strlen(file->d_name)+strlen(pathname)+2);
	char* path = malloc(length);					
	if ( path == NULL ) {
		fprintf(stderr,"Allocation error \n");
		exit(EXIT_FAILURE);
	}

	memcpy(path, pathname, strlen(pathname));
	memcpy(path+strlen(pathname), file->d_name, strlen(file->d_name));
	path[length-2] = '/';
	path[length-1] = '\0'; 

	DIR *dir = opendir(path);
	if(dir == NULL) {
		printf(" : Permission denied\n");
		printf("%s\n", path);
	} else {
		printf("\n");
		process_path(path);
	}

	closedir(dir);
	free(path);
}
