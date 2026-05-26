#include <pthread.h>
#include <sched.h>
#include <iostream>
#include <atomic>
#include <cstdlib>
#include <chrono>

std::atomic<int> current_value;

struct to_thread{

    int iterations; // number of thread iterations
    int value; // value the thread wants to set

};

void* thread_work(void* arg){

    struct to_thread* t = (struct to_thread*)(arg);

    while(t->iterations--){

        while(true){
            if(t->value != current_value){
                current_value = t->value;
                break;
            }
            
        }

    }
    return nullptr;
}

int main(int argc, char* argv[]){

    if(argc < 2){
        printf("Usage ./ping_pong <iterations>");
        return EXIT_FAILURE;
    }

    int iterations = atoi(argv[1]);

    pthread_attr_t attr;
    pthread_attr_init(&attr);

    pthread_t a_wait,b_wait;

    std::cout<<"Starting..." << std::endl;

    struct to_thread a;
    struct to_thread b;

    a.iterations = b.iterations = iterations;
    a.value = 0;
    b.value = 1;

    auto start = std::chrono::high_resolution_clock::now();

    pthread_create(&a_wait, &attr, thread_work, (void*)&a);
    pthread_create(&b_wait, &attr, thread_work, (void*)&b);

    pthread_join(a_wait, nullptr);
    pthread_join(b_wait, nullptr);

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();

    std::cout<< "Execution time: " << duration << " Avg per ping-pong: " << (double)duration/(double)iterations<< std::endl;

    return EXIT_SUCCESS;

}   