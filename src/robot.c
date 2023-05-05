#include "robot.h"


struct sockaddr_in serv_addr;
struct sockaddr_in cli_addr;
struct client_info clients[MAX_CLIENTS];

char *dialog(char *message)
{
    char* str=calloc(2048, sizeof(char));
    // message = message + 1;
    int flag = 0;
    if (strstr(message, "bonjour") != NULL)
    {
        flag++;
        sprintf(str, "%s %s", str, " Bonjour, je suis votre robot d'assistance.");
    }
    if (strstr(message, "ça va") != NULL)
    {
        flag++;
        sprintf(str, "%s %s", str, " Je vais bien, merci de demander.");
    }
    if (strstr(message, "client")!= NULL)
    {
        flag++;
        sprintf(str, "%s %s", str, " Pour créer un nouveau client, entrez dans un terminal la commande : \"./client <ip> 2222 1\".");
    }
    if (strstr(message,"technicien")!= NULL)
    {
        flag++;
        sprintf(str, "%s %s", str, " Pour créer un nouveau technicien, entrez dans un terminal la commande : \"./client <ip> 2222 2\".");
    }
    if (strstr(message,"expert")!= NULL)
    {
        flag++;
        sprintf(str, "%s %s", str, " Pour créer un nouveau expert, entrez dans un terminal la commande : \"./client <ip> 2222 3\".");
    }
    if (strstr(message,"commandes")!=NULL){
        flag++;
        sprintf(str, "%s %s", str, " Voici la liste des commandes disponibles : \n - \"bonjour\" : le robot vous répondra par un message de bienvenue. \n - \"ça va\" : le robot vous répondra par un message de bien-être. \n - \"client\" : le robot vous expliquera comment créer un nouveau client. \n - \"technicien\" : le robot vous expliquera comment créer un nouveau technicien. \n - \"expert\" : le robot vous expliquera comment créer un nouveau expert. \n - \"totech\" : le robot vous mettra en contact avec un technicien. \n - \"toexp\" : le robot vous mettra en contact avec un expert. \n - \"commandes\" : le robot vous affichera la liste des commandes disponibles.");
    }
    if (strstr(message,"totech")!=NULL)
    {
        flag++;
        sprintf(str, "%s %s", str, "-1");
    }
    if (strstr(message,"toexp")!=NULL)
    {
        flag++;
        sprintf(str, "%s %s", str, "Veuillez d'abord passer par un technicien avant de contacter un expert, vous pouvez utiliser la commande \"totech\".");
    }
    if (flag == 0)
    {
        /*TODO : Envoyer techniciens et/ou experts*/
        sprintf(str, "%s %s", str, "-1");
    }

    //puts(str);
    return str;
}

int isInArray(int socket, int contacted_socket[])
{
    int i;
    for (i = 0; i < MAX_CLIENTS; i++)
    {
        if (contacted_socket[i] == socket)
        {
            return 1;
        }
    }
    return 0;
}

