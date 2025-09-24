#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 3
#define NUM_COUNT_INCREASE 1000000000 //1 triệu

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;

long long counter = 0;

void *thread_function(void* arg){
    pthread_mutex_lock(&lock1);
    for (int i=0; i<NUM_COUNT_INCREASE;i++){
        counter++;
        printf("Giá trị biến đếm: %lld\n",counter);
    }
    pthread_mutex_unlock(&lock1);
    return NULL;
}

int main(int argc, char const* argv[]){

    pthread_t thread_[NUM_THREADS];

    for (int i=0; i<NUM_THREADS;i++){
        if (pthread_create(&thread_[i],NULL,thread_function,NULL) != 0){
            printf("Khởi tạo luồng %d thất bại",i);
        }
    }

    for (int i=0; i<NUM_THREADS;i++){
        pthread_join(thread_[i],NULL);
        return 1;
    }
    
    return 0;
}

//trong chương trình cần mutex là vì khi khởi tạo nhiều luồng như vậy nhwung lại dùng chung một memory có thể dẫn đến việc lấy dữ liệu
//sẽ không còn chính xác nữa. Vì vậy để tránh tình trạng như vậy thì mutex sẽ giúp cho dữ liệu trở nên đồng nhất. Tuy nhiên kết quả có thể
//vẫn có thể không chính xác là do có thể ở lần cuối cùng mặc dù đã tới giá trị cần đếm nhưng các luồng vẫn cứ chạy, làm cho giá trị biến đếm
//vẫn tăng lên.