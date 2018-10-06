/* This program works exactly as Linux command: cp -r f1 f2
 * which copies f1 to f2, where f1 and f2 are REG, LNK, or 
 * DIR files
 */

/* cp f1 f2
 * 		if f1 does not exist -> error
 * case 1: file1 to file2
 *		if file2 does not exist -> creat file2, do the copy
 *		if file2 exists -> overwrite file2 by content of file1
 * case 2: file to dir
 *		if dir does not exist -> copy file to new file named dir
 *		id dir exists -> 
 *			if dir has the file of same name, overwrite the file
 *			if dir does not have file, make new file same name
 *				and copy content of the file over
 * case 3: dir1 to dir2
 *		if dir2 exists ->
 *			if dir2 has a dir(3) named dir1, add content of dir1
 *				into dir(3)
 *			if dir2 does not have dir1 yet, make a copy of dir1
 *				and let it stay inside dir2
 *		if dir2 does not exist -> mkdir dir2, make dir2 = dir1
 * case 4: dir to file -> error
*/



#include <stdio.h>	//for printf()
#include <stdlib.h>	//for exit()
#include <string.h>	//for strcpy(), strcmp()
#include <libgen.h>	//for basename(), dirname()
#include <fcntl.h>	//for open(), close(), read(), write()
#include <sys/stat.h>	//for stat()
#include <unistd.h>	
#include <sys/types.h>	//for opendir(), readdir()
#include <dirent.h>

void execute_cp(int argc, char *argv[]);
void validate_files(char *f1, char *f2);
void cp_1on1(char *f1, char *f2);
void cp_f2f(char *f1, char *f2);
void cp_f2d(char *f1, char *f2);
void cp_d2d(char *f1, char *f2);


int main(int argc, char *argv[])
{
	if (argc==1)
		printf("cp: missing file operand\n");
	else if (argc==2)
		printf("cp: missing destination file operand after '%s'\n", argv[1]);
	else
		execute_cp(argc, argv);
	return 0;
}


//cp f1 f2 f3 ... f5 will copy f1, f2, f3, ... to f5
void execute_cp(int argc, char *argv[])
{
	int i;
	for (i=1; i<argc-1; i++)
	{
		cp_1on1(argv[i], argv[argc-1]);
	}
}

//validate f1 and f2
//if f1 and f2 are not valid, exit the program
void validate_files(char *f1, char *f2)
{
	struct stat f1stat, f2stat;
	
	//validate f1
	if (stat(f1, &f1stat) < 0)
	{
		printf("cp: cannot stat '%s': No such file or directory\n", f1);
		exit(1);
	}
	else if (S_ISREG(f1stat.st_mode) < 0 &&
			 S_ISDIR(f1stat.st_mode) < 0 &&
			 S_ISLNK(f1stat.st_mode) < 0)
	{
		printf("cp: '%s' is a special file: Cannot copy special files\n", f1);
		exit(1);
	}
	
	//validate f2
	if (stat(f2, &f2stat) == 0 &&
		S_ISREG(f1stat.st_mode) < 0 &&
		S_ISDIR(f1stat.st_mode) < 0 &&
	    S_ISLNK(f1stat.st_mode) < 0)
	{
		printf("cp: '%s' is a special file: Cannot copy to special files\n", f2);
		exit(1);
	}
}


//copy f1 to f2
void cp_1on1(char *f1, char *f2)
{
	validate_files(f1, f2);
	struct stat f1stat, f2stat;
	stat(f1, &f1stat);
	
	if (S_ISREG(f1stat.st_mode) == 1) //f1 is REG
	{
		//f2 does not exist or is REG
		if (stat(f2, &f2stat) < 0 || 
			S_ISREG(f2stat.st_mode) == 1)
			cp_f2f(f1, f2);
		else //f2 exists and is DIR
			cp_f2d(f1, f2);
	}
	else if (S_ISDIR(f1stat.st_mode) == 1) //f1 is DIR
	{
		if (stat(f2, &f2stat) == 0 && //f2 is REG
			S_ISREG(f2stat.st_mode) == 1)
		{
			printf("cp: cannot overwrite non-directory '%s' with directory '%s'\n", f2, f1);
			exit(1);
		}
		else //f2 does not exist or f2 is DIR
			cp_d2d(f1, f2);
	}
}

void cp_f2f(char *f1, char *f2)
{
	printf("enter cp_f2f\n");
}

void cp_f2d(char *f1, char *f2)
{
	printf("enter cp_f2d\n");
}

void cp_d2d(char *f1, char *f2)
{
	printf("enter cp_d2d\n");
}

