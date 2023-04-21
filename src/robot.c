#include "robot.h"

#define SERV_PORT 2222
struct sockaddr_in serv_addr;
struct sockaddr_in cli_addr;



char *dialog(char* message)
{
    char* response = "";
    if (strstr(message, "bonjour") != NULL)
    {
        //on ajoute à response le message de bienvenue
        strcat(response, "Bonjour, je suis Wall-E.");
    }
    // si le message contient "comment vas-tu"
    if (strstr(message, "comment vas-tu") != NULL)
    {
        strcat(response, " Je vais bien, merci");
    }


    return response;
}




int main()
{
    int serverSocket;
    int dialogSocket;
    int clilen;




    if ((serverSocket = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("erreur socket");
        exit(1);
    }
    else
    {
        printf("Socket created\n");
    }

    /*
    *Lier l'adresse locale à la socket
    */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(SERV_PORT);
    if (bind(serverSocket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("servecho : erreur bind\n");
        exit(1);
    }
    else
    {
        printf("Bind done\n");
    }

    /* Paramétrer le nombre de connexion "pending" */
    if (listen(serverSocket, 5) < 0)
    {
        perror("servecho : erreur listen\n");
        exit(1);
    }
    else
    {
        printf("Listening\n");
    }

    /*Création d'un socket de dialogue*/
    clilen = sizeof(cli_addr);
    dialogSocket = accept(serverSocket,(struct sockaddr *)&cli_addr,(socklen_t *)&clilen);
    if (dialogSocket < 0)
    {
        perror("servecho : erreur accept\n");
        exit(1);
    }
    else
    {
        printf("Robot connecté\n");
    }
    /* On fait tourner indéfiniment le serveur*/
    while(1)
    {
        char buffer[256];
        int n;
        bzero(buffer,256);
        n = read(dialogSocket,buffer,255);
        if (n < 0)
        {
            perror("ERROR reading from socket");
            exit(1);
        }

        //TODO : Mettre les tests de message ici 


        printf("%s \n",dialog(buffer));
        n = write(dialogSocket,"I got your message",18);
        if (n < 0)
        {
            perror("ERROR writing to socket");
            exit(1);
        }
    }
    


    /*Fermeture de la socket de dialogue*/
    close(dialogSocket);
    /*Fermeture de la socket serveur*/
    close(serverSocket);

}

