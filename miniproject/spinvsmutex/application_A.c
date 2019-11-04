//Name: svm2.c
//Source: http://www.solarisinternals.com/wiki/index.php/DTrace_Topics_Locks
//Compile(spin lock version): gcc -o spin -DUSE_SPINLOCK svm2.c -lpthread
//Compile(mutex version): gcc -o mutex svm2.c -lpthread
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/syscall.h>
 
#define        THREAD_NUM     2
 
pthread_t g_thread[THREAD_NUM];
#ifdef USE_SPINLOCK
  pthread_spinlock_t g_spin;
#else
  pthread_mutex_t g_mutex;
#endif
  
__uint64_t g_count;
 
pid_t gettid()
{
    return syscall(SYS_gettid);
}
 
void *run_amuck(void *arg)
{
  printf("Thread %lu started.n", (unsigned long)gettid());

  while(1){
    #ifdef USE_SPINLOCK
      pthread_spin_lock(&g_spin);
    #else
      pthread_mutex_lock(&g_mutex);
    #endif
    if (g_count== 123456789){
      #ifdef USE_SPINLOCK
        pthread_spin_unlock(&g_spin);
      #else
        pthread_mutex_unlock(&g_mutex);
      #endif
      break;

    }
    g_count++;               
       
    #ifdef USE_SPINLOCK
      pthread_spin_unlock(&g_spin);
    #else
      pthread_mutex_unlock(&g_mutex);
    #endif
   }
   
  printf("Thread %lu finished!n", (unsigned long)gettid());

  return (NULL);
}
 
int main(int argc, char *argv[])
{
  int i, threads = THREAD_NUM;

  printf("Creating %d threads...n", threads);
  #ifdef USE_SPINLOCK
    pthread_spin_init(&g_spin, 0);
  #else
    pthread_mutex_init(&g_mutex, NULL);
  #endif
  for (i = 0; i < threads; i++)
    pthread_create(&g_thread[i], NULL, run_amuck, (void *) i);

  for (i = 0; i < threads; i++)
    pthread_join(g_thread[i], NULL);

  printf("Done.n");

  return (0);

}