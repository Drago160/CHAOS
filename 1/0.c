#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static void* thread_func(void* arg) {
    int val;
    if (scanf("%d ", &val) != 1) {
        return NULL;
    }
    pthread_t new_thread;
    pthread_create(&new_thread, NULL, thread_func, NULL);
    pthread_join(new_thread, NULL);
    printf("%d ", val);
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, thread_func, NULL);
    pthread_join(thread,NULL);
    return 0;
}
