#include "robot.h"

#define SERV_PORT 2222
#define MAX_CLIENTS 50
#define FD_SETSIZE 1024
struct sockaddr_in serv_addr;
struct sockaddr_in cli_addr;
struct client_info clients[MAX_CLIENTS];

char *dialog(char *message)
{
    char str[500] = "";
    //message = message + 1;
    int flag = 0;
    if (strstr(message, "exit") != NULL)
    {
        // close all program that run on port 2222
        //TODO : Modifier ça, ça boucle à l'infini quand on écrit exit
        exit(1);
    }
    if (strstr(message, "bonjour") != NULL)
    {
        flag++;
        sprintf(str, "%s %s", str, " Bonjour, je suis votre robot d'assistance");
    }
    if (strstr(message, "ça va") != NULL)
    {
        flag++;
        sprintf(str, "%s %s", str, " Je vais bien, merci de demander");
    }

    /*
    if (flag == 0)
    {
        
        sprintf(str, "%s %s", str, "-1");
        puts(str);
        return str;
    } else {
        sprintf(str, "%s %s\n", "Voici la réponse : ", str);
        sprintf(str, "%s %s", str, "Votre message : ");
        puts(str);
        return str;
    }*/
    if (flag == 0)
    {
        /*TODO : Envoyer techniciens et/ou experts*/
        sprintf(str, "%s %s", str, "-1");
    }

    puts(str);
    return str;
}

