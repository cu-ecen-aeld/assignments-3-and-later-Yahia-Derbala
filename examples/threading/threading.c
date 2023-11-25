#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

void* threadfunc(void* thread_param) {
    struct thread_data* thread_func_args = (struct thread_data*)thread_param;

    // Sleep to wait before obtaining the mutex
    usleep(thread_func_args->wait_to_obtain_ms * 1000);

    // Attempt to lock the mutex
    if (pthread_mutex_lock(thread_func_args->mutex) != 0) {
        perror("pthread_mutex_lock");
        thread_func_args->thread_complete_success = false;
        return thread_param;
    }

    // Sleep to hold the mutex
    usleep(thread_func_args->wait_to_release_ms * 1000);

    // Unlock the mutex
    if (pthread_mutex_unlock(thread_func_args->mutex) != 0) {
        perror("pthread_mutex_unlock");
        thread_func_args->thread_complete_success = false;
        return thread_param;
    }

    // Operation completed successfully
    thread_func_args->thread_complete_success = true;
    return thread_param;
}

bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex, int wait_to_obtain_ms, int wait_to_release_ms) {
    // Allocate memory for thread_data structure
    struct thread_data *data = (struct thread_data*)malloc(sizeof(struct thread_data));
    if (data == NULL) {
        perror("Memory allocation failed");
        return false;
    }

    // Set up thread_data structure
    data->mutex = mutex;
    data->wait_to_obtain_ms = wait_to_obtain_ms;
    data->wait_to_release_ms = wait_to_release_ms;
    data->thread_complete_success = false;

    // Create a new thread
    if (pthread_create(thread, NULL, threadfunc, (void*)data) != 0) {
        perror("pthread_create");
        free(data);
        return false;
    }

    return true;
}
