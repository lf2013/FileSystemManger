#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <utime.h>
#include <fcntl.h>
int main(int argc, char * argv[])
{
	struct stat fileStat;
	if(argc < 2){
		printf("Error for lost argv\n");
		return 0;
	}
	stat(argv[1], &fileStat);

//	printf("st_mod%d\n", fileStat.st_mode);
	mode_t x = fileStat.st_mode;
//	printf(S_ISREG(x) ? "RG\n" : "NRG\n");//regular file : Not Regular
//	printf(S_ISDIR(x) ? "Dir\n" : "Not Dir\n");

//	printf("%d\n", S_IRUSR);
	printf("Infomation for %s\n-------------------\n", argv[1]);
	/* 输出文件属性 */
	printf(S_ISDIR(x) ? "d" : "-");
	printf(S_IRUSR & x ? "r" : "-"); //S_IRUSR 是常量不是宏
	printf(S_IWUSR & x ? "w" : "-");
	printf(S_IXUSR & x ? "x" : "-");
	printf(S_IRGRP & x ? "r" : "-");
	printf(S_IWGRP & x ? "w" : "-");
	printf(S_IXGRP & x ? "x" : "-");
	printf(S_IROTH & x ? "r" : "-");
	printf(S_IWOTH & x ? "w" : "-");
	printf(S_IXOTH & x ? "x" : "-");
	printf("\nsize:  %d Bytes\n", fileStat.st_size);

	time_t t = fileStat.st_mtime;
	//printf("Last modified at: %s\n", asctime(gmtime(&t)));
	printf("Last modified at: %s\n", ctime(&t)); //ctime  --> <time.h>
	
	if(argc == 3){
		if(strcmp(argv[1], "rm") == 0) //删除
			remove(argv[2]);
		else if(strcmp(argv[1], "mkdir") == 0) //建立与删除目录
			mkdir(argv[2], 0775);
		else if(strcmp(argv[1], "rmdir") == 0)
			rmdir(argv[2]);
		else
			rename(argv[1], argv[2]); //改名字

	}

	struct utimbuf tim;
	tim.actime = 12;
	tim.modtime = 123;
	utime(argv[1], &tim);
	t = fileStat.st_mtime;
	printf("Last modified at: %s\n", ctime(&t)); //ctime  --> <time.h>
									//	utime.actime | utime.modtime

	printf("O_CREAT = %d\t O_APPEND = %d\t O_RDONLY = %d\t O_WRONLY = %d\t O_RDWR = %d\n", O_CREAT, O_APPEND, O_RDONLY, O_WRONLY, O_RDWR);
	printf("O_EXCL = %d\t O_TRUNC = %d\t O_NOCTTY = %d\t O_NONBLOCK = %d\t\n", O_EXCL, O_TRUNC, O_NOCTTY, O_NONBLOCK); 
	printf("O_DSYNC = %d\t O_RSYNC = %d\t O_SYNC = %d\t\n", O_DSYNC, O_RSYNC, O_SYNC);
	
	return 0;
}

