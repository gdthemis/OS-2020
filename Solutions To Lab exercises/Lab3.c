#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <ctype.h>
#define DEFAULT "\033[30;1m"
#define RED "\033[31;1m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define GRAY "\033[38;1m"
#define MAX(a, b) ((a) > (b) ? (a) : (b))
int isNumber(char s[]){ //will be used to see if a s tring is a number, returns 1 if it is a number and 0 if it is not
    int a = 0;
    for (int i = 0; s[i]!='\0'; i++)
        if (isdigit(s[i]) == 0){
            a=0 ;
            break;
        }
        else{ a=1;}
    return a;
}
int main(int argc, char** argv) {
    int k; //will be used by random
    int round = 0; //will be used by round robin
    int temp; //will be used before we send message to kids
    short mode; //a flag that will tell if we are in random or round robin procedures
    int children = atoi(argv[1]); //n. of children that will be created
    int childPids[children]; //will save pids of kids
    if (argc < 1 || argc > 3) //called the program with too many or too few parameters
    {
        printf(RED"Usage:<nChildren> [--random] [--round-robin]"WHITE"\n");
        return 0;
    }
    else if (atoi(argv[1]) < 1){ //you have to create at least one child
        printf(RED"Usage:<nChildren> [--random] [--round-robin]" WHITE"\n");
        printf(RED"You have to create at least 1 child"WHITE"\n");
        return 0;
    }
    else if (argc == 3) // if second argument is not random or round robin...
        if((strncmp(argv[2], "--random", 8)!= 0 || strlen(argv[2]) != 8)&&((strncmp(argv[2], "--round-robin",13)!=0)|| strlen(argv[2]) != 13))
        {
            printf(RED"Usage:<nChildren> [--random] [--round-robin]"WHITE"\n");
            return 0;
        }
    if (argc == 2) { //determine the mode
        mode = 0;
    }
    else if (strncmp(argv[2], "--random", 8) == 0){
        mode = 1;
    }
    else mode = 0;
    int pd[children][2]; //To create pipes for all the children
    int pd1[children][2]; //To create the second pipe
    for (int i = 0; i < children; ++i) {
//        pd[i] = malloc(sizeof(int*) * 2);
        pipe(pd[i]); //create the pipes
        if (pipe(pd[i]) != 0) {
            perror("pipe");
        }
    }
    for (int i = 0; i < children; ++i) {
//        pd[i] = malloc(sizeof(int*) * 2);
        pipe(pd1[i]); //create the second pipes
        if (pipe(pd[i]) != 0) {
            perror("pipe");
        }
    }
    for (int i = 0; i < children; ++i) {
        int pid = fork(); //create the children
        if (pid < 0)
            printf("fork error");
        if (pid == 0) {
             close(pd[i][1]); //we will not be writing to this pipe from kids, so we close it
             int val;
             while(1) {
                 if (read(pd[i][0], &val, sizeof(int)) < 0){ //read from pipe pd the message that father sent
                     printf("error detected in read will read again after you enter an integer\n");
                     continue; //error handling
                 }
                 printf(GREEN"[Child %d] [%d] Child received %d!"WHITE"\n", i, getpid(), val);
                 val++;
                 sleep(5); //sleep 5 seconds and return the value
                 if(write(pd1[i][1], &val, sizeof(int)) == -1) //write the message to father through pipe pd1
                 {
                     printf("error on writing the output, program will continue\n");
                     continue;
                 }
                 printf(CYAN"[Child %d] [%d] Child Finished hard work, writing back %d"WHITE"\n", i, getpid(), val);
        }
    }
        childPids[i] = pid; //save Pids
    }
    int tempMax = - 1;
    while (1) {
        fd_set inset; //will need a set for file descriptors
        int maxfd; //will help us to check only the fds we want
        FD_ZERO(&inset);                // we must initialize before each call to select
        FD_SET(STDIN_FILENO, &inset);   // select will check for input from stdin
        for (int i = 0; i < children; i++) { //FS_SETs and getting the max from pd1[i] for maxfd
            FD_SET(pd1[i][0], &inset); //add pd1 to the set
            if (tempMax < pd1[i][0]) {
                tempMax = pd1[i][0]; //find the max
            }
            close(pd1[i][1]); //close the writing pipes of pd1
        }
        maxfd = MAX(STDIN_FILENO, tempMax) + 1;
        int ready_fds = select(maxfd, &inset, NULL, NULL, NULL); //selext between the fds
        if (ready_fds <= 0) {
            perror("select");
            continue;                                       // just try again
        }

        if (FD_ISSET(STDIN_FILENO, &inset)) { // if STDIN_FILENO is on set after select
             char buffer[101];
             long int n_read = read(STDIN_FILENO, buffer, 100);
             buffer[n_read] = '\0';
             if (n_read > 0 && buffer[n_read-1] == '\n') {
                 buffer[n_read-1] = '\0'; //remove the'\n'
             }
             if (n_read >= 4 && strncmp(buffer, "exit", 4) == 0) { //if you get "exit" from stdin
                 for (int i = 0; i < children; ++i) {
                     if(kill(childPids[i], SIGTERM) != 0)
                         printf("killed failed on kid n.%d \n",i);
                     wait(NULL);
                 }
                 printf(YELLOW"All children terminated"WHITE"\n");
                 return 0;
             }
             else if (n_read >= 4 && strncmp(buffer, "help", 4) == 0) //if you get help
             {
                 printf(GREEN"Type a number to send job to a child!"WHITE"\n");
             }
             else if (n_read > 0 && isNumber(buffer) == 1) //if you get a number
             {
                 if (mode == 0) { //round robin
                     temp = atoi(buffer);
                     printf(BLUE"[Parent] Assigned %d to child %d"WHITE"\n", temp, round);
                     write(pd[round][1], &temp, sizeof(int));
                     round = (round+1)%children;
                 }
                 else
                 { //random
                     k = rand()%children;
                     temp = atoi(buffer);
                     printf(BLUE"[Parent] Assigned %d to child %d"WHITE"\n", temp, k);
                     write(pd[k][1], &temp, sizeof(int));
                 }
             }
             else
                 printf(MAGENTA"Type a number to send job to a child!"WHITE"\n");
         }
        for (int i = 0; i < children; ++i) // catch a message that came from a kid
            if (FD_ISSET(pd1[i][0], &inset)) {
                int val;
                read(pd1[i][0], &val, sizeof(int));
                printf(MAGENTA"Got input from pipe: '%d'"WHITE"\n", val);
            }
    }

}