int main()
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int dialogSocket;
    socklen_t clilen;
    int opt = 1;
    int client_socket;
    int client_count = 0;
    fd_set read_fds;

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
    if (listen(serverSocket, MAX_CLIENTS) < 0)
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

    while (1)
    {
        FD_ZERO(&read_fds);
        FD_SET(serverSocket, &read_fds);

        // Add client sockets to read set
        for (int i = 0; i < client_count; i++)
        {
            FD_SET(clients[i].socket_id, &read_fds);
        }

        // Wait for activity on sockets
        if (select(FD_SETSIZE, &read_fds, NULL, NULL, NULL) < 0)
        {
            perror("Failed to select sockets");
            exit(EXIT_FAILURE);
        }

        // Check if server socket is ready for a new connection
        if (FD_ISSET(serverSocket, &read_fds))
        {
            int cli_addr_len = sizeof(cli_addr);

            // Accept new connection
            clients[client_count].socket_id = accept(serverSocket, (struct sockaddr *)&cli_addr, (socklen_t *)&cli_addr_len);
            if (clients[client_count].socket_id < 0)
            {
                perror("Failed to accept incoming connection");
                exit(EXIT_FAILURE);
            }

            // Print client address
            printf("New connection from %s:%d\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

            client_count++;
        }
        // define buffer
        char buffer[1024];
        bzero(buffer, 1024);

        // Check if client sockets are ready for reading
        for (int i = 0; i < client_count; i++)
        {
            if (FD_ISSET(clients[i].socket_id, &read_fds)) // if the socket is ready
            {
                // Read data from client
                int read_size = read(clients[i].socket_id, buffer, sizeof(buffer));
                if (read_size <= 0)
                {
                    close(clients[i].socket_id);
                    clients[i].socket_id = 0;
                    // Remove client socket from list
                    for (int j = i; j < client_count - 1; j++)
                    {
                        clients[j].socket_id = clients[j + 1].socket_id;
                    }
                    client_count--;

                    printf("Client disconnected\n");
                }
                else
                {
                    // Print message
                    printf("Message from client %d: %s   %d\n", clients[i].socket_id, buffer + 1, clients[i].socket_connected);
                    if (strcmp(buffer,"!1") == 0 || strcmp(buffer,"!2") == 0 || strcmp(buffer, "!3") == 0) //Quand le client se connecte, il envoie ce message au serveur
                    {
                        
                        clients[i].level = buffer[1];
                        clients[i].socket_connected = -1;
                    }

                    else if (clients[i].socket_connected != -1)
                    {
                        printf("Clients are speaking together\n");
                        // send the message to the client connected

                        if (strstr(buffer, "expert") != NULL  && clients[i].level == '2') {
                            //TODO : ICI LA CONNEXION AVEC UN EXPERT, SI LE TECH ENVOIE "expert"
                            //On trouve le client du techincien
                            //C'est socket_connected
                            int index_client = -1;
                            for (int j = 0; j < client_count; j++) {
                                if (clients[j].socket_id == clients[i].socket_connected) {
                                    //On a trouvé le client
                                    printf("OUI");
                                    index_client = j;
                                }
                            }
                            if (index_client == -1) {
                                perror("Client n'existe pas");
                                exit(1);
                            }

                            //On connecte le client à un expert si possible
                            int found = 0;
                            for (int j = 0; j < client_count; j++) {
                                if(clients[j].level == '3' && clients[index_client].level == '1' && clients[j].socket_connected == -1) {
                                    found = 1;
                                    clients[j].socket_connected = clients[index_client].socket_id;
                                    clients[index_client].socket_connected = clients[j].socket_id;
                                    char lastresponse[255];

                                    strcpy(lastresponse, "\n\nTechnicien :\n");
                                    strcat(lastresponse, buffer+1);
                                    write(clients[index_client].socket_id, lastresponse, sizeof(lastresponse));
                                    write(clients[index_client].socket_id, "You are connected with an expert.\n\nVous :", sizeof("You are connected with an expert.\n\nVous :"));
                                    write(clients[j].socket_id, "You are connected with a client.\n\nVous :", sizeof("You are connected with a client.\n\nVous :"));
                                    write(clients[i].socket_id, "You have been disconnected from the client.\n\nVous :", sizeof("You have been disconnected from the client.\n\nVous :"));
                                    clients[i].socket_connected = -1;
                                    break;
                                }
                            }
                            if (found == 0)
                            {
                                write(clients[i].socket_id, "No expert available.\n", 24);
                            }
                            
                        } else { //Si le client envoie un message normal
                            char finalresponse[255], clientstatus[255], vous[255];
                            if (clients[i].level == '1')
                            {
                                strcpy(clientstatus, "\n\nClient :\n");
                                strcat(finalresponse, clientstatus);
                                strcat(finalresponse, buffer+1);
                              
                            }
                            else if(clients[i].level == '2')
                            {
                                strcpy(clientstatus, "\n\nTechnicien :\n");
                                strcat(finalresponse, clientstatus);
                                strcat(finalresponse, buffer+1);
                            }
                            else if(clients[i].level == '3')
                            {
                                strcpy(clientstatus, "\n\nExpert :\n");
                                strcat(finalresponse, clientstatus);
                                strcat(finalresponse, buffer+1);
                            }
                            else{
                                perror("Apagnan");
                                exit(1);
                            }
                            
                            strcpy(vous, "\n\nVous :");
                            strcat(finalresponse, vous);
                            write(clients[i].socket_connected, finalresponse, strlen(finalresponse));
                            write(clients[i].socket_id, "\nVous :", sizeof("\nVous :"));
                            //reinitiliser finalresponse vous et clientstatus
                            strcpy(finalresponse, "");
                            strcpy(vous, "");
                            strcpy(clientstatus, "");

                        }
                     
                    }
                    else
                    {
                        char *response = dialog(buffer);
                        printf("response : %s\n", response);
                        if (strcmp((response), " -1") == 0) //Condition pour passer au niveau au-dessus
                        {
                            int found = 0;
                            printf("Connection with a technician\n");
                            // établir une connexion entre un client level 2 et le clients[i]
                            for (int j = 0; j < client_count; j++)
                            {
                                if (clients[j].level == '2' && clients[j].socket_connected == -1 && clients[i].level == '1' && clients[i].socket_connected == -1)
                                {
                                    clients[j].socket_connected = clients[i].socket_id;
                                    clients[i].socket_connected = clients[j].socket_id;
                                    found = 1;
                                    break;
                                }
                            }
                            if (found == 0)
                            {
                                write(clients[i].socket_id, "No technician available\n", 24);
                            }
                            else{
                                write(clients[i].socket_id, "You are connected with a technician\n\nVous :", sizeof("You are connected with a technician\n\nVous :"));
                                write(clients[i].socket_connected, "You are connected with a client\n\nVous :", sizeof("You are connected with a client\n\nVous :"));
                            }
                        }
                        else{
                            
                            char finalresponse[255], clientstatus[255];
                            strcpy(finalresponse, "\nRobot :\n");
                            strcat(finalresponse, response);
                            strcpy(clientstatus, "\n\nVous :");
                            strcat(finalresponse, clientstatus);
                            write(clients[i].socket_id, finalresponse, strlen(finalresponse));
                        }
                        
                    }

                    

                    // reset buffer
                    bzero(buffer, 1024);
                }
            }
        }
    }
    /*Fermeture de la socket de dialogue*/
    close(client_socket);
    /*Fermeture de la socket serveur*/
    close(serverSocket);
    return 0;
}

/*

int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

int opt = 1;
setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

struct sockaddr_in server_address;
server_address.sin_family = AF_INET;
server_address.sin_addr.s_addr = INADDR_ANY;
server_address.sin_port = htons(PORT);

bind(serverSocket, (struct sockaddr *)&server_address, sizeof(server_address));

listen(serverSocket, MAX_CLIENTS);

int client_socket;
struct sockaddr_in cli_address;
socklen_t cli_address_len = sizeof(cli_address);

while ((client_socket = accept(serverSocket, (struct sockaddr *)&cli_address, &cli_address_len)) >= 0) {
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
struct sockaddr_in cli_address;
socklen_t cli_address_len = sizeof(cli_address);

while ((client_socket = accept(serverSocket, (struct sockaddr *)&cli_address, &cli_address_len)) >= 0) {
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