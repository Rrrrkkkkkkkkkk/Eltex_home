#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    printf("PID: %d\n", getpid());
    if (argc > 1) {
        printf("Message: %s\n", argv[1]);
        return 0;
    }
    sleep(1); 
    printf("Executing self...\n");
    execlp("./self_exec", "./self_exec", "hello world", NULL);
    perror("execlp failed");
    return 1;
}
