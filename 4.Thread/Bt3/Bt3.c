#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

#define NUM_ITEMS 10

int data;
int data_flag = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* prod_function(void* arg){
    for (int i=0; i<NUM_ITEMS;i++){
        int num = rand() % 100;
        printf("Giá trị khởi tạo: %d\n",num);

        pthread_mutex_lock(&mutex);
        data = num;
        data_flag = 1;

        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}

void* cus_function(void* arg){
    for (int i=0; i<NUM_ITEMS;i++){

        pthread_mutex_lock(&mutex);

        while (data_flag == 0){
            pthread_cond_wait(&cond,&mutex);
        }
        printf("Giá trị nhận được bên Cus là: %d\n",data);

        data_flag = 0;

        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main(int argc, char const *argv[]){
    pthread_t prod_thread, cus_thread;

    if (pthread_create(&prod_thread,NULL,prod_function,NULL) != 0){
        printf("Khởi tạo prod_thread thất bại");
    }

    if (pthread_create(&cus_thread,NULL,cus_function,NULL) != 0){
        printf("Khởi tạo cus_thread thất bại");
    }

    pthread_join(prod_thread,NULL);
    pthread_join(cus_thread,NULL);

    return 0;
}