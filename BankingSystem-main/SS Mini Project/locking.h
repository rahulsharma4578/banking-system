#include<fcntl.h>
#include<sys/file.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
void writeLock(int fd,int fromwhere, int length)
{
    struct flock lock;
    lock.l_type=F_WRLCK;
    lock.l_whence=SEEK_SET;
    lock.l_start=fromwhere;
    lock.l_len=length;
    lock.l_pid=getpid();
    fcntl(fd,F_SETLKW,&lock);

}

void unlock(int fd,int fromwhere,int length)
{
    struct flock lock;
    lock.l_type=F_UNLCK;
    lock.l_whence=SEEK_SET;
    lock.l_start=fromwhere;
    lock.l_len=length;
    lock.l_pid=getpid();
    fcntl(fd,F_SETLK,&lock);
}

void readLock(int fd,int fromwhere, int length)
{
    struct flock lock;
    lock.l_type=F_RDLCK;
    lock.l_whence=SEEK_SET;
    lock.l_start=fromwhere;
    lock.l_len=length;
    lock.l_pid=getpid();
    fcntl(fd,F_SETLKW,&lock);

}
