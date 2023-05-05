#include "client.h"

#define SERV_PORT 2222
struct sockaddr_in serv_addr;

int main(int argc, char *argv[])
{
    if (argc < 4) {
        printf("Usage : %s <ip> <port> <level>\n", argv[0]);
        exit(1);
    }
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    int level = atoi(argv[3]);

    /*Remplir la structure serv_addr avec l'adresse du serveur*/
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons((unsigned short)atoi(argv[2]));
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {

        perror("ERROR connecting");
        exit(1);
    }
    else
    {
        printf("[Level %d] : Connected\n", level);
        char *str = malloc(sizeof(char *));
        sprintf(str, "!%d", level);
        printf("Sending : %s\n", str);
        send(sockfd, str, sizeof(str), 0);
        //printf("[Level %d] : Please enter the message : \n", level);
    }

    /*Permettre d'écrire des messages indéfiniment jusqu'à fermeture de la connexion*/
    char buffer[256];
    int n;
    bzero(buffer, 256);
    while (1)
    {
        /* printf("[Level %d] : Please enter the message: ", level);
        fgets(buffer,255,stdin);

        size_t len = strlen(buffer);
        memmove(buffer + 1, buffer, ++len);
        *buffer = level+'0';
        //printf("Buffer : %s", buffer);
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
        printf("%s \n",buffer); */

        // le client peut recevoir un message du serveur à tout moment, même pendant un fgets
        // il faut donc utiliser select pour pouvoir recevoir un message du serveur
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);
        int maxfd = sockfd;
        if (STDIN_FILENO > maxfd) // si le stdin est plus grand que le socket
            maxfd = STDIN_FILENO;
        select(maxfd + 1, &readfds, NULL, NULL, NULL);

        if (FD_ISSET(STDIN_FILENO, &readfds)) // si le stdin est prêt à être lu, c'est ce client qui parle
        {
            //TODO : Ce n'est pas au client de dire ça, si ? On pourrait faire en sorte que le serveur affiche les "Please enter the message" et "Voici la réponse" pour contrôler l'ordre
            
            fgets(buffer, 255, stdin);

            size_t len = strlen(buffer);
            memmove(buffer + 1, buffer, ++len); // ça rajoute un caractère au début du buffer
            *buffer = level + '0'; // on met le niveau du client au début du buffer
            // printf("Buffer : %s", buffer);
            n = write(sockfd, buffer, strlen(buffer));
            if (n < 0)
            {
                perror("ERROR writing to socket");
                exit(1);
            }
        }
        if (FD_ISSET(sockfd, &readfds)) // si le socket est prêt à être lu
        {
            bzero(buffer, 256);
            n = read(sockfd, buffer, 255);
            if (n < 0)
            {
                perror("ERROR reading from socket");
                exit(1);
            }
            printf("%s \n", buffer); //message de la personne avec qui ce client est connecté
        }
        
    }
}