int main()
{
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
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
                    // printf("Message from client %d: %s   %d\n", clients[i].socket_id, buffer + 1, clients[i].socket_connected);
                    if (strcmp(buffer, "!1") == 0 || strcmp(buffer, "!2") == 0 || strcmp(buffer, "!3") == 0) // Quand le client se connecte, il envoie ce message au serveur
                    {

                        clients[i].level = buffer[1];
                        clients[i].socket_connected = -1;
                        clients[i].number_contacted = 0;
                    }

                    else if (clients[i].socket_connected != -1)
                    {
                        printf("Clients are speaking together\n");
                        // send the message to the client connected

                        if (strstr(buffer, "leave") != NULL)
                        {
                            for (int j = 0; j < client_count; j++)
                            {
                                if (clients[j].socket_id == clients[i].socket_connected)
                                {
                                    if (clients[i].level == '1')
                                    {
                                        memset(&clients[i].socket_contacted, 0, sizeof(clients[i].socket_contacted));
                                        clients[i].number_contacted = 0;
                                    }
                                    else{
                                        memset(&clients[j].socket_contacted, 0, sizeof(clients[j].socket_contacted));
                                        clients[j].number_contacted = 0;
                                    }

                                    clients[j].socket_connected = -1;
                                    write(clients[j].socket_id, "\n\033[31;01m/!\\ Your contact left the chat.\033[00m\n\n\033[36;01mVous :\033[00m", sizeof("\n\033[31;01m/!\\ Your contact left the chat.\033[00m\n\n\033[36;01mVous :\033[00m"));
                                    break;
                                }
                            }

                            clients[i].socket_connected = -1;
                            write(clients[i].socket_id, "\n\033[31;01m/!\\ You have been disconnected from your contact.\033[00m\n\n\033[36;01mVous :\033[00m", sizeof("\n\033[31;01m/!\\ You have been disconnected from your contact.\033[00m\n\n\033[36;01mVous :\033[00m"));
                        }

                        else if (strstr(buffer, "switch") != NULL && clients[i].level != '1')
                        { // i : tech/expert, j : client, k: recherche de expert/tech
                            for (int j = 0; j < client_count; j++)
                            {
                                if (clients[j].socket_id == clients[i].socket_connected)
                                {
                                    int found = 0;
                                    for (int k = 0; k < client_count; k++)
                                    {
                                        if (!isInArray(clients[k].socket_id, clients[j].socket_contacted) && clients[k].socket_connected == -1 && clients[k].level == clients[i].level)
                                        {
                                            clients[j].socket_connected = clients[k].socket_id;
                                            clients[k].socket_connected = clients[j].socket_id;
                                            clients[j].socket_contacted[clients[j].number_contacted] = clients[k].socket_id;
                                            clients[j].number_contacted++;
                                            clients[i].socket_connected = -1;

                                            write(clients[j].socket_id, "\n\033[31;01m/!\\ You have switched to another contact.\033[00m\n\n\033[36;01mVous :\033[00m", sizeof("\n\033[31;01m/!\\ You have switched to another contact.\033[00m\n\n\033[36;01mVous :\033[00m"));
                                            write(clients[i].socket_id, "\n\033[31;01m/!\\ You have been switched out.\033[00m\n\n\033[36;01mVous :\033[00m", sizeof("\n\033[31;01m/!\\ You have been switched out.\033[00m\n\n\033[36;01mVous :\033[00m"));
                                            write(clients[k].socket_id, "\n\033[32;01mYou have been connected to a client.\033[00m\n\n\033[36;01mVous :\033[00m", sizeof("\n\033[32;01mYou have been connected to a client.\033[00m\n\n\033[36;01mVous :\033[00m"));

                                            break;
                                        }
                                    }
                                    if (found == 0)
                                    {
                                        write(clients[i].socket_id, "\n\033[31;01m/!\\ No new contact available.\033[00m\n\n\033[36;01mVous :\033[00m", sizeof("\n\033[31;01m/!\\ No new contact available.\033[00m\n\n\033[36;01mVous :\033[00m"));
                                    }

                                    break;
                                }
                            }
                        }

                        else if (strstr(buffer, "expert") != NULL && clients[i].level == '2')
                        {
                            int index_client = -1;
                            for (int j = 0; j < client_count; j++)
                            {
                                if (clients[j].socket_id == clients[i].socket_connected)
                                {
                                    index_client = j;
                                }
                            }
                            if (index_client == -1)
                            {
                                perror("Client n'existe pas");
                                exit(1);
                            }

                            // On connecte le client à un expert si possible
                            int found = 0;
                            for (int j = 0; j < client_count; j++)
                            {
                                if (clients[j].level == '3' && clients[index_client].level == '1' && clients[j].socket_connected == -1)
                                {
                                    found = 1;
                                    clients[j].socket_connected = clients[index_client].socket_id;
                                    clients[index_client].socket_connected = clients[j].socket_id;
                                    char lastresponse[255];
                                    clients[index_client].socket_contacted[clients[index_client].number_contacted] = clients[j].socket_id;
                                    clients[index_client].number_contacted++;

                                    strcpy(lastresponse, "\n\n\033[34;01mTechnicien :\033[00m\n");
                                    strcat(lastresponse, buffer + 1);
                                    write(clients[index_client].socket_id, lastresponse, sizeof(lastresponse));
                                    write(clients[index_client].socket_id, "\033[32;01mYou are connected with an expert.\033[00m\n\n\033[36;01mVous :\033[00m", sizeof("\033[32;01mYou are connected with an expert.\033[00m\n\n\033[36;01mVous :\033[00m"));
                                    write(clients[j].socket_id, "\033[32;01mYou are connected with a client.\033[00m\n\nVous :\033[00m", sizeof("\033[32;01mYou are connected with a client.\033[00m\n\nVous :\033[00m"));
                                    write(clients[i].socket_id, "\033[31;01m/!\\ You have been disconnected from the client.\033[00m\n\n\033[36;01mVous :\033[00m", sizeof("\033[31;01m/!\\ You have been disconnected from the client.\033[00m\n\n\033[36;01mVous :\033[00m"));
                                    clients[i].socket_connected = -1;
                                    break;
                                }
                            }
                            if (found == 0)
                            {
                                write(clients[i].socket_id, "\033[31;01m/!\\ No expert available.\033[00m\n", sizeof("\033[31;01m/!\\ No expert available.\033[00m\n"));
                            }
                        }
                        else
                        { // Si le client envoie un message normal
                            char finalresponse[2048], clientstatus[2048], vous[255];
                            if (clients[i].level == '1')
                            {
                                strcpy(clientstatus, "\n\n\033[34;01mClient :\033[00m\n");
                                strcat(finalresponse, clientstatus);
                                strcat(finalresponse, buffer + 1);
                            }
                            else if (clients[i].level == '2')
                            {
                                strcpy(clientstatus, "\n\n\033[34;01mTechnicien :\033[00m\n");
                                strcat(finalresponse, clientstatus);
                                strcat(finalresponse, buffer + 1);
                            }
                            else if (clients[i].level == '3')
                            {
                                strcpy(clientstatus, "\n\n\033[34;01mExpert :\033[00m\n");
                                strcat(finalresponse, clientstatus);
                                strcat(finalresponse, buffer + 1);
                            }
                            else
                            {
                                perror("Apagnan");
                                exit(1);
                            }

                            strcpy(vous, "\n\n\033[36;01mVous :\033[00m");
                            strcat(finalresponse, vous);
                            write(clients[i].socket_connected, finalresponse, strlen(finalresponse));
                            write(clients[i].socket_id, "\n\033[36;01mVous :\033[00m", sizeof("\n\033[36;01mVous :\033[00m"));
                            // reinitiliser finalresponse vous et clientstatus
                            strcpy(finalresponse, "");
                            strcpy(vous, "");
                            strcpy(clientstatus, "");
                        }
                    }
                    else
                    {
                        char *response = dialog(buffer);
                        printf("response : %s\n", response);
                        if (strcmp((response), " -1") == 0 && clients[i].level == '1') // Condition pour passer au niveau au-dessus
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
                                write(clients[i].socket_id, "\033[31;01m/!\\ No technician available\033[00m\n", sizeof("\033[31;01m/!\\ No technician available\033[00m\n"));
                            }
                            else
                            {
                                clients[i].socket_contacted[clients[i].number_contacted] = clients[i].socket_connected;
                                clients[i].number_contacted++;
                                write(clients[i].socket_id, "\033[32;01mYou are connected with a technician\033[00m\n\n\033[36;01mVous :\033[00m", sizeof("\033[32;01mYou are connected with a technician\033[00m\n\n\033[36;01mVous :\033[00m"));
                                write(clients[i].socket_connected, "\033[32;01mYou are connected with a client\033[00m\n\n\033[36;01mVous :\033[00m", sizeof("\033[32;01mYou are connected with a client\033[00m\n\n\033[36;01mVous :\033[00m"));
                            }
                        }
                        else
                        {

                            char finalresponse[2048], clientstatus[2048];
                            strcpy(finalresponse, "\n\033[35;01mRobot :\033[00m\n");
                            strcat(finalresponse, response);
                            strcpy(clientstatus, "\n\n\033[36;01mVous :\033[00m");
                            strcat(finalresponse, clientstatus);
                            write(clients[i].socket_id, finalresponse, strlen(finalresponse));
                        }
                        free(response);
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
