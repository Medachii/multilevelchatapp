#ifndef __ROBOT_H__
#define __ROBOT_H__

struct client_info {
    int socket_id;
    int level;
    int socket_connected;
    // autres informations sur le client
};

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <sys/select.h>



#endif

