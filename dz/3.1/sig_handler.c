#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_PATH "./sig_fifo"

void sigabrt_handler(int signum) {
    int fd = open(FIFO_PATH, O_WRONLY | O_CREAT, 0666);
    if (fd == -1) {
        perror("open FIFO");
        exit(EXIT_FAILURE);
    }
    const char *msg = "Received SIGABRT\n";
    write(fd, msg, sizeof("Received SIGABRT\n") - 1);
    close(fd);
    exit(EXIT_FAILURE);
}

int main() {
    // Создаем FIFO, если его нет
    mkfifo(FIFO_PATH, 0666);
    
    // Устанавливаем обработчик сигнала SIGABRT
    signal(SIGABRT, sigabrt_handler);
    
    printf("Process running. Send SIGABRT to test.\n");
    while (1) {
        pause(); // Ожидание сигналов
    }
    return 0;
}
