#include <pthread.h>
#include <sched.h>
#include <iostream>
#include <atomic>
#include <cstdlib>
#include <chrono>
#include <sys/mman.h>


alignas(64) std::atomic<int> current_value;

struct alignas(64) to_thread{

    int iterations; // number of thread iterations
    int value; // value the thread wants to set

};

void* thread_work(void* arg){

    struct to_thread* t = (struct to_thread*)(arg);

    while(t->iterations--){

        while(true){
            if(t->value != current_value.load(std::memory_order_acquire)){
            
                current_value.store(t->value, std::memory_order_release);
                break;
            }
            __builtin_ia32_pause();   
        }

    }
    return nullptr;
}

int main(int argc, char* argv[]){

    if(argc < 2){
        printf("Usage ./ping_pong <iterations>\n");
        return EXIT_FAILURE;
    }
    
    if (mlockall(MCL_CURRENT | MCL_FUTURE) != 0) {
        std::cout << "mlockall error\n";
        return EXIT_FAILURE;
    }
    
    int iterations = atoi(argv[1]);

    pthread_attr_t attra, attrb;
    pthread_attr_init(&attra);
    pthread_attr_init(&attrb);
    
    cpu_set_t cpu_set_a, cpu_set_b;
    CPU_ZERO(&cpu_set_a);
    CPU_ZERO(&cpu_set_b);

    CPU_SET(4, &cpu_set_a);
    CPU_SET(6, &cpu_set_b);
    pthread_attr_setaffinity_np(&attra, sizeof(cpu_set_t), &cpu_set_a);
    pthread_attr_setaffinity_np(&attrb, sizeof(cpu_set_t), &cpu_set_b);
    

    pthread_t a_wait,b_wait;

    std::cout<<"Starting..." << std::endl;

    struct to_thread a;
    struct to_thread b;

    a.iterations = b.iterations = iterations;
    a.value = 0;
    b.value = 1;

    auto start = std::chrono::high_resolution_clock::now();

    pthread_create(&a_wait, &attra, thread_work, (void*)&a);
    pthread_create(&b_wait, &attrb, thread_work, (void*)&b);

    pthread_join(a_wait, nullptr);
    pthread_join(b_wait, nullptr);

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();

    std::cout<< "Execution time: " << duration << " Avg per ping-pong: " << (double)duration/(double)iterations<< std::endl;

    return EXIT_SUCCESS;

}   