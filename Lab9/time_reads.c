/* The purpose of this program is to practice writing signal handling
 * functions and observing the behaviour of signals.
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

/* Message to print in the signal handling function. */
#define MESSAGE "%ld reads were done in %ld seconds.\n"

/* Global variables to store number of read operations and seconds elapsed. 
 */
long num_reads, seconds;

void handler(int code){
  fprintf(stderr,MESSAGE,num_reads,seconds);
  exit(0);
}

/* The first command-line argument is the number of seconds to set a timer to run.
 * The second argument is the name of a binary file containing 100 ints.
 * Assume both of these arguments are correct.
 */

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }
    seconds = strtol(argv[1], NULL, 10);

    struct sigaction newact;
    newact.sa_handler = handler;
    newact.sa_flags = 0;
    sigemptyset(&newact.sa_mask);
    sigaction(SIGPROF,&newact,NULL);

    struct itimerval inter;
    inter.it_value.tv_sec = seconds;
    inter.it_value.tv_usec = 0;
    inter.it_interval.tv_sec = 0;
    inter.it_interval.tv_usec = 0;
    if (setitimer(ITIMER_PROF,&inter,NULL) == -1){
      perror("set time");
      exit(1);
    }

    FILE *fp;
    if ((fp = fopen(argv[2], "r")) == NULL) {
      perror("fopen");
      exit(1);
    }

    int num;
    /* In an infinite loop, read an int from a random location in the file,
     * and print it to stderr.
     */
    for (;;) {
      int loc = random() %100;
      fseek(fp,loc*4,SEEK_SET);
      if (fread(&num,1,4,fp) != 4){
        perror("fread");
        exit(1);
      }
      fprintf(stderr,"%d \n",num);
      num_reads ++;

    }
    return 1; // something is wrong if we ever get here!
}
