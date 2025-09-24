#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *thread_function(void* arg){
    int tid = pthread_self();
    printf("Đang chạy Theard với ID: %d\n",tid);
    sleep(2);
    printf("Kết thúc Thread với ID: %d\n",tid);
    return NULL;
}

int main(int argc, char const *argv[]){
    pthread_t thread_1 ,thread_2;

    int ret_val;
    //tạo thread 1
    if ((ret_val = pthread_create(&thread_1,NULL,&thread_function,NULL)) != 0){
        printf("Khởi tạo thread 1 thất bại\n");
        return 1;
    }
    //tạo thread 2
    if ((ret_val = pthread_create(&thread_2,NULL,&thread_function,NULL)) != 0){
        printf("Khởi tạo thread 2 thất bại\n");
        return 1;
    }

    //Chờ hai thread kết thúc
    pthread_join(thread_1,NULL);
    pthread_join(thread_2,NULL);

    printf("Kết thúc chương trình\n");

    return 0;
}

// Các thành phần trong pthread_create(con trỏ trỏ đến địa chỉ ID mới, thuộc tính của luồng, hàm luồng sẽ thực thi, tham số truyền vào)
// Luồng khi được tạo thành công sẽ trả về giá trị 0
// Một luồng sẽ kết thúc khi gọi ra các lệnh như pthread_exit(), exit(),thread_cancle(). Sau khi các luồng kết thúc nên gọi hàm
// pthread_join() để tránh tình trạng zombie