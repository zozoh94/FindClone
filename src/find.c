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

static  locale_t locale;

void process_top_path(char* pathname);
void process_path(char* pathname);
void process_dir(char* pathname, char* name);

int main(int argc, char *argv[])
{
	locale = newlocale(LC_CTYPE_MASK|LC_NUMERIC_MASK|LC_TIME_MASK|
			LC_COLLATE_MASK|LC_MONETARY_MASK|LC_MESSAGES_MASK,
			"",(locale_t)0);
	
        if (locale == (locale_t)0) {
		return EXIT_FAILURE;
        }
	
	if(argc < 2)
		process_top_path(".");
	
	process_top_path(argv[1]);
	
	return EXIT_SUCCESS;
}

void process_top_path(char* pathname)
{
	char *path_process;
 	if(pathname[strlen(pathname)-1] == '/') 
		path_process = pathname;
	else {
		path_process = malloc(strlen(pathname)+1);
		memcpy(path_process, pathname, strlen(pathname));
		path_process[strlen(pathname)] = '/';
	}
	if(chdir(path_process) != -1)
		process_path(path_process);
	else {  
		if(errno == EACCES) printf("%s\n", pathname);
		fprintf(stderr,"find: \"%s\": %s\n", pathname, strerror_l(errno, locale));
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
					printf("%s%s\n", pathname, file->d_name);
					if(S_ISDIR(stat_file.st_mode) && !S_ISLNK(stat_file.st_mode))
						process_dir(pathname, file->d_name);
				}
			}
		}
	}
	closedir(dir);	
}

void process_dir(char* pathname, char* name)
{
	int length = (strlen(name)+strlen(pathname)+2);
	char* path = malloc(length);					
	if ( path == NULL ) {
		errno = ENOMEM;
		fprintf(stderr,"%s \n", strerror_l(errno, locale));
		exit(EXIT_FAILURE);
	}

	memcpy(path, pathname, strlen(pathname));
	memcpy(path+strlen(pathname), name, strlen(name));
	path[length-2] = '/';
	path[length-1] = '\0'; 

	DIR *dir = opendir(path);
	if(dir == NULL)
		fprintf(stderr,"find: \"%s\": %s\n", path, strerror_l(errno, locale));
	else
		process_path(path);

	closedir(dir);
	free(path);
}
