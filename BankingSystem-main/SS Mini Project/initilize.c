#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
void main()
{
	int fd;
	int count=0;
	creat("count.txt",0777);
	fd=open("count.txt",O_WRONLY);
	write(fd,&count,sizeof(count));//number of accounts
	lseek(fd,sizeof(count),SEEK_SET);
	write(fd,&count,sizeof(count));//number of transactions
	close(fd);
	creat("accounts.txt",0777);
	creat("transactions.txt",0777);
}
