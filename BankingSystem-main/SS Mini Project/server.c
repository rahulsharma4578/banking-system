#include<sys/types.h>  
#include<sys/socket.h> 
#include<netinet/ip.h> 
#include<stdio.h>      
#include<unistd.h> 
#include<stdlib.h>
#include "serverOptions.h"
int main()
{
    int sfd, cfd,status,csize;
    struct sockaddr_in address, client;
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

    status = bind(sfd, (struct sockaddr *)&address, sizeof(address));
    if (status == -1)
    {
        perror("Error in binding");
        exit(0);
    }
    status = listen(sfd, 2);
    if (status== -1)
    {
        perror("Error in  listening");
        exit(0);
    }
    
    while (1)
    {
        csize = (int)sizeof(client);
        cfd = accept(sfd, (struct sockaddr *)&client, &csize);
        if (cfd == -1)
            perror("Error in accepting!");
        else
        {
            if (!fork())
            {
                while(1)
                {
                    status = read(cfd,&option,sizeof(option));//options 1: admin login , 2:user login 
                    switch(option)
                        {
                            case 1: 
                                serverAdmin(cfd);// is are imported from serverOptions.h
                                break;
                            
                            case 2: 
                                serverUser(cfd);// is are imported from serverOptions.h
                                break;
                            case 3: 
                                exit(0);// exit applicaiton condition
                            default: 
                                break;
                        }
                }
		     
            }
            else
                close(cfd);
        }
    }

    close(sfd);
}