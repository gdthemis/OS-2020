#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <netdb.h>
#include <sys/time.h>
#include <ctype.h>
#include <stdlib.h>
#define DEFAULT "\033[30;1m"
#define RED "\033[31;1m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define GRAY "\033[38;1m"


char *mode1 (int in) //when you call get
{
    if (in == 0)
        return "boot\n";
    if (in == 1)
        return "setup\n";
    if (in == 2)
        return "interval\n";
    if (in == 3)
        return "button\n";
    if (in == 4)
        return "motion\n";
    return "something else\n";
}

int findHost (const char ** a){
    int x = 0;
    while (x < 7) {
        if (strncmp(a[x], "[--host", 7) == 0) {
           return x;
        }
        x++;
    }
    return -1;
}
int findPort (const char ** a){
    int x = 0;
    while (x < 7) {
        if (strncmp(a[x], "[--port", 7) == 0) {
            return x;
        }
        x++;
    }
    return -1;
}
int findDebug (const char ** a){
    int x = 0;
    while (x < 8) {
        if (strncmp(a[x], "[--debug]", 9) == 0) {
            return x;
        }
        x++;
    }
    return -1;
}



int main(int argc, const char * argv[]) {
    char aux[1000];
    char buffer[1000];
    int a;
    int port = 0;
    int mode = 0;
    int debug = -1;
    if (argc > 6) {
        printf("should be something like : ./ask4 [--host HOST] [--port PORT] [--debug]\n");
        return 0;
    }
    else if (argc != 1)
    {
        mode = 1; //mode debug is on
        int aux = findPort(argv);
        if (aux != -1)
        {
            port = atoi(argv[aux+1]);
        }
    }
    if (mode == 1)
    {
        debug = findDebug(argv);
    }
    printf(GREEN"connecting..."WHITE"\n");
    
    struct hostent *server_host;  //host info
    long int n, n_read, n_write;
    int domain = AF_INET; //type of addreses that our socket can communicate
    int type = SOCK_STREAM;  //communication type
    int sock_fd = socket(domain, type, 0);  //create socket file discriptor
    if (sock_fd < 0)
    {
        perror("socket");
        return -1;
    }
    
    struct sockaddr_in sin; //specifies a transport address and port for the AF_INET address family. (has information about                        the connection to the server)
    sin.sin_family = AF_INET; //has to be the same with socket domain parameter
    sin.sin_port = htons(8080); //give port number
    sin.sin_addr.s_addr = htonl(INADDR_ANY); //we accept connections from everywhere
    
    if (mode == 1) {
        sin.sin_port = htons(port);  //give port read from argv
    }
    
    server_host = gethostbyname("tcp.akolaitis.os.grnetcloud.net");
    if(server_host == NULL) {
       printf(RED"Unknown Host"WHITE"\n");
       exit(1);
    }
    
    if (mode == 1) {
        char temp [strlen(argv[findHost(argv) + 1])];
        strcpy(temp, argv[findHost(argv) + 1]); //move address to temp
        temp[strlen(temp)-1] = '\0';
        server_host = gethostbyname(temp); //give name read from argv
        if(server_host == NULL) {
           printf(RED"Unknown Host"WHITE"\n");
           exit(1);
        }
    }
    bcopy((char*)server_host->h_addr, (char*)&sin.sin_addr,
    server_host->h_length); //move h_length chars from 2nd to 1st
    
    if(connect(sock_fd, (struct sockaddr*)&sin, sizeof(sin)) < 0) // connect socket to server (second argument points to an
                                                                  // address) third argument is the length of second
                                                                  // first is the socket
    {
        printf(RED"could not connect to server"WHITE"\n");
        return 0;
    }
    printf(BLUE"Connencted to server"WHITE"\n");
    while (1) {
        n = read(0, buffer, 999);  //read from terminal
        if (n < 0) {
            printf("error\n");
            exit(-1);
        }

        if (strncmp(buffer, "help", 4) == 0) {  //print this if youy get "help"
            printf(YELLOW"EXAMPLES :"WHITE"\n");
            printf(YELLOW"[Number] [Name] [Surname] [Reason]"WHITE"\n");
            printf(YELLOW"[get]"WHITE"\n");
            printf(YELLOW"help"WHITE"\n");
            printf(YELLOW"exit"WHITE"\n");
        }
        else if(strncmp(buffer, "get", 3) == 0){  //print this if you get "get"
            n_write = write(sock_fd, "get", 3);
            if (n_write < 0) {
                printf(RED"error in write"WHITE"\n");
                exit(-1);
            }
            if (debug != -1)
                write(1,"[DEBUG] sent get", 17);
            n_read = read(sock_fd, buffer, 999);
            if (n_read < 0) {
                printf(RED"error in read"WHITE"\n");
                exit(-1);
            }
            if (debug != -1)
            {
                write(1, "\n", 1);
                write(1, "[DEBUG] read : ", 15);
                write(1, buffer, n_read);
            }
            printf("latest event: %s", mode1(atoi(&buffer[0])));  //break the message you receive to smaller strings
            strncpy(aux, buffer + 2, 3);
            aux[3] = '\0';
            a = atoi(aux);
            printf("light level is : %d \n", a);
            strncpy(aux, buffer + 6, 4);
            aux[4] = 0;
            a = atoi(aux);
            printf("Temperature: %f \n",a/100.0);
            strncpy(aux, buffer + 11, 10);
            aux[10] = 0;
            time_t rawtime = atoi(aux);
            struct tm *a;
            a = localtime(&rawtime); //get the date from timestamp
            printf("Timestamp is: %s", asctime(a));
        }
        else if (strncmp(buffer, "exit", 4) == 0){ //if you get "exit" do this
            close(sock_fd);
            return 0;
        }
        else  //if you are trying to put name surname etc do this
        {
            n_write = write(sock_fd, buffer, n);  //write message to server
            if (n_write < 0) {
                printf(RED"error in write"WHITE"\n");
                exit(-1);
            }
            if (debug != -1) {
                write(1,"[DEBUG] sent :", 14);
                n_write = write(1, buffer, n);
            }
            n_read = read(sock_fd, buffer, 999);  //read back from server
            if (n_read < 0) {
                printf(RED"error in read"WHITE"\n");
                exit(-1);
            }
            if (strncmp(buffer, "ACK", 3) == 0) {  //if what you read started with "ACK" do this
                if (debug != -1) {
                    write(1,"[DEBUG] received :", 18);
                    for (int i = 0; buffer[i] != '\n'; i++) {
                        write(1, &buffer[i], 1);
                    }
                    write(1, "\n", 1);
                }
                write(1, "response : ", 11);
                for (int i = 0; buffer[i] != '\n'; i++) {
                    write(1, &buffer[i], 1);
                }
                write(1, "\n", 1);
            }
            else if (strncmp(buffer, "try again", 9) == 0)  //if you received "try again" do this
            {
                if (debug != -1) {
                    write(1,"[DEBUG] received :", 18);
                    printf(CYAN"try again"WHITE"\n");
                }
                printf(CYAN"try again"WHITE"\n");
            }
            else if (strncmp(buffer, "invalid code", 12) == 0)  //if you received "invalid code"
            {
                if (debug != -1) {
                    write(1,"[DEBUG] received :", 18);
                    n_write = write(1, buffer, n_read);
                }
                write(1, "Invalid code, please try again from the start", 45);
                write(1, "\n", 1);
            }
            else  //if none of the above you received the code, so do that
            {
                if (debug != -1) {
                    write(1,"[DEBUG] received :", 18);
                    n_write = write(1, buffer, n_read);
                    write(1,"\n", 1);
                }
                write(1, "send verification code : ", 25);
                n_write = write(1, buffer, n_read);
                write(1, "\n", 1);
            }
        }
    }
}
