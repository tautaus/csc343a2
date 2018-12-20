#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

char* name;

void sing(int code){
    printf("happy birthday to you %s\n",name);
    sleep(10);  
    printf("sigint doesnt works\n");
    exit(1);
}


int main(int argc, char const *argv[])
{   

    name = argv[1];

    struct sigaction newact;
    newact.sa_handler = sing;
    newact.sa_flags = 0;
    sigemptyset(&newact.sa_mask);
    sigaction(SIGINT,&newact,NULL);

    for(;;){
        printf("hello world\n");
    }
    return 0;
}
