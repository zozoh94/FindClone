/**
 * Fonction des predicats du FindClone
 *
 * Par Amirali Ghazi et Enzo Hamelin
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
#include <sys/wait.h>
#include <fnmatch.h>
#include <libgen.h>
#include "defs.h"

#define SECPERDAYS 86400

/* Structure associant un predicat à son nom */
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
	{pred_user, "user"},
	{pred_group, "group"},
	{pred_ctime, "ctime"},
	{pred_mtime, "mtime"},
	{pred_atime, "atime"},
	{pred_exec, "exec"},
	{pred_name, "name"},
	{pred_perm, "perm"},
	{pred_prune, "prune"},
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
	(void)pathname;
	return((file_stat->st_mode & S_IFMT) == info->args.type);
}

bool pred_true(char* pathname, struct stat* file_stat, struct predicate* info) {
	(void)pathname;
	(void)(file_stat);
	(void)(info);
	return true;
}

bool pred_false(char* pathname, struct stat* file_stat, struct predicate* info) {
	(void)pathname;
	(void)(file_stat);
	(void)(info);
	return false;
}

bool pred_print(char* pathname, struct stat* file_stat, struct predicate* info) {
	(void)(file_stat);
	(void)(info);
	puts(pathname);
	return true;
}

bool pred_ls(char* pathname, struct stat* file_stat, struct predicate* info) {
	(void)(info);
	printf("%6d %4d ", (int)file_stat->st_ino, (int)file_stat->st_blocks);
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

	printf(" %3d ", (int)file_stat->st_nlink);

	struct passwd* user;
	struct group* group;
	
	user = getpwuid(file_stat->st_uid);
	if (user)
		printf("%-8s ", user->pw_name);
	else
		printf("%-8d ", file_stat->st_uid);
       
	group = getgrgid(file_stat->st_gid);
	if(group)
		printf("%-8s ", group->gr_name);
	else
		printf("%-8d ", file_stat->st_gid);
	
	printf("%8d ", (int)file_stat->st_size);

	struct tm const *when_local = localtime(&file_stat->st_mtime);
	char time[18];
	strftime(time,18,"%Y-%m-%d %H:%M", when_local);
	printf("%s ", time);
	
	printf("%s ", pathname);

	char symbolic[2048];
	int len;
	//Si le fichier est un lien symbolique
	if(S_ISLNK(file_stat->st_mode)) {
		if ((len = readlink(pathname, symbolic, sizeof(symbolic)-1)) != -1)
			symbolic[len] = '\0';
		printf("-> %s", symbolic);
	}
	
	printf("\n");
	return true;
}

bool pred_uid(char* pathname, struct stat* file_stat, struct predicate* info) {
	(void)pathname;
	if (file_stat->st_uid == info->args.val)
		return true;
	return false;
}

bool pred_gid(char* pathname, struct stat* file_stat, struct predicate* info) {
	(void)pathname;
	if (file_stat->st_gid == info->args.val)
		return true;
	return false;
}

bool pred_user(char* pathname, struct stat* file_stat, struct predicate* info) {
	(void)pathname;
	struct passwd* user = getpwnam(info->args.str);
	if (user && file_stat->st_uid == user->pw_uid)
		return true;
	return false;
}

bool pred_group(char* pathname, struct stat* file_stat, struct predicate* info) {
	(void)pathname;
	struct group* group = getgrnam(info->args.str);
	if (group && file_stat->st_gid == group->gr_gid)
		return true;
	return false;
}

bool pred_ctime(char* pathname, struct stat* file_stat, struct predicate* info) {
	(void)pathname;
        time_t comp = (time(NULL)-file_stat->st_ctime)/(SECPERDAYS);
        int diff = (int)difftime(comp, info->args.time.val);
	if(info->args.time.comp == EQUAL && diff == 0)
		return true;
	else if(info->args.time.comp == GREATER_THAN && diff > 0)
		return true;
	else if(info->args.time.comp == LOWER_THAN && diff < 0)
		return true;
	return false;
}

bool pred_atime(char* pathname, struct stat* file_stat, struct predicate* info) {
	(void)pathname;
        time_t comp = (time(NULL)-file_stat->st_atime)/(SECPERDAYS);
        int diff = (int)difftime(comp, info->args.time.val);
	if(info->args.time.comp == EQUAL && diff == 0)
		return true;
	else if(info->args.time.comp == GREATER_THAN && diff > 0)
		return true;
	else if(info->args.time.comp == LOWER_THAN && diff < 0)
		return true;
	return false;
}

bool pred_mtime(char* pathname, struct stat* file_stat, struct predicate* info) {
	(void)pathname;
        time_t comp = (time(NULL)-file_stat->st_mtime)/(SECPERDAYS);
        int diff = (int)difftime(comp, info->args.time.val);
	if(info->args.time.comp == EQUAL && diff == 0)
		return true;
	else if(info->args.time.comp == GREATER_THAN && diff > 0)
		return true;
	else if(info->args.time.comp == LOWER_THAN && diff < 0)
		return true;
	return false;
}

bool pred_exec(char* pathname, struct stat* file_stat, struct predicate* info) {
	(void)(file_stat);
	pid_t child;
	if( info->args.args != NULL && info->args.args[0] != NULL) {
		child = fork(); //On fork pour lancer le execvpx
		if(child == -1)
			return EXIT_FAILURE;

		char *acc = NULL;
		int place_acc;
		for(int i = 0; info->args.args[i] != NULL; i++) {
			if(strcmp(info->args.args[i], "{}") == 0) {
				acc = info->args.args[i];
				place_acc = i;
				info->args.args[i] = pathname;
			}
		}
	
		if(child == 0) {
			if(execvp(info->args.args[0], info->args.args) == -1)
				exit(EXIT_FAILURE);
		} else {
			int result;
			waitpid(child, &result, WUNTRACED); //On attend le fils
			if(acc != NULL)
				info->args.args[place_acc] = acc;
		}	
		return true;
	} else 
		return false;
}

bool pred_name(char* pathname, struct stat* file_stat, struct predicate* info) {
	(void)(file_stat);
	if(!fnmatch(info->args.str, basename(pathname), 0))
		return true;
	else
		return false;
}

bool pred_perm(char* pathname, struct stat* file_stat, struct predicate* info) {
	(void)(pathname);
        if(info->args.perm.type == EXACT &&
		info->args.perm.val[0] == ((file_stat->st_mode & S_IRWXU)>>6) &&
		info->args.perm.val[1] == ((file_stat->st_mode & S_IRWXG)>>3) &&
		info->args.perm.val[2] == ((file_stat->st_mode & S_IRWXO)))
		return true;
	else if(info->args.perm.type == AT_LEAST &&
		info->args.perm.val[0] <= ((file_stat->st_mode & S_IRWXU)>>6) &&
		info->args.perm.val[1] <= ((file_stat->st_mode & S_IRWXG)>>3) &&
		info->args.perm.val[2] <= ((file_stat->st_mode & S_IRWXO)))
		return true;
	else if(info->args.perm.type == ANY && (
		info->args.perm.val[0]<<6 & file_stat->st_mode ||
		info->args.perm.val[1]<<3 & file_stat->st_mode ||
		info->args.perm.val[2] & file_stat->st_mode ||
		(info->args.perm.val[0] == 0 && info->args.perm.val[1] == 0 && info->args.perm.val[2] == 0)))
		return true;
	return false;
}

bool pred_prune(char* pathname, struct stat* file_stat, struct predicate* info) {
	(void)(pathname);
	(void)(info);
	if(S_ISDIR(file_stat->st_mode))
		stop_at_current_level = true;
	return true;
}
