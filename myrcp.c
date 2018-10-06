/* This program works exactly as Linux command: cp -r f1 f2
 which copies f1 to f2, where f1 and f2 are REG, LNK, or DIR files*/

#include <stdio.h>	//for printf()
#include <stdlib.h>	//for exit()
#include <string.h>	//for strcpy(), strcmp()
#include <libgen.h>	//for basename(), dirname()
#include <fcntl.h>	//for open(), close(), read(), write()
#include <sys/stat.h>	//for stat()
#include <unistd.h>	
#include <sys/types.h>	//for opendir(), readdir()
#include <dirent.h>

void copy_files(int argc, char *argv[]);
void copy_f2f(char *f1, char *f2);


int main(int argc, char *argv[])
{
	if (argc==1)
		printf("cp: missing file operand\n");
	else if (argc==2)
		printf("cp: missing destination file operand after '%s'\n", argv[1]);
	else
		copy_files(argc, argv);
	return 0;
}


//cp f1 f2 f3 ... f5 will copy f1, f2, f3, ... to f5
void copy_files(int argc, char *argv[])
{
	int i;
	for (i=1; i<argc-1; i++)
	{
		copy_f2f(argv[i], argv[argc-1]);
	}
}

//copy f1 to f2
void copy_f2f(char *f1, char *f2)
{
	printf("f1=%s, f2=%s\n", f1, f2);
}
