#include "client.h"

#define SERV_PORT 2222
struct sockaddr_in serv_addr;



int main( int argc, char *argv[] ){
    int sockfd;
    sockfd=socket(AF_INET,SOCK_STREAM,0);


    /*Remplir la structure serv_addr avec l'adresse du serveur*/
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons((unsigned short)atoi(argv[2]));
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }
    else
    {
        printf("Connected\n");
    }

    /*Permettre d'écrire des messages indéfiniment jusqu'à fermeture de la connexion*/
    char buffer[256];
    int n;
    bzero(buffer,256);
    while(1)
    {
        printf("Please enter the message: ");
        fgets(buffer,255,stdin);
        n = write(sockfd,buffer,strlen(buffer));
        if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
        }
        bzero(buffer,256);
        n = read(sockfd,buffer,255);
        if (n < 0) {
            perror("ERROR reading from socket");
            exit(1);
        }
        printf("%s \n",buffer);

    }
}