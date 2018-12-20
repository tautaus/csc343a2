#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hcq.h"
#define INPUT_BUFFER_SIZE 256
#define OUT_BUF_SIZE 1024
#ifndef PORT
  #define PORT 30000
#endif
#define MAX_BACKLOG 5
#include <unistd.h>
#include <arpa/inet.h>     /* inet_ntoa */
#include <netdb.h>         /* gethostname */
#include <sys/socket.h>

struct sockname {
    int sock_fd;
    char username[30];
    char role;
    char code[7];
    char buf[INPUT_BUFFER_SIZE];
    struct sockname *next;
};

// Use global variables so we can have exactly one TA list and one student list
Ta *ta_list = NULL;
Student *stu_list = NULL;

Course *courses;  
int num_courses = 3;

/* read a string and removes \r \n
 * return -1 if disconnect the client
 * return 1 if string is reday
 * return 0 else
 */
int read_str(int fd,char *buf,int space){
    int num = read(fd,buf,space);
    if (num == 0 || num == space){
        return -1;
    }
    if (buf[num - 1] == '\n'){
        buf[num - 2] = '\0';
        return 1;
    }else{
        buf[num] = '\0';
        return 0;
    }
}

void free_struct(struct sockname* user){
    user->buf[0] = '\0';
    user->code[0] = '\0';
    user->role = '\0';
    user->username[0] = '\0';
}

/* Accept a connection. Note that a new file descriptor is created for
 * communication with the client. The initial socket descriptor is used
 * to accept connections, but the new socket is used to communicate.
 * Return the new client's file descriptor or -1 on error.
 */
int accept_connection(int fd, struct sockname *usernames) {
    int client_fd = accept(fd, NULL, NULL);
    if (client_fd < 0) {
        perror("server: accept");
        close(fd);
        exit(1);
    }

    while (usernames->sock_fd != -1 && usernames->next != NULL){
        usernames = usernames->next;
    }

    if (usernames->sock_fd == -1){
        usernames->sock_fd = client_fd;
    }else{
        struct sockname *user = malloc(sizeof(struct sockname));
        user ->sock_fd = client_fd;
        usernames->next = user;
        user ->next = NULL;
        usernames = user;
    }
    free_struct(usernames);

    return client_fd;
}

int process_arg(struct sockname *user, struct sockname *head){
    int fd = user->sock_fd;
    if ((*user).role == 'S'){
        if (strcmp("stats",user->buf) == 0){
            char *out_buf = print_currently_serving(ta_list);
            dprintf(fd,"%s",out_buf);
            free(out_buf);
        }else{
            dprintf(fd,"Incorrect syntax\r\n");
        }
    }else{
        if (strcmp("stats",user->buf) == 0){
            char* out_buf = print_full_queue(stu_list);
            dprintf(fd,"%s",out_buf);
            free(out_buf);
        }else if(strcmp("next",user->buf) == 0){
            next_overall(user->username,&ta_list,&stu_list);
            // first sock_fd = 0, do not use head
            head = head ->next;
            while (head != NULL && (head ->sock_fd == -1 || find_student(stu_list,head->username)!= NULL || head->role == 'T')){
                head = head->next;
            }
            if (head != NULL){
                int file_d = head->sock_fd;
                dprintf(file_d,"You have been taken to a TA.\nConnect is closed. Use Ctrl-C to exit nc.\r\n");
                head->sock_fd = -1;
                free_struct(head);
                user->buf[0] = '\0';
                return file_d;
            }
        }else{
            dprintf(fd,"Incorrect syntax\r\n");
        }
    }
    user->buf[0] = '\0';
    return 0;
}

int process(struct sockname *user,struct sockname *usernames){
    int fd = user->sock_fd;
    int status = read_str(fd,&(user->buf[strlen(user->buf)]), 30 - strlen(user->buf));
    if (status == -1){
        close(fd);
        return fd;
    }else if(status == 1){
        // havent implement name
        if (user->username[0] == '\0'){
            strcpy(user->username,user->buf);
            (*user).buf[0] = '\0';
            dprintf(fd,"Are you a TA or a Student (enter T or S)?\r\n");
        // TA argrument
        }else if((*user).role == 'T'){
            return process_arg(user,usernames);
        //student
        }else if((*user).role == 'S'){
            //havent implement course information
            if ((*user).code[0] == '\0'){
                if (strcmp((*user).buf,"CSC108") == 0|| strcmp((*user).buf,"CSC148") == 0 || strcmp((*user).buf,"CSC209") == 0){
                    strcpy((*user).code,(*user).buf);
                    user->buf[0] = '\0';
                    add_student(&stu_list,user->username,user->code,courses,num_courses);
                    dprintf(fd,"You have been entered into the queue. While you wait, you can use the command stats to see which TAs are currently serving students.\r\n");
                }else{
                    dprintf(fd,"This is not a vaild course. Good-Bye \r\n");
                    close(fd);
                    free_struct(user);
                    return fd;
                }
            }else{
                process_arg(user,usernames);
            }
        // havent implement role information
        }else{
            if (user->buf[0] == 'S'){
                user->role = 'S';
                dprintf(fd, "Valid courses: CSC108, CSC148, CSC209\nWhich course are you asking about\r\n");
            }else if(user->buf[0] == 'T'){
                user->role = 'T';
                add_ta(&ta_list,user->username);
                dprintf(fd,"Valid comments for TA: stats, next (or use Ctrl-C to leave)\r\n");
            }else{
                dprintf(fd,"Invalid role (enter T or S)\r\n");
            }
            user->buf[0] = '\0';
        }
    }
    return 0;
}

int main(){
    if ((courses = malloc(sizeof(Course) * 3)) == NULL) {
        perror("malloc for course list\n");
        exit(1);
    }
    strcpy(courses[0].code, "CSC108");
    strcpy(courses[1].code, "CSC148");
    strcpy(courses[2].code, "CSC209");

    struct sockname *usernames = malloc(sizeof(struct sockname));
    usernames->sock_fd = 0;
    usernames->next = NULL;

    // Create the socket FD.
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("server: socket");
        exit(1);
    }

    // Set information about the port (and IP) we want to be connected to.
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    // This should always be zero. On some systems, it won't error if you
    // forget, but on others, you'll get mysterious errors. So zero it.
    memset(&server.sin_zero, 0, 8);

    // Bind the selected port to the socket.
    if (bind(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("server: bind");
        close(sock_fd);
        exit(1);
    }

    // Announce willingness to accept connections on this socket.
    if (listen(sock_fd, MAX_BACKLOG) < 0) {
        perror("server: listen");
        close(sock_fd);
        exit(1);
    }

    // The client accept - message accept loop. First, we prepare to listen to multiple
    // file descriptors by initializing a set of file descriptors.
    int max_fd = sock_fd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(sock_fd, &all_fds);

    while (1) {
        // select updates the fd_set it receives, so we always use a copy and retain the original.
        fd_set listen_fds = all_fds;
        int nready = select(max_fd + 1, &listen_fds, NULL, NULL, NULL);
        if (nready == -1) {
            perror("server: select");
            exit(1);
        }

        // Is it the original socket? Create a new connection ...
        if (FD_ISSET(sock_fd, &listen_fds)) {
            int client_fd = accept_connection(sock_fd, usernames);
            if (client_fd > max_fd) {
                max_fd = client_fd;
            }
            FD_SET(client_fd, &all_fds);
            printf("Accepted connection\n");
            dprintf(client_fd,"Welcome to the Help Centre, what is your name?\r\n");
        }

        // Next, check the clients.
        // NOTE: We could do some tricks with nready to terminate this loop early.
        else{

        struct sockname *head = usernames;
        while (!FD_ISSET(head->sock_fd, &listen_fds)){
            head = head ->next;
        }
        int client_closed = process(head,usernames);
        if (client_closed > 0){
            //diconnect signal, not student removed by ta
            if (client_closed == head ->sock_fd){
                if (head->role == 'T'){
                    remove_ta(&ta_list,head->username);
                }else{
                    give_up_waiting(&stu_list,head->username);
                }
                head->sock_fd = -1;
                free_struct(head);
            }
            FD_CLR(client_closed, &all_fds);
            printf("Client %d disconnected\n", client_closed);
        }
        head = usernames;
        }
    }

    //should never get here
    return 1;
}