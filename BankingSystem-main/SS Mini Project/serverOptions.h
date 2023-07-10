#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include"locking.h"
#include<time.h>
struct userAccount
	{
		int id;
		int userCount;
		int isActive; // 1 if active , 0 if inactive
		int balance;
		char user1[10];
		char pass1[10];
		char user2[10];
		char pass2[10];
		int isLive1; //0 user 1 is not live, 1 user 1 is live
		int isLive2; 
	};
struct transaction
	{
		int accountid;
		int usertype;//0 for primary , 1 for secondary , -1 for ADMIN_ACCESS
		int transactiontype;//0 for withdrawal/deposit, -1 for deactivation
		int oldbalance;
		int newbalance;
		time_t time;
	};
void addNewAccount(int cfd)
{
	struct names
	{
		char user1[10];
		char user2[10];
	};
	struct names newnames;
	struct userAccount newaccount;
	int status;
	int fromwhere,length;
	int count,countfd,accountfd;
	status=read(cfd,&newnames,sizeof(newnames));
	strcpy(newaccount.user1,newnames.user1);
	strcpy(newaccount.user2,newnames.user2);
	strcpy(newaccount.pass1,newnames.user1);
	strcpy(newaccount.pass2,newnames.user2);
	newaccount.balance=0;
	newaccount.isLive1=0;
	newaccount.isLive2=0;
	countfd=open("count.txt",O_RDWR);
	fromwhere=0;
	length=sizeof(int);
	writeLock(countfd,fromwhere,length);/* lock */
	status=read(countfd,&count,sizeof(count));
	count=count+1;
	lseek(countfd,0,SEEK_SET);
	status=write(countfd,&count,sizeof(count));
	unlock(countfd,fromwhere,length);/*unlock*/
	close(countfd);
	newaccount.id=count;
	newaccount.isActive=1;
	if(strcmp(newaccount.user2,"None")==0)
	{
		newaccount.userCount=1;
	}
	else
	{
		newaccount.userCount=2;
	}
	accountfd=open("accounts.txt",O_RDWR);
	fromwhere=(count-1)*sizeof(newaccount);
	length=sizeof(newaccount);
	writeLock(accountfd,fromwhere,length);/*lock*/
	lseek(accountfd,(count-1)*sizeof(newaccount),SEEK_SET);
	status=write(accountfd,&newaccount,sizeof(newaccount));
	unlock(accountfd,fromwhere,length);/*unlock*/
	close(accountfd);
	status=write(cfd,&count,sizeof(count));//sending account id created back to client program
	
}
void deactivateAccount(int cfd,char* pass)//after account is deactivated password is set as Admin password
{
	struct transaction t;
	int transactionfd;
	struct userAccount account;
	int status,accountid;
	int accountfd;
	int oldbalance=0;
	int count,countfd;
	int numberoftransactions;
	status=read(cfd,&accountid,sizeof(accountid));
	accountfd=open("accounts.txt",O_RDWR);
	int fromwhere,length;
	fromwhere=(accountid-1)*sizeof(account);
	length=sizeof(account);
	writeLock(accountfd,fromwhere,length);/*lock*/
	lseek(accountfd,(accountid-1)*sizeof(account),SEEK_SET);
	status=read(accountfd,&account,sizeof(account));
	if(accountid==account.id)
	{
		account.isActive=0;
		strcpy(account.pass1,pass);
		strcpy(account.pass2,pass);
		oldbalance=account.balance;
		printf("AccountID:%d,UserID:ADMIN_ACCESS,Transaction Type:DEACTIVATION,Old Balance:%d,New Balance:0\n",accountid,oldbalance);
		lseek(accountfd,(accountid-1)*sizeof(account),SEEK_SET); 
		account.balance=0;
		status=write(accountfd,&account,sizeof(account));
		countfd=open("count.txt",O_RDWR);
		int a,b;
		a=sizeof(int);
		b=sizeof(int);
		writeLock(countfd,a,b);/*lock*/
		lseek(countfd,sizeof(int),SEEK_SET);
		status=read(countfd,&count,sizeof(count));
		t.accountid=accountid;
		t.usertype=-1;
		t.transactiontype=-1;
		t.oldbalance=oldbalance;
		t.newbalance=0;
		t.time=time(NULL);
		transactionfd=open("transactions.txt",O_RDWR);
		int c,d;
		c=count*sizeof(t);
		d=sizeof(t);
		writeLock(transactionfd,c,d);/*lock*/
		lseek(transactionfd,count*sizeof(t),SEEK_SET);
		status=write(transactionfd,&t,sizeof(t));
		unlock(transactionfd,c,d);/*unlock*/
		close(transactionfd);
		count=count+1;
		lseek(countfd,sizeof(int),SEEK_SET);
		status=write(countfd,&count,sizeof(count));
		unlock(countfd,a,b);/*unlock*/
		close(countfd);
		status=1;
		write(cfd,&status,sizeof(status));
	}
	else
	{
		status=0;
		write(cfd,&status,sizeof(status));
	}
	unlock(accountfd,fromwhere,length);/*unlock*/
}
void viewAccount(int cfd)
{
	struct userAccount account;
	int status,accountid;
	int accountfd;
	status=read(cfd,&accountid,sizeof(accountid));
	accountfd=open("accounts.txt",O_RDWR);
	lseek(accountfd,(accountid-1)*sizeof(account),SEEK_SET);
	int fromwhere,length;
	fromwhere=(accountid-1)*sizeof(account);
	length=sizeof(account);
	readLock(accountfd,fromwhere,length);/*lock*/
	status=read(accountfd,&account,sizeof(account));
	unlock(accountfd,fromwhere,length);/*unlock*/
	close(accountfd);
	status=0;
	if(account.id==accountid)
		status=1;
	write(cfd,&account,sizeof(account));
	write(cfd,&status,sizeof(status));
}
void modifyAccount(int cfd)
{
	struct userAccount account;
	int status,accountid;
	int accountfd;
	status=read(cfd,&accountid,sizeof(accountid));
	accountfd=open("accounts.txt",O_RDWR);
	lseek(accountfd,(accountid-1)*sizeof(account),SEEK_SET);
	int fromwhere,length;
	fromwhere=(accountid-1)*sizeof(account);
	length=sizeof(account);
	writeLock(accountfd,fromwhere,length);/*lock*/
	status=read(accountfd,&account,sizeof(account));
	status=write(cfd,&account,sizeof(account));
	status=read(cfd,&account,sizeof(account));
	lseek(accountfd,(accountid-1)*sizeof(account),SEEK_SET);
	status=write(accountfd,&account,sizeof(account));
	unlock(accountfd,fromwhere,length);/*unlock*/
	close(accountfd);
}
void viewTransactions(int cfd)
{
	int id,isValid;
	int fd,countfd,count;
	struct transaction t;
	int i;
	read(cfd,&id,sizeof(id));
	fd=open("transactions.txt",O_RDWR);
    countfd=open("count.txt",O_RDWR);
	int a,b;
	a=sizeof(int);
	b=sizeof(int);
	readLock(countfd,a,b);/*lock*/
    lseek(countfd,sizeof(int),SEEK_SET);
    read(countfd,&count,sizeof(count));
	unlock(countfd,a,b);/*unlock*/
	readLock(fd,0,0);/*lock*/
    for(i=0;i<count;i++)
    {
        read(fd,&t,sizeof(t));
		isValid=0;
		if(id==-1 || t.accountid==id)
			isValid=1;
		write(cfd,&isValid,sizeof(isValid));
		write(cfd,&t,sizeof(t));
		write(cfd,&i,sizeof(i));
    }
	unlock(fd,0,0);/*unlock*/
	isValid=-1;
	write(cfd,&isValid,sizeof(isValid));
	write(cfd,&t,sizeof(t));
	write(cfd,&i,sizeof(i));
}
void serverAdmin(int cfd)
{
	int status,isValid,choice;
	char* pass="aditya";//password is hardcoded inside the program for simplicity
	char attempt[10];
	status = read(cfd,&attempt,8);//read password entered by user
        
	isValid=strcmp(pass,attempt);
	status=write(cfd,&isValid,sizeof(int));
	if(isValid==0)
	{	while(1)
		{
			status=read(cfd,&choice,sizeof(int));
			switch(choice)
			{
				case 1: addNewAccount(cfd);
					break;
				
				case 2: 
					deactivateAccount(cfd,pass);
					break;
				
				case 3: 
					viewAccount(cfd);
					break;
				case 4: 
					modifyAccount(cfd);
					break; 
				case 5: 
					viewTransactions(cfd);
					break;
				case 6: 
					exit(0);
				case 7:
					return;
				default: 
					break;
			}
		}
	}  
	else
	{
		return;
	} 	
        	
}
void changePassword(int cfd)
{
	int id,status,accountfd;
	struct userAccount account;
	status=read(cfd,&id,sizeof(id));
	accountfd=open("accounts.txt",O_RDWR);
	int fromwhere,length;
	fromwhere=(id-1)*sizeof(account);
	length=sizeof(account);
	writeLock(accountfd,fromwhere,length);/*lock*/
	lseek(accountfd,(id-1)*sizeof(account),SEEK_SET);
	status=read(accountfd,&account,sizeof(account));
	status=write(cfd,&account,sizeof(account));
	status=read(cfd,&account,sizeof(account));
	lseek(accountfd,(id-1)*sizeof(account),SEEK_SET);
	status=write(accountfd,&account,sizeof(account));
	unlock(accountfd,fromwhere,length);/*unlock*/
	close(accountfd);
}
void modifyBalance(int cfd)
{
	struct transaction t;
	int countfd,count,transactionfd;
	int status;
	int usertype,id,accountfd;
	struct userAccount account;
	int balance,newbalance;
	read(cfd,&usertype,sizeof(usertype));
	read(cfd,&id,sizeof(id));
	accountfd=open("accounts.txt",O_RDWR);
	int a,b;
	a=(id-1)*sizeof(account);
	b=sizeof(account);
	writeLock(accountfd,a,b);/*lock */
	lseek(accountfd,(id-1)*sizeof(account),SEEK_SET);
	status=read(accountfd,&account,sizeof(account));
	balance=account.balance;
	status=write(cfd,&balance,sizeof(balance));
	status=write(cfd,&account.isActive,sizeof(account.isActive));
	status=read(cfd,&newbalance,sizeof(balance));
	if(newbalance!=balance)
	{
		lseek(accountfd,(id-1)*sizeof(account),SEEK_SET);
		account.balance=newbalance;
		status=write(accountfd,&account,sizeof(account));
		if(usertype==0)
		{
			printf("AccountID:%d,UserID:P %d,Transaction Type:withdraw/deposit,Old Balance:%d,New Balance:%d\n",id,id,balance,newbalance);
		}
		else
		{
			printf("AccountID:%d,UserID:S %d,Transaction Type:withdraw/deposit,Old Balance:%d,New Balance:%d\n",id,id,balance,newbalance);
		}
		
		countfd=open("count.txt",O_RDWR);
		lseek(countfd,sizeof(int),SEEK_SET);
		int c,d;
		c=0;
		d=sizeof(count);
		writeLock(countfd,c,d);/*lock*/
		status=read(countfd,&count,sizeof(count));
		t.accountid=id;
		if(usertype==0)
			t.usertype=0;
		else	
			t.usertype=1;
		t.transactiontype=0;
		t.oldbalance=balance;
		t.newbalance=newbalance;
		t.time=time(NULL);
		transactionfd=open("transactions.txt",O_RDWR);
		int p,q; 
		p=count*(sizeof(t));
		q=sizeof(t);
		writeLock(transactionfd,p,q);/*lock*/
		lseek(transactionfd,count*sizeof(t),SEEK_SET);
		status=write(transactionfd,&t,sizeof(t));
		unlock(transactionfd,p,q);/*unlock*/
		close(transactionfd);
		count=count+1;
		lseek(countfd,sizeof(int),SEEK_SET);
		status=write(countfd,&count,sizeof(count));
		unlock(countfd,c,d);/*unlock*/
		close(countfd);
	}
	unlock(accountfd,a,b);/*unlock */
	close(accountfd);
}
void serverUser(int cfd)
{
	struct userAccount account;
	int status,id,accountfd,choice;
	status=read(cfd,&id,sizeof(id));
	accountfd=open("accounts.txt",O_RDWR);
	lseek(accountfd,(id-1)*sizeof(account),SEEK_SET);
	status=read(accountfd,&account,sizeof(account));
	status=write(cfd,&account,sizeof(account));
	int isValid;
	read(cfd,&isValid,sizeof(isValid));
	if(isValid==0)
	{
		int usertype;
		read(cfd,&usertype,sizeof(usertype));
		int fromwhere,length;
		fromwhere=(id-1)*sizeof(account);
		length=sizeof(account);
		writeLock(accountfd,fromwhere,length);/*lock*/
		lseek(accountfd,(id-1)*sizeof(account),SEEK_SET);
		read(accountfd,&account,sizeof(account));
		if(usertype==0)
		{
			account.isLive1=account.isLive1+1;
		}
		else
		{
			account.isLive2=account.isLive2+1;
		}
		lseek(accountfd,(id-1)*sizeof(account),SEEK_SET);
		write(accountfd,&account,sizeof(account));
		unlock(accountfd,fromwhere,length);/*unlock*/
		while(1)
		{
			status=read(cfd,&choice,sizeof(choice));
			switch(choice)
			{
				case 1:
						viewAccount(cfd);
						break;
				
				case 2: 
						changePassword(cfd);
						break;
				case 3: 
						modifyBalance(cfd);
						break;
				case 4: 
						modifyBalance(cfd);
						break;
				case 5: 
						viewTransactions(cfd);
						break;
				case 6:
						writeLock(accountfd,fromwhere,length);/*lock*/
						lseek(accountfd,(id-1)*sizeof(account),SEEK_SET);
						read(accountfd,&account,sizeof(account));
						if(usertype==0)
						{
							account.isLive1=account.isLive1-1;
						}
						else
						{
							account.isLive2=account.isLive2-1;
						}
						lseek(accountfd,(id-1)*sizeof(account),SEEK_SET);
						write(accountfd,&account,sizeof(account));
						unlock(accountfd,fromwhere,length);/*unlock*/
						exit(0);
				case 7: 
						//return to main menu
						writeLock(accountfd,fromwhere,length);/*lock*/
						lseek(accountfd,(id-1)*sizeof(account),SEEK_SET);
						read(accountfd,&account,sizeof(account));
						if(usertype==0)
						{
							account.isLive1=account.isLive1-1;
						}
						else
						{
							account.isLive2=account.isLive2-1;
						}
						lseek(accountfd,(id-1)*sizeof(account),SEEK_SET);
						write(accountfd,&account,sizeof(account));
						unlock(accountfd,fromwhere,length);/*unlock*/
						return;
				default: 
						break;
			}
		}
	}
	else
	{
		return;
	}
}