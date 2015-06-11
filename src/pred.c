/**
 * Fonction des predicats du FindClone
 *
 * @author Enzo Hamelin
 * @author Amirali Ghazi
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include "defs.h"

struct pred_assoc {
  ptr_function_bool function;
  char *name;
};

struct pred_assoc pred_table[] = {
	{pred_type, "type"},
	{pred_true, "true"},
	{pred_false, "false"},
	{pred_print, "print"},
	{pred_ls, "ls"},
	{pred_uid, "uid"},
	{pred_gid, "gid"},
	{0, "none "}
};

char* find_name(ptr_function_bool function){
	int i;
	for (i = 0; pred_table[i].function != 0; i++)
		if (pred_table[i].function == function)
			break;
	return (pred_table[i].name);
}

bool pred_type(char* pathname, struct stat* file_stat, struct predicate* info) {
	return((file_stat->st_mode & S_IFMT) == info->args.type);
}

bool pred_true(char* pathname, struct stat* file_stat, struct predicate* info) {
	return true;
}

bool pred_false(char* pathname, struct stat* file_stat, struct predicate* info) {
	return false;
}

bool pred_print(char* pathname, struct stat* file_stat, struct predicate* info) {
	puts(pathname);
	return true;
}

bool pred_ls(char* pathname, struct stat* file_stat, struct predicate* info) {
	printf("%6d %4d ", file_stat->st_ino, file_stat->st_blocks);
	switch(file_stat->st_mode & S_IFMT) {
	case S_IFBLK:		
		printf("b");
		break;
	case S_IFCHR:		
		printf("c");
		break;
	case S_IFDIR:			
		printf("d");
		break;
	case S_IFREG:		
		printf("-");
		break;
	case S_IFLNK:		
		printf("l");
		break;
	case S_IFIFO:		
		printf("f");
		break;
	case S_IFSOCK:		
		printf("s");
		break;
	default:
		printf(" ");
	}
	if(file_stat->st_mode & S_IRUSR)
		printf("r");
	else
		printf("-");
	if(file_stat->st_mode & S_IWUSR)
		printf("w");
	else
		printf("-");
	if(file_stat->st_mode & S_IXUSR)
		printf("x");
	else
		printf("-");
	if(file_stat->st_mode & S_IRGRP)
		printf("r");
	else
		printf("-");
	if(file_stat->st_mode & S_IWGRP)
		printf("w");
	else
		printf("-");
	if(file_stat->st_mode & S_IXGRP)
		printf("x");
	else
		printf("-");
	if(file_stat->st_mode & S_IROTH)
		printf("r");
	else
		printf("-");
	if(file_stat->st_mode & S_IWOTH)
		printf("w");
	else
		printf("-");
	if(file_stat->st_mode & S_IXOTH)
		printf("x");
	else
		printf("-");

	printf(" %3d ", file_stat->st_nlink);

	char const *user_name;
	char const *group_name;
	
	user_name = getpwuid(file_stat->st_uid)->pw_name;
	if (user_name)
		printf("%-8s ", user_name);
	else
		printf("%-8d ", file_stat->st_uid);

	group_name = getgrgid(file_stat->st_gid)->gr_name;
	if(group_name)
		printf("%-8s ", group_name);
	else
		printf("%-8d ", file_stat->st_gid);

	printf("%8d ", file_stat->st_size);

	struct tm const *when_local = localtime(&file_stat->st_mtime);
	char time[18];
	strftime(time,18,"%Y-%m-%d %H:%M", when_local);
	printf("%s ", time);

	printf("%s ", pathname);

	char symbolic[1024];
	int len;
	if(S_ISLNK(file_stat->st_mode)) {
		if ((len = readlink(pathname, symbolic, sizeof(symbolic)-1)) != -1)
			symbolic[len] = '\0';
		printf("-> %s", symbolic);
	}
	
	printf("\n");
	return true;
}

bool pred_uid(char* pathname, struct stat* file_stat, struct predicate* info) {
	if (file_stat->st_uid == info->args.val)
		return true;
	return false;
}

bool pred_gid(char* pathname, struct stat* file_stat, struct predicate* info) {
	if (file_stat->st_gid == info->args.val)
		return true;
	return false;
}
