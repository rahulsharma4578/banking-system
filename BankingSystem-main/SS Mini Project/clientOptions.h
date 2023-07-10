#include<string.h>
#include<time.h>
struct userAccount
	{
		int id;
		int userCount;
		int isActive;
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
void addNewAccount(int sfd)
{
	struct names
	{
		char user1[10];
		char user2[10];
	};
	char c[4];
	int id,status;
	struct names newnames;
	//printf("Do you want to create a joint account[Yes/No]:\n");
	printf("Type \"Yes\" to make joint account, press any other key for single user account\n");
	scanf("%s",c);
	while((getchar())!='\n');
	if(strcmp("Yes",c)==0)
	{
	
		printf("Enter Primary user name:\n");
		scanf("%s",newnames.user1);
		while((getchar())!='\n');
		printf("Enter secondary user name:\n");
		scanf("%s",newnames.user2);
		while((getchar())!='\n');
		status=write(sfd,&newnames,sizeof(newnames));
		status=read(sfd,&id,sizeof(id));
		printf("Account created with account id:%d\n",id);
		printf("User id for %s is:P %d\n",newnames.user1,id);
		printf("User id for %s is:S %d\n",newnames.user2,id);
	}
	else
	{
		printf("Enter user name:\n");
		scanf("%s",newnames.user1);
		while((getchar())!='\n');
		strcpy(newnames.user2,"None");
		status=write(sfd,&newnames,sizeof(newnames));
		status=read(sfd,&id,sizeof(id));
		printf("Account created with account id:%d\n",id);
		printf("User id for %s is:P %d\n",newnames.user1,id);
	}
	
}
void deactivateAccount(int sfd)
{	
	int accountid,status;
	printf("Enter the account id which needs to be deactivated:\n");
	scanf("%d",&accountid);
	while((getchar())!='\n');
	status=write(sfd,&accountid,sizeof(accountid));
	read(sfd,&status,sizeof(status));
	if(status==1)
		printf("Account Deactivated\n");
	else	
		printf("Invalid request\n");
}
void viewAccount(int sfd,int accountid)
{
	int status;
	struct userAccount account;
	if(accountid==-1)
	{
		printf("Enter the account id which needs to be viewed:\n");
		scanf("%d",&accountid);
		while((getchar())!='\n');
	}
	status=write(sfd,&accountid,sizeof(accountid));
	status=read(sfd,&account,sizeof(account));
	read(sfd,&status,sizeof(status));
	if(status==1)
	{
		printf("-----------------\n");
		if(account.userCount==1)
		{
			if(account.isActive==1)
				printf("AccountId:%d\nUser Name:%s\nAccount Type:Active Account\nBalance:%d\n",account.id,account.user1,account.balance);
			else
				printf("AccountId:%d\nUser Name:%s,\nAccount Type:Inactive Account\nBalance:0\n",account.id,account.user1);
		}
		else
		{
			if(account.isActive==1)
				printf("AccountId:%d\nPrimary User Name:%s\nSecondary User Name:%s\nAccount type:Active Account\nBalance:%d\n",account.id,account.user1,account.user2,account.balance);
			else
				printf("AccountId:%d\nPrimary User Name:%s\nSecondary User Name:%s\nAccount type:Inactive Account\nBalance:0\n",account.id,account.user1,account.user2);

		}
		printf("-----------------\n");
	}
	else
	{
		printf("Invalid request\n");
	}
}
void modifyAccount(int sfd)
{
	
	int accountid,status;
	struct userAccount account;
	char newname[10];
	printf("Enter the account id which needs to be modified:\n");
	scanf("%d",&accountid);
	status=write(sfd,&accountid,sizeof(accountid));
	status=read(sfd,&account,sizeof(account));
	if(account.id==accountid)
	{
		printf("Enter new primary user name,[Enter '-' if you don't want to change]\n");
		scanf("%s",newname);
		while((getchar())!='\n');
		if(strcmp(newname,"-")!=0)
		{
			strcpy(account.user1,newname);
		}
		printf("Enter new name of secondary user/Add new secondary user,[Enter '-' if you don't want to change]\n");
		scanf("%s",newname);
		while((getchar())!='\n');
		if(strcmp(newname,"-")!=0)
		{
			account.userCount=2;
			strcpy(account.user2,newname);
			strcpy(account.pass2,newname); //bug 1 fixed
		}
		status=write(sfd,&account,sizeof(account));
		printf("Modifications completed\n");
	}
	else
	{
		printf("Invalid request\n");
		status=write(sfd,&account,sizeof(account));
	}
}
void viewTransactions(int sfd,int id)
{
	struct transaction t;
	int isValid,fd,i;
	int counter=0;
	write(sfd,&id,sizeof(id));
	while(1)
	{
		
		read(sfd,&isValid,sizeof(int));
		read(sfd,&t,sizeof(t));
		read(sfd,&i,sizeof(i));
		if(isValid==1)
		{
		counter=counter+1;
		printf("-----------------\n");
        printf("Transaction ID:%d\n",i+1);
        printf("Account No:%d\n",t.accountid);
        if(t.usertype==0)
            printf("User Id:P %d\n",t.accountid);
        else if(t.usertype==1)
            printf("User Id:S %d\n",t.accountid);
        else
            printf("User Id:ADMIN_ACCESS\n");
        if(t.transactiontype==0)
        {
            if(t.oldbalance<t.newbalance)
            	printf("Transaction Type:Deposit\n");
            else
            	printf("Transaction Type:Withdrawal\n");
            	
         }
        else
            printf("Transaction Type:Account Deactivation\n");
        printf("Old Balance:%d\n",t.oldbalance);
        printf("New Balance:%d\n",t.newbalance);
        printf("Time of Transaction:%s\n",ctime(&t.time));
		}
		else if(isValid==-1)
		{	printf("-----------------\n");
			printf("Number of Transactions:%d\n",counter);
			printf("-----------------\n");
			return;
		}

	}
	printf("-----------------\n");
}
void clientAdmin(int sfd)
{
	char pass[10];
	int status,isValid;
	int choice;
	printf("ENTER ADMIN PASSWORD TO CONTINUE:\n");
	scanf("%s",pass);
	while((getchar())!='\n');
	status=write(sfd,pass,8);
    	status=read(sfd,&isValid,sizeof(isValid));//isValid =0 if password is correct
    	if (isValid==0)
    	{
    		while(1)
			{
				printf("Press ENTER to continue");
				getchar();
				system("clear");	
				printf(" -------CHOOSE YOUR OPTION-------\n");
				printf(" 1.ADD NEW ACCOUNT\n");
				printf(" 2.DELETE/DEACTIVATE ACCOUNT\n");
				printf(" 3.VIEW ACCOUNT DETAILS\n");
				printf(" 4.MODIFY ACCOUNT DETAILS\n");
				printf(" 5.VIEW ALL TRANSACTIONS\n");
				printf(" 6.LOGOUT AND EXIT APPLICATION\n");
				printf(" 7.LOGOUT AND RETURN TO MAIN MENU\n");
				scanf("%d",&choice);
				while((getchar())!='\n');
				status=write(sfd,&choice,sizeof(choice));
				switch(choice)
				{
					case 1: 
						addNewAccount(sfd);
						break;
					
					case 2: 
						deactivateAccount(sfd);
						break;
					case 3: 
						viewAccount(sfd,-1);//-1 means the account-id will be asked inside the program...
						break;
					case 4: 
						modifyAccount(sfd);
						break;
					case 5: 
						viewTransactions(sfd,-1); //id=-1 means we consider all transactions ,insted 
						//of only 1 id's transactions.
						break;

					case 6: 
							exit(0);
					case 7: 
							return;

					default:
							printf("Invalid Request\n");
						
				}
			}
    		
    	}
    	else
    	{
    		printf("password is incorrect\n");
    		return;
    	}
 }	
void changePassword(int sfd,int id,int usertype)
{
	int status;
	char pass[10];
	struct userAccount account;
	status=write(sfd,&id,sizeof(id));
	printf("Enter new password:\n");
	scanf("%s",pass);
	while((getchar())!='\n');
	status=read(sfd,&account,sizeof(account));
	if(usertype==0)// if  primary user
	{
		strcpy(account.pass1,pass);
	}
	else// if secondary user
	{
		strcpy(account.pass2,pass);
	}
	status=write(sfd,&account,sizeof(account));
	printf("Password changed sucessfully!!\n");
}
void modifyBalance(int sfd,int id,int usertype,int transactiontype)
{
	int amount;
	int status,curbalance;
	int isActive;
	printf("Enter the amount to withdraw/deposit\n");
	char str[20];
	read(0,str,20);
	amount=atoi(str);
	while((getchar())!='\n');
	//scanf("%d",&amount);
	//while((getchar())!='\n');
	//printf("Amount entered is:%d\n",amount);
	status=write(sfd,&usertype,sizeof(usertype));
	status=write(sfd,&id,sizeof(id));
	status=read(sfd,&curbalance,sizeof(curbalance));
	status=read(sfd,&isActive,sizeof(isActive));
	if(amount>0 && curbalance+(transactiontype*amount)>0 && isActive==1)
	{
		printf("Press 1 to confirm transaction or 0 to cancle\n");
		scanf("%d",&status);
		while((getchar())!='\n');
		if (status==1)
		{
			curbalance=curbalance+(transactiontype*amount);
			status=write(sfd,&curbalance,sizeof(curbalance));
			printf("Transaction sucessful\n");
		}
		else
		{
			status=write(sfd,&curbalance,sizeof(curbalance));
			printf("Transaction cancelled\n");
		}
	}
	else
	{
		status=write(sfd,&curbalance,sizeof(curbalance));
		printf("Transaction is not valid maybe due to : \n \ta.Insufficient Funds \n \tb.Bad Request \n \tc.Account Deactivated by Admin*\n");
	}
}

void clientUser(int sfd)
{
	char pass[10];
	int status,isValid;
	int usertype;
	int choice;
	char p[2];
	int id;
	struct userAccount account;
	printf("ENTER USER ID:");
	scanf("%s %d",p,&id);
	while((getchar())!='\n');
	printf("ENTER USER PASSWORD:");
	scanf("%s",pass);
	while((getchar())!='\n');
	status=write(sfd,&id,sizeof(id));
	status=read(sfd,&account,sizeof(account));
	usertype=strcmp(p,"P");//usertype=0 for primary user
	int live;
	if (usertype==0)
	{
		isValid=strcmp(account.pass1,pass);
		isValid=isValid+account.isLive1;// is isLive1 is 1 then isValid!=0 so will not enter account
	}
	else
	{
		isValid=strcmp(account.pass2,pass);
		isValid=isValid+account.isLive2;
	}
	write(sfd,&isValid,sizeof(isValid));
	if (isValid==0)
	{
		write(sfd,&usertype,sizeof(usertype));
		while(1)
		{
			printf("Press ENTER to continue");
			getchar();
			system("clear");
			printf(" -------CHOOSE YOUR OPTION-------\n");
			printf(" 1.VIEW ACCOUNT DETAILS\n");
			printf(" 2.CHANGE PASSWORD\n");
			printf(" 3.DEPOSIT MONEY\n");
			printf(" 4.WITHDRAW MONEY\n");
			printf(" 5.VIEW ACCOUNT TRANSACTIONS\n");
			printf(" 6.LOGOUT AND EXIT APPLICATION\n");
			printf(" 7.LOGOUT AND RETURN TO MAIN MENU\n");
			char str[20];
			read(0,str,20);
			choice=atoi(str);
			status=write(sfd,&choice,sizeof(choice));
			switch(choice)
			{
				case 1: 
					viewAccount(sfd,id);
					break;
				
				case 2: 
					changePassword(sfd,id,usertype);
					break;
				
				case 3: 
					modifyBalance(sfd,id,usertype,1);//+1 to indicate deposit
					break;
				case 4: 
					modifyBalance(sfd,id,usertype,-1);//-1 to indicate withdraw
					break;
				case 5: 
					viewTransactions(sfd,id);
					break;
				case 6:
					exit(0);
				case 7: 
					return;
				default: 
					printf("Invalid Request \n");
					
			}
		}
		
	}
	else
	{
		//choice=7;//return to main menu
		//status=write(sfd,&choice,sizeof(int));//for the server the quite ,choice 7 is nothing but return to main initial menu	
		printf("INVALID CREDENTIALS OR USER HAS ALREADY LOGGED IN\n");
		return ;
	}	
}
