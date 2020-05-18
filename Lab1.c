//
//  main.cpp
//
//  Created by christos on 23/3/20.
//  Copyright © 2020 christos. All rights reserved.
//

#define BUFFER_SIZE 64

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <string.h>

typedef enum {
    ENCRYPT,
    DECRYPT
} encrypt_mode;

int find (const char ** a){
    int x = 0;
    while (x < 5) {
        int res = strncmp(a[x], "--input", 7);
        if (res == 0) {
            return x;
        }
//        printf ("%s\n", a[x]);
        x++;
    }
    return -1;
}

int find1 (const char ** a){
    int x = 0;
    while (x < 5) {
        int res = strncmp(a[x], "--key", 5);
        if (res == 0) {
            return x;
        }
//        printf ("%s\n", a[x]);
        x++;
    }
    return -1;
}

char caesar(unsigned char ch, encrypt_mode mode, int key)
{
    if (ch >= 'a' && ch <= 'z') {
        if (mode == ENCRYPT) {
            ch += key;
            if (ch > 'z') ch -= 26;
        } else {
            ch -= key;
            if (ch < 'a') ch += 26;
        }
        return ch;
    }

    if (ch >= 'A' && ch <= 'Z') {
        if (mode == ENCRYPT) {
            ch += key;
            if (ch > 'Z') ch -= 26;
        } else {
            ch -= key;
            if (ch < 'A') ch += 26;
        }
        return ch;
    }

    return ch;
}

int main(int argc, const char ** argv) {
    char buffer[BUFFER_SIZE];
    int a =find(argv) ;
    int key = atoi(argv[find1(argv) + 1]);
    if (key >25 || key <= 0) {
        printf("error : key > 25 or not valid key\n");
        exit(-1);
    }
  long  int n_read, n_write;
    int i = 0;
    char buffer1[BUFFER_SIZE];
    
    int fd_in = open(argv[a + 1], O_RDONLY);
    if (fd_in == -1) {
        perror("open");
//        fd_in = open("neo.txt", O_RDONLY | O_CREAT);
//        while (true) {
//        n_write = write(fd_in, 0, n_read);
//
//        }
        exit(-1);
    }
    int out = open("encrypted.txt", O_CREAT|O_WRONLY| O_TRUNC);
    if (out == -1) {
        perror("open");
        exit(-1);
    }
    pid_t c1 = fork();
    if(c1<0){
        perror("fork");
    }
  else if (c1 == 0) {
//      printf("ela2 my id is %d \n", getpid());
//            printf("ela2 my parentid is %d \n", getppid());

            do {
                n_read = read(fd_in, buffer, BUFFER_SIZE);
                if (n_read == -1) {
                    perror("read");
                    exit(-1);
                }
                for (i = 0; i < n_read; i++) {
                    buffer1[i] = caesar(buffer[i], ENCRYPT, key);
                }
                n_write = write(out, buffer1, n_read);
                if (n_write < n_read) {
                    perror("write");
                    exit(-1);
                }
                
            } while (n_read > 0);

            close(fd_in);
            close(out);

  }
//    wait(NULL);
    else if (c1>0)
    {
        wait(NULL);

        pid_t c2 = fork();
        if(c2<0){
            perror("fork");
        }
       else  if (c2 == 0){

//            wait(&pid);

//    printf("ela1 my id is %d \n", getpid());
//    printf("ela1 my parentid is %d \n", getppid());
//            pid_t pid1 = fork();
        int enc = open("encrypted.txt", O_RDONLY);
        if (out == -1) {
        perror("open");
        exit(-1);
                }
        do {
        n_read = read(enc, buffer, BUFFER_SIZE);
            if (n_read == -1) {
        perror("read");
        exit(-1);
            }
            
    for (i = 0; i < n_read; i++) {
    buffer1[i] = caesar(buffer[i], DECRYPT, key);
    }
    n_write = write(1, buffer1, n_read);
    if (n_write < n_read) {
    perror("write");
    exit(-1);
    }
    } while (n_read > 0);
        close(enc);

        }

        wait(NULL);
    
    }
}


