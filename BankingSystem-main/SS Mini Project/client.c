#include<sys/types.h>  
#include<sys/socket.h> 
#include<netinet/ip.h> 
#include<stdio.h>      
#include<unistd.h> 
#include<stdlib.h>
#include"clientOptions.h"
int main()
{
    int sfd,status;
    struct sockaddr_in address;
    int option;
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        perror("Error in creating socket");
        exit(0);
    }
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_family = AF_INET;
    address.sin_port = htons(8080);

    status = connect(sfd, (struct sockaddr *)&address, sizeof(address));
    if (status == -1)    
    {
        perror("Error in connecting");
        exit(0);
    }
    while(1)
    {
        printf("PRESS ENTER TO CONTINUE");
        getchar();
        system("clear");
        printf(" -------WELCOME TO ADITYA BANK-------\n");
        printf(" CHOOSE YOUR OPTION\n");
        printf(" 1. ADMIN LOGIN\n");
        printf(" 2. USER LOGIN\n");
        printf(" 3. EXIT APPLICATION\n");
        scanf("%d",&option);
        while((getchar())!='\n');
        switch(option)
        {
            case 1: 
                status=write(sfd,&option,sizeof(option));
                if (status==-1)
                {
                    perror("Error in writing");
                    exit(0);
                }
                else
                {
                    clientAdmin(sfd);//is imported from clientOptions.h
                    break;
                }
            
            case 2: 
                status=write(sfd,&option,sizeof(option));
                if (status==-1)
                {
                    perror("Error in writing");
                    exit(0);
                }
                else
                {
                    clientUser(sfd);//is imported from clientOptions.h
                    break;
                }   
            case 3: 
                    exit(0); 	
            default: 
                    printf("Invalid option\n");
                    break;
        }
    }
    close(sfd);
}