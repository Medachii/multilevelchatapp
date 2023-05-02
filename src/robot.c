#include "robot.h"

#define SERV_PORT 2222
struct sockaddr_in serv_addr;
struct sockaddr_in cli_addr;

char *dialog(char *message)
{
    char str[500] = "";
    sprintf(str,"[Level %c]", message[0]);
    message = message+1;
    int flag = 0;
    if (strstr(message, "exit") != NULL)
        {
            //close all program that run on port 2222
            exit(1);
        }
    if (strstr(message, "bonjour") != NULL)
    {
        flag++;
        sprintf(str, "%s %s", str, " Bonjour, je suis Wall-E.");
    }
    if (strstr(message, "ca va") != NULL)
    {
        flag++;
        sprintf(str, "%s %s", str, " Ouai et toi bb ?");
    }
    if (flag == 0) {
        /*TODO : Envoyer techniciens et/ou experts*/
        sprintf(str, "%s %s", str, message);
    }

    puts(str);
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
    dialogSocket = accept(serverSocket, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);
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
    while (1)
    {
        char buffer[256];
        int n;
        bzero(buffer, 256);
        n = read(dialogSocket, buffer, 255);
        if (n < 0)
        {
            perror("ERROR reading from socket");
            exit(1);
        }

        // TODO : Mettre les tests de message ici
        
        dialog(buffer);

        n = write(dialogSocket, "I got your message", 18);
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




/*

int server_socket = socket(AF_INET, SOCK_STREAM, 0);

int opt = 1;
setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

struct sockaddr_in server_address;
server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr = INADDR_ANY;
server_address.sin_port = htons(PORT);

bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

listen(server_socket, MAX_CLIENTS);

int client_socket;
struct sockaddr_in client_address;
socklen_t client_address_len = sizeof(client_address);

while ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len)) >= 0) {
    // gérer la connexion du client ici
}


// structure pour stocker les informations du client
struct client_info {
    int socket_id;
    // autres informations sur le client
};

// tableau pour stocker les informations de tous les clients
struct client_info clients[MAX_CLIENTS];

// lorsqu'un nouveau client se connecte, stocker son identifiant de socket dans la structure clients
int client_socket;
struct sockaddr_in client_address;
socklen_t client_address_len = sizeof(client_address);

while ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_len)) >= 0) {
    // trouver la première entrée libre dans le tableau clients
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].socket_id == -1) {
            clients[i].socket_id = client_socket;
            break;
        }
    }
    // gérer la connexion du client ici
}

// pour envoyer un message à un client en particulier, utiliser la fonction send() avec l'identifiant de socket correspondant
int client_id = 3; // exemple: envoyer un message au client d'indice 3
char message[] = "Bonjour client 3!";
send(clients[client_id].socket_id, message, strlen(message), 0);

*/