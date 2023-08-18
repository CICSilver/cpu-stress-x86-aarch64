#include <thread>
#include <string.h>
#include <stdio.h>
#include <iostream>
#ifndef _WIN32
#include <unistd.h>
#include <pthread.h>
#endif
#define CurrentTime_MilliSec std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()
#define HLINE "-------------------------------------------------------------\n"

typedef void (*stress_func)(int);

extern "C" void int_stress_cpu(int operations);
extern "C" void float_stress_cpu(int operations);
extern "C" void multi_stress(stress_func func);
extern "C" void single_stress(stress_func func);

void int_stress_cpu(int operations)
{
    int src = 1, step = 1;
#ifdef __aarch64__
// #define M1 asm volatile("add x0, x0, #1\n\t");
#define M1 asm volatile(    \
    "add x0, x0, x1\n\t"    \
);
#else
// #define M1 asm volatile("addl $1, %eax\n\t");
#define M1 asm volatile(    \
    "addl %eax, %ebx\n\t" \
);
#endif
#define M2 M1 M1 M1 M1 M1
#define M3 M2 M2 M2 M2 M2
#define M4 M3 M3 M3 M3 M3
#define M5 M4 M4 M4 M4 M4
#define M6 M5 M5 M5 M5 M5

#ifdef __aarch64__
    asm volatile(
        "mov x0, %0\n\t"
        "mov x1, %1\n\t"
        :
        :"r"(src), "r"(step)
    );
#else
    asm volatile(
        "movl $1, %eax\n\t"
        "movl $1, %ebx\n\t");
#endif
    for (int i = 0; i < operations; i++)
    {
      M6
    }
}

void float_stress_cpu(int operations)
{
    double src = 1.0, step = 1.0;
#ifdef __aarch64__
#define F1 asm volatile(    \
    "fadd s0, s0, s1\n\t"   \
);
// #define F1 asm volatile("fadd s0, s0, s1\n\t");
#else
#define F1 asm volatile(    \
    "fadds %0\n\t"  \
	:	\
	:"m"(step)	\
    );
// #define F1 asm volatile(	\
// 	"addss %xmm0, %xmm1\n\t"	\
// );
#endif

#define F2 F1 F1 F1 F1 F1
#define F3 F2 F2 F2 F2 F2
#define F4 F3 F3 F3 F3 F3
#define F5 F4 F4 F4 F4 F4
#define F6 F5 F5 F5 F5 F5

#ifdef __aarch64__
    // asm volatile(
    //     "fmov s0, 1.0e+0\n\t"
    //     "fmov s1, 1.0e+0\n\t"
    //     );
    asm volatile(
        "fmov s0, 1.0e+0\n\t"
        "fmov s1, 1.0e+0\n\t"
    );
#else
   asm volatile(
       "flds %0\n\t"
       :
       :"m"(src)
   );

	// asm volatile(
	// 	"movss %%xmm0, %0\n\t"
	// 	"movss %%xmm1, %1\n\t"
	// 	:
	// 	:"m"(src),"m"(step)
	// );	
#endif
    for (int i = 0; i < operations; i++)
    {
        F6
    }
}

unsigned long long start_ms, end_ms;
const int MAX_THREADS = std::thread::hardware_concurrency();
int threads_active;
const long OPERA_CNT = 10000000UL;
#define SCORE(THREAD_COUNT) (unsigned long)(OPERA_CNT / (end_ms - start_ms) * THREAD_COUNT)

void *thread(void *f)
{
    stress_func func = *(stress_func *)f;
    func(OPERA_CNT);
    threads_active--;
    return NULL;
}

void multi_stress(stress_func func)
{
    if (MAX_THREADS > 1)
    {
        threads_active = 0;
        start_ms = CurrentTime_MilliSec;
        for (int i = 1; i < MAX_THREADS; ++i)
        {
            ++threads_active;
            pthread_t tid;
            pthread_create(&tid, NULL, thread, &func);
        }
        func(OPERA_CNT);
        while (threads_active)
            sleep(1);
        end_ms = CurrentTime_MilliSec;
        std::cout << "using time :" << (end_ms - start_ms) / 1000.0 << "s "
                  << " multi-core score: " << SCORE(MAX_THREADS) << std::endl;
    }
}

void single_stress(stress_func func)
{
    start_ms = CurrentTime_MilliSec;
    func(OPERA_CNT);
    end_ms = CurrentTime_MilliSec;
    std::cout << "using time :" << (end_ms - start_ms) / 1000.0 << "s "
              << " single-core score: " << SCORE(1) << std::endl;
}

int main(int argc, char *argv[])
{
    bool isFloat = false;
    bool isAll = !(argc > 1);
    if (argc > 1)
    {
        char *str = argv[1];
        isFloat = (strcmp(str, "-float") == 0);
        isAll = (strcmp(str, "-a") == 0 || strcmp(str, "-all") == 0);
    }

    if (!isAll)
    {
        stress_func func = isFloat ? float_stress_cpu : int_stress_cpu;
        printf(HLINE);
        multi_stress(func);
        printf(HLINE);
        single_stress(func);
        printf(HLINE);
    }
    else
    {
        printf(HLINE);
        // float stress
        std::cout << "float cpu stress running with " << MAX_THREADS << " threads." << std::endl;
        multi_stress(float_stress_cpu);
        single_stress(float_stress_cpu);
        printf(HLINE);
        // int stress
        std::cout << "int cpu stress running....." << std::endl;
        multi_stress(int_stress_cpu);
        single_stress(int_stress_cpu);
        printf(HLINE);
    }

    return 0;
}
