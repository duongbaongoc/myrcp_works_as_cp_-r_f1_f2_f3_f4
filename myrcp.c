/* This program works exactly as Linux command: cp -r f1 f2
 * which copies f1 to f2, where f1 and f2 are REG, LNK, or 
 * DIR files
 */

/* cp f1 f2 analysis
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
void chech_same_files(char *f1, char *f2);
void get_readpaths(char *f1, char *f2);
void cp_f2f(char *f1, char *f2);
int check_dir_contain(char *f1, char *d1);
void cp_f2d(char *f1, char *d1);
void cp_d2d(char *d1, char *d2);
void copy_content_d2d(char *d1, char *d2);

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
			 S_ISDIR(f1stat.st_mode) < 0)
	{
		printf("cp: '%s' is a special file: Cannot copy special files\n", f1);
		exit(1);
	}
	
	//validate f2
	if (stat(f2, &f2stat) == 0 &&
		S_ISREG(f1stat.st_mode) < 0 &&
		S_ISDIR(f1stat.st_mode) < 0)
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
	else //f1 is DIR
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

//exit the program if f1 and f2 are the same file
void chech_same_files(char *f1, char *f2)
{
	struct stat f1stats, f2stats;
	int r1s = stat(f1, &f1stats);
	int r2s = stat(f2, &f2stats); 
	//exit if f1 and f2 are the same
	if (r2s==0 && f1stats.st_dev==f2stats.st_dev &&
		f1stats.st_ino==f2stats.st_ino)
	{
		printf("cp: '%s' and '%s' are the same file\n", f1, f2);
		exit(1);
	}
}

//if f1 (or f2) is a LNK, change f1 (or f2) to readpath
void get_readpaths(char *f1, char *f2)
{
	struct stat f1stat, f2stat;
	int r1 = lstat(f1, &f1stat);
	int r2 = lstat(f2, &f2stat); 
	char path1[PATH_MAX+1], path2[PATH_MAX+1];
	//get real paths of all symbolic links
	if (S_ISLNK(f1stat.st_mode) == 1)
		f1 = realpath(f1, path1);
	if (r2 == 0 && S_ISLNK(f2stat.st_mode) == 1)
		f2 = realpath(f2, path2);
}
	
//f1 must exist and is REG/LNK_to_REG
//f2 may not exist and if exists, it is REG/LNK_to_REG
//copy f1 to f2
//refer to "cp f1 f2 analysis"
void cp_f2f(char *f1, char *f2)
{
	printf("enter cp_f2f\n");
	chech_same_files(f1, f2);
	get_readpaths(f1, f2);
	
	//open files
	int f1d = open(f1, O_RDONLY);
	int f2d = open(f2, O_WRONLY|O_TRUNC|O_CREAT, 0644);
	
	//copy f1 to f2
	int n, m;
	char buf[4096];
	while (n = read(f1d, buf, 4096))
		m = write(f2d, buf, n);
		
	//close files
	close(f1d);
	close(f2d);
}

//check if a directory contains a given file/directory
//return 1 it does, return 0 otherwise
int check_dir_contain(char *f1, char *d1)
{
	int flag = 0;
	char *f1_basename = basename(f1);
	struct dirent *ep;
	DIR *dp = opendir(d1);
	
	while (ep = readdir(dp))
	{
		if (strcmp(f1_basename, ep->d_name) == 0)
		{
			flag = 1;
			break;
		}
	}
	return flag;
}

//f1 must exist, d1 may or may not exist
//refer to "cp f1 f2 analysis"
void cp_f2d(char *f1, char *d1)
{
	printf("enter cp_f2d\n");
	int dir_contain = check_dir_contain(f1, d1);
	char *f2 = strcat(strcat(d1,"/"), basename(f1));
	struct stat f2stat;
	int r2 = stat(f2, &f2stat); 
	
	if (dir_contain == 0 | S_ISREG(f2stat.st_mode))
		cp_f2f(f1, f2);
	else //d2 contains a dir named basename(f1)
		cp_f2d(f1, f2);	
}

//f1 must exist, d2 may or may not exist
//refer to "cp f1 f2 analysis"
void cp_d2d(char *d1, char *d2)
{
	printf("enter cp_d2d\n");
	struct stat dstat;
	char d3[100];
	char temp[100];
	strcpy(temp, d2);
	
	//if d2 does not exist, make d2
	if (stat(d2, &dstat) < 0) 
	{
		mkdir(d2, ACCESSPERMS);
		strcpy(d3, d2);
	}
	else
	{
		strcpy(d3, strcat(strcat(temp,"/"), basename(d1)));
		//if d2 does not contain file named basename(d1), make one
		if (check_dir_contain(d1, d2) == 0)
			mkdir(d3, ACCESSPERMS);
	}
		
	//copy content of d1 over to d3
	copy_content_d2s(d1,d3);
}

//given d1 and d2 exist, copy content of d1 to d2
void copy_content_d2d(char *d1, char *d2)
{
	printf("inside copy_content_d2s()\n");
	struct stat dstat;
	struct dirent *ep;
	DIR *dp = opendir(d1);
	char d3[100];
		
	while (ep = readdir(dp))
	{
		//check type of each entry
		strcpy(d3,d1);
		strcat(d3, "/");
		strcat(d3, ep->d_name);
		printf("d3=%s\n",d3);
		stat(d3, &dstat);
		if (S_ISREG(dstat.st_mode) == 1)
		{
			printf("is reg\n");
		}
		else if (S_ISDIR(dstat.st_mode) == 1)
		{
			printf("is dir\n");
		}
		else
		{
			printf("cp: cannot copy special files\n");
			exit(1);
		}
	}
}

