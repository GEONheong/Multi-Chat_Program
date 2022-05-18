#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/un.h>

#define IP_ADDR "127.0.0.1"

///////////////////////////////////////
//////// User display process /////////
///////////////////////////////////////

int main(int argc, char *argv[])
{
    ///////////////////////////////////////////////
    ////////// User input prcoess socket///////////
    ///////////////////////////////////////////////

    int access_sock, accept_sock;
    struct sockaddr_un server_addr, client_addr;
    char buf[256];
    char path[256];

    int client_addr_len = sizeof(client_addr);

    if ((access_sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("error : ");
        exit(1);
    }

    server_addr.sun_family = AF_UNIX;
    sprintf(path, "./client_sock_%s", argv[1]);
    strcpy(server_addr.sun_path, path);

    if (bind(access_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind failed : ");
        exit(1);
    }

    if (listen(access_sock, 5) == -1)
    {
        perror("listen failed : ");
        exit(1);
    }
    else
    {
        printf("[Info] Unix socket : Wating for conn req\n");
    }

    if ((accept_sock = accept(access_sock, (struct sockaddr *)&client_addr, &client_addr_len)) == -1)
    {
        perror("error : ");
        exit(1);
    }
    else
    {
        printf("[Info] Unix socket : client connected\n");
    }

    ///////////////////////////////////////////////
    ////////// connected to server socket//////////
    ///////////////////////////////////////////////

    int inet_sock;
    struct sockaddr_in inet_server_addr;
    char buf2[256];

    if ((inet_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("error");
        exit(1);
    }

    inet_server_addr.sin_family = AF_INET;
    inet_server_addr.sin_addr.s_addr = inet_addr(IP_ADDR);
    inet_server_addr.sin_port = htons(atoi(argv[2]));

    if (connect(inet_sock, (struct sockaddr *)&inet_server_addr, sizeof(inet_server_addr)) == -1)
    {
        perror("connect failed");
        exit(1);
    }
    else
    {
        printf("[Info] Inet socket : connected to the server\n");
    }

    // set socket NON-BLCOK

    int flag = fcntl(accept_sock, F_GETFL, 0);
    fcntl(accept_sock, F_SETFL, flag | O_NONBLOCK);

    flag = fcntl(inet_sock, F_GETFL, 0);
    fcntl(inet_sock, F_SETFL, flag | O_NONBLOCK);

    while (1)
    {
        if (recv(accept_sock, buf, sizeof(buf), 0) > 0) //recv msg from user input process
        {
            printf("[ME] : %s\n", buf);
            send(inet_sock, buf, strlen(buf) + 1, 0); //send msg to server
        }
        else if (recv(inet_sock, buf2, sizeof(buf2), 0) > 0) //recv msg from server
        {
            printf("%s\n", buf2);
        }

        if ((strcmp(buf, "4")) == 0) //if recv "4" terminate process
        {
            break;
        }
    }

    close(inet_sock);
    close(accept_sock);
    printf("[Info] Terminate connect...");
    return 0;
}