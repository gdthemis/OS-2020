
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
int i = 0;
int count = 0;
int use;
short print = 0;
short alarming = 0;
short terminate = 0;
short parusr2 = 0;
short hd1 = 0;
short hd2 = 0;
short sigint = 0;
short handler22 = 0;
short handler23 = 0;

void terminator1(int sig)
{
    sigint = 1;
}
void teliko(int sig)
{
    alarming = 1;
}
void terminator (int sig)
{
    terminate = 1;
}
void handler7 (int sig)
{
    parusr2 = 1;
}
void handlertelos(int sig)
{
    hd2 = 1;
}
void handler4 (int sig)
{
    print = 1;
}
void handler2 (int sig)
{
    handler22 = 1;
//    printf("[Child Process %d: %d] Value: %d!\n",i,getpid(),count);
}
void handler3 (int sig)
{
    handler23 = 1;
//    printf("[Child Process %d] Echo!\n", getpid());
}
void handler1 (int sig)
{
    hd1 = 1;
}

int main(int argc, const char * argv[]) {
    short see[argc - 1];
    int z = 100;
    alarm(z);
    printf("MAXIMUM EXECUTION TIME IS SET TO %d SECONDS\n",z);
    use = argc - 1;
    int status;
    pid_t w;
    int c[argc - 1];
    int delays[argc - 1];

    printf("[Father process was created : %d]\n", getpid());
//    struct sigaction action1;
//    action1.sa_handler =handler4;
//    sigaction(SIGUSR1, &action1, NULL);
//    struct sigaction action2;
//    action2.sa_handler =handler7;
//    sigaction(SIGUSR2, &action2, NULL);
//    struct sigaction action3;
//    action3.sa_handler =terminator;
//    sigaction(SIGTERM, &action3, NULL);
//    struct sigaction action4;
//    action4.sa_handler =teliko;
//    sigaction(SIGALRM, &action4, NULL);
    signal(SIGUSR1, handler4); //Για να εκτυπωθεί το μήνυμα με την τιμή του count σε αυτή τη φάση από το κάθε παιδί
    signal(SIGUSR2, handler7); //Για το echo του πατέρα
    signal(SIGTERM, terminator); //Θα οδηγήσει στο Sigkill του κάθε παιδιού
    signal(SIGALRM, teliko); //Θα οδηγήσει στο Sigkill λόγω χρόνου του κάθε παιδιού
    signal(SIGINT, terminator1); // θα οδηγήσει όπου οδηγεί και το Sigterm, όπως μας λέει και η εκφώνηση
    for (int x = 1; x < argc; x++)
    {
        if (atoi(argv[x])<=0) { // αν έχω είσοδο που δεν είναι αριθμός τότε δώσε μου error
            printf("error, invalid input\n");
            return 0;
        }
        delays[x-1] = atoi(argv[x]);
        if (delays[x-1] < 0)
        { //αν η είσοδος είναι αρνητικός αριθμός επέστρεψε error
            printf("error, invalid input\n");
            return 0;
        }
    }
    for (int x = 0; x < argc - 1; x++)
    {
        i++;
        int pid = fork();
            if (pid < 0)
                printf("fork error");
            if (pid == 0)
            {
                printf("[child Process %d: %d] was created and will pause!\n",i,getpid());
                count = 0;
                if(raise(SIGSTOP) != 0)
                    printf("Error, child did not pause\n");
                signal(SIGUSR1, handler2); //Θα κάνει ένα short = 1 και θα εκτυπώσει ένα μήνυμα
                signal(SIGUSR2, handler3); //για το echo αυτού του παιδιού
                signal(SIGTERM, handlertelos); //θα οδηγήσει στο Sigkill αυτού του παιδιού
                signal(SIGALRM, handler1); //θα οδηγήσει στο Sigkill λόγω χρόνου
                printf("[Child Process %d: %d] Is starting!\n", i , getpid());
                while(1)
                {
                    sleep(delays[x]);
                    count++;
                    if (handler23 == 1) {
                        printf("[Child Process %d] Echo!\n", getpid());
                        handler23 = 0;
                    }
                    if (handler22 == 1) {
                        printf("[Child Process %d: %d] Value: %d!\n",i,getpid(),count);
                        handler22 = 0;
                    }
                    if (hd1 == 1) {
                        printf("[Child process %d: %d] Time Expired! Final Value: %d\n",i,getpid(),count);
                        raise(SIGKILL); // θα ενεργοποιηθεί όταν πάρει το σήμα από την alarm το οποίο είναι στη main και θα
                        exit(0); //θα σταλεί στα παιδία
                    }
                    if (hd2 == 1) {
                        printf("[Child process %d: %d] Will Be Killed Final Value: %d\n",i,getpid(),count);
                        raise(SIGKILL); //θα ενεργοποιηθεί όταν τα παιδιά λάβουν το σήμα sigterm είτε από το terminal είτε
                        exit(0); //από τον πατέρα
                    }
                }
         }
        else
        {
            c[x] = pid; //αποθήκευση των pid των παιδιών
        }
    }
    for (int x = 0; x < argc-1; x++) {
        w = waitpid(c[x], &status, WUNTRACED|WCONTINUED);
    } //περίμενε να γίνει pause σε όλα τα παιδιά
    for (int x = 0; x < argc - 1; ++x) {
        kill(c[x], SIGCONT); //εκκίνησε όλα τα παιδιά
    }
    for (int x = 0; x < argc-1; x++) {
        w = waitpid(c[x], &status, WUNTRACED|WCONTINUED);
    } //περίμενε να εκκινήσουν τα παιδιά
    while(1)
    {
        if (terminate == 0){
            sleep(2); //Για να μην προλάβει να πιάσει θάνατο που έγινε μέσω του πατέρα (καθυστερεί 2 δευτερόλεπτα)
        if (waitpid(-1, &status, WNOHANG)) { //Για να πιάσουμε θάνατο από το τερμιναλ
            printf("A child died directly from the terminal \n");
        }
        }
        if (parusr2 == 1) { // εκτύπωσε το echo του πατέρα
            printf("[Process %d] Echo!\n",getpid());
            parusr2 = 0;
        }
    if (print == 1) { //εκτύπωσε ότι ο πατέρας θα ζητήσει τα count των παιδιών που είναι ζωντανά
        printf("[Father process: %d] Will ask current values from all active children processes\n",getpid());
        for (int x = 0; x < argc - 1; x++) {
            kill(c[x], SIGUSR1); //ζήτα το count από το κάθε παιδιί
        }
        print = 0; // κάνε πάλι τη μεταβλητή 0 για να μην εκτυπωθούν πολλές φορές τα ίδια πράγματα
    }
    if (terminate == 1 && sigint == 0) //αν είναι η πρώτη φορά που καλλείται η Sigint ή η sigterm στον πατέρα
    {
        for (int x = 0; x < argc - 1; x++) { //τερμάτισε όλα τα παιδιά
            printf("[Father process: %d] Will terminate (SIGTERM) child process %d: %d\n",getpid(),x+1,c[x]);
           if(kill(c[x], SIGTERM) != 0) //αν υπάρχει πρόβλημα στην kill δώσε error
                    printf("kill1 failed on child n.%d or child was dead\n",x+1);
            }
        
        for (int x = 0; x < argc-1; x++) { //περίμενε να δεις αν όλα πήγαν καλά και εκτύπωσε ότι το κάθε παιδί τερμάτισε
            w = waitpid(c[x], &status, WUNTRACED|WCONTINUED);
             printf("child %d (pid : %d) was terminated\n",x + 1,c[x]);
        }
        sigint = -1; //για να μην προσπαθήσεις να τερματίσεις ξανά τα παιδιά
        terminate = -1; //για να μην προσπαθήσεις να τερματίσεις ξανά τα παιδιά
    }
        if (sigint == 1 && terminate == 0) { //όμοια με πριν
             for (int x = 0; x < argc - 1; x++) {
                       printf("[Father process: %d] Will terminate (SIGINT) child process %d: %d\n",getpid(),x+1,c[x]);
                       if(kill(c[x], SIGTERM) != 0)
                               printf("kill2 failed on child n.%d or child was dead\n",x+1);
                       }
            for (int x = 0; x < argc-1; x++) {
                w = waitpid(c[x], &status, WUNTRACED|WCONTINUED);
                printf("child %d (pid : %d) was terminated\n",x + 1,c[x]);
            }
            terminate = -1;
            sigint = -1;
        }
        if ((sigint == 1 && terminate == -1) || (sigint == -1 && terminate == 1)) {
            //αν προσπασθήσεις να τερματίσεις ξανά τα παιδία δώσε μήνυμα ότι αυτό δεν είναι δυνατό
            printf("Should not call SIGINT or SIGTERM more than once\n");
            sigint = - 1;
            terminate = - 1;
        }
//            if (terminate == 0)
//            if (waitpid(-1, &status, WNOHANG))
//                printf("User Has Killed a Kid From Terminal\n");
            

    if (alarming == 1)
    { //αν τελειώσει ο χρόνος εκτέλεσης δώσε τερμάτισε όλα τα παιδιά
        for (int x = 0; x < argc - 1; x++) {
            if(kill(c[x], SIGALRM) != 0)
            {
                printf("kill3 failed on child n.%d or child was dead\n",x+1);
                see[x] = 1;
            }
            else
                see[x] = 0;
            
        }
        for (int x = 0; x < argc-1; x++) {
              w = waitpid(c[x], &status,WUNTRACED|WCONTINUED);
            if (see[x] == 0)
                printf("child %d (pid : %d) was terminated\n",x + 1,c[x]);
          }
        //τέλος τερματισε και τον πατέρα
        printf("Father %d will be terminated\n", getpid());
        exit(0);
        return 0;
    }
    }
    return 0;
}
