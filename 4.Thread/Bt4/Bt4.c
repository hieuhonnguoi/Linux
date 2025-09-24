#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int share_count = 0;
pthread_rwlock_t rwlock;

#define NUM_READ 5
#define NUM_WRITE 2

void* read_function(void* arg){

    int id = *((int*)arg);
    for (int i=0; i<NUM_READ;i++){
        pthread_rwlock_rdlock(&rwlock);
        printf("Giá trị đọc được là: %d\n",share_count);
        pthread_rwlock_unlock(&rwlock);
        sleep(2);
    }
    return NULL;
}

void* write_function(void* arg){

    int id =*((int*)arg);
    for (int i=0; i<NUM_WRITE; i++){
        pthread_rwlock_wrlock(&rwlock);
        share_count++;
        printf("Dữ liệu vừa được ghi là: %d\n",share_count);
        pthread_rwlock_unlock(&rwlock);
        sleep(2);
    }
}

int main(int argc, char const* argv[]){
    pthread_t thread_read[NUM_READ], thread_write[NUM_WRITE];
    int reader_data[NUM_READ],writer_data[NUM_WRITE];

    pthread_rwlock_init(&rwlock,NULL);

    for (int i=0; i<NUM_READ;i++){
        if(pthread_create(&thread_read[i],NULL,read_function,&reader_data[i]) !=0){
            printf("Khởi tạo thất bại\n");
        }
    }

    for (int i=0; i<NUM_WRITE;i++){
        if(pthread_create(&thread_write[i],NULL,write_function,&writer_data[i]) !=0){
            printf("Khởi tạo thất bại\n");
        }
    }
    
    for (int i=0; i<NUM_READ;i++){
        pthread_join(thread_read[i],NULL);
    }

    for (int i=0; i<NUM_WRITE;i++){
        pthread_join(thread_write[i],NULL);
    }

    pthread_rwlock_destroy(&rwlock);

    printf("Chương trình kết thúc: %d\n",share_count);
    return 0;
}