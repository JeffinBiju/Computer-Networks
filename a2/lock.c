#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

pthread_t tid[2];
pthread_mutex_t lock;
FILE* fp;

void* f(void* data){
    pthread_mutex_lock(&lock);
    fputs("Hi\n", fp);
    sleep(5);
    fputs("Hey\n", fp);
    pthread_mutex_unlock(&lock);
}

int main(){
    int i=0;
    int error;
    fp = fopen("file.txt", "w");
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex init has failed\n");
        return 1;
    }
    while(i<2){
        error = pthread_create(&(tid[i]), NULL, &f, NULL);
        if (error != 0)
            printf("\nThread can't be created : [%s]", strerror(error));
        i++;
    }
    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_mutex_destroy(&lock);
    fclose(fp);
}
