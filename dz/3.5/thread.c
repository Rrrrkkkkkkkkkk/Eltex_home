#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
// Глобальная переменная для завершения потока 2 и 3:
volatile sig_atomic_t thread2_running = 1;
volatile sig_atomic_t thread3_running = 1;

void sigint_handler(int signum) {
	if (signum == SIGINT) {
        	printf("Поток 2: Получен SIGINT. Завершаем работу.\n");
        	thread2_running = 0;
    	}
}
void *thread_func(void *arg){
  	int i;
  	int loc_id = * (int *) arg;
  	for (i = 0; i < 10; i++) {
    		printf("Поток %i выполняется\n", loc_id);
    		sleep(1);
  	}
  	pthread_exit(NULL);
}
void *thread1_func(void *arg) {
    	sigset_t mask;
    	sigfillset(&mask);
    	if (pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0) {
        	perror("pthread_sigmask");
        	pthread_exit(NULL);
    	}
    	printf("Поток 1: Все сигналы заблокированы\n");
    	int i;
    	int loc_id = * (int *) arg;
     	for (i = 0; i < 10; i++) {
        	printf("Поток %i выполняется\n", loc_id);
        	sleep(1);//Задержка
    	}
    	pthread_exit(NULL);
}
void *thread2_func(void *arg) {
    	int loc_id = * (int *) arg;
    	printf("Поток 2: Начинаем работу. Ожидаем SIGINT.\n");
    	struct sigaction sa;
    	sa.sa_handler = sigint_handler;
    	sigemptyset(&sa.sa_mask);
    	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) == -1) {
        	perror("sigaction");
        	pthread_exit(NULL);
    	}
    	while (thread2_running) {
        	printf("Поток 2 выполняется\n");
         	sleep(1);//Задержка
    	}
   	printf("Поток 2: Завершаем работу.\n");
    	pthread_exit(NULL);
	}
	void *thread3_func(void *arg) {
    	int loc_id = *(int *)arg;
     	printf("Поток 3: Начинаем работу. Ожидаем SIGQUIT.\n");
	sigset_t mask;
    	sigemptyset(&mask);
    	sigaddset(&mask, SIGQUIT);
	if (pthread_sigmask(SIG_BLOCK, &mask, NULL) != 0) {
        	perror("pthread_sigmask для потока 3");
        	pthread_exit(NULL);
    	}
    	int sig;
    	while (thread3_running) {
        	if (sigwait(&mask, &sig) != 0) {
            		if (errno == EINTR) {
                		continue; // Если прервано сигналом, продолжаем цикл
            		}
             		perror("Ошибка в sigwait");
             		thread3_running = 0;
            	break;
        	}
        	if (sig == SIGQUIT) {
            		printf("Поток 3: Получен SIGQUIT. Завершаем работу.\n");
            		thread3_running = 0;
        	}
        	printf("Поток 3 выполняется\n");
        	sleep(1);//Задержка
    	}
       	printf("Поток 3: Завершаем работу.\n");
    	pthread_exit(NULL);
}
int main(int argc, char * argv[]){
  	pid_t pid;
  	pid_t my_pid = getpid();
  	printf("%d\n", my_pid);
  	int id1, id2, id3, result;
  	pthread_t thread1, thread2, thread3;
  	id1 = 1;//....
  	result = pthread_create(&thread1, NULL, thread1_func, &id1);
  	if (result != 0) {
    		perror("Создание первого потока");
    		return EXIT_FAILURE;
  		}
	id2 = 2;//....
  	result = pthread_create(&thread2, NULL, thread2_func, &id2);
  	if (result != 0) {
    		perror("Создание второго потока");
    		return EXIT_FAILURE;
  	}
  	id3 = 3;//....
  	result = pthread_create(&thread3, NULL, thread3_func, &id3);
  	if (result != 0) {
    		perror("Создание третьего потока");
    		return EXIT_FAILURE;
  	}
	result = pthread_join(thread1, NULL);
  	if (result != 0) {
    		perror("Ожидание завершения первого потока");
    		return EXIT_FAILURE;
  	}
	result = pthread_join(thread2, NULL);
  	if (result != 0) {
    		perror("Ожидание завершения второго потока");
    		return EXIT_FAILURE;
  	}
	result = pthread_join(thread3, NULL);
  	if (result != 0) {
    		perror("Ожидание завершения третьего потока");
    		return EXIT_FAILURE;
  	}
	printf("Готово\n");
  	return EXIT_SUCCESS;
}
