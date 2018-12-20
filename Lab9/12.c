/* The purpose of this program is to get some practice writing signal handling
 * functions and observing the behaviour of signals.
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

/* Messages to print in the signal handling function. */
#define MESSAGE1 "%d reads were done\n"
#define MESSAGE2 "%d reads were done in %d seconds.\n"

/* global variables to store number of read operations and seconds elapsed. You won't use
 * the seconds variable until you add the timer in step 4.
 */
int num_reads, seconds, count;



void handler(int code) {
    fprintf(stderr, MESSAGE2, count, seconds);
    exit(1);
}

/* The first command line argument is the number of seconds to set a timer to run.
 * The second argument is the name of a binary file containing 100 ints.
 * Assume both of these arguments are correct.
 */

int main(int argc, char **argv) {
    
    struct sigaction newact;
    struct itimerval timer;

    newact.sa_handler = handler;

    newact.sa_flags = 0;

    sigemptyset(&newact.sa_mask);

    sigaction(SIGALRM, &newact, NULL);

    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }
    seconds = strtol(argv[1], NULL, 10);

    timer.it_value.tv_sec = seconds;
    timer.it_value.tv_usec = 0;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 0;
    setitimer(ITIMER_REAL, &timer, NULL);
    

    FILE *fp;
    if ((fp = fopen(argv[2], "r")) == NULL) {
      perror("fopen");
      exit(1);
    }

	/* In an infinite loop, read an int from a random location in the file, and print
	 * it to stderr.
	 */

    int num;
    for (;;) {
        
        int offset = (random() % 100);
        if (fseek(fp, offset * sizeof(int), SEEK_SET) != 0) {
            perror("seek");
            exit(1);
        }
        if (fread(&num, 1, sizeof(int), fp) <= 0) {
            perror("read");
            exit(1);
        }
        fprintf(stderr, "%d\n", num);
        count++;
    }
    return 1; // something is wrong if we ever get here!
}
