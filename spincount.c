/*
Simple POSIX thread and spinlock example using asm(), xchg.
for Operating Systems Bonus 3, University of Electro-Communications
by Jesse Jennings
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
int testnset(int *pTargetAddress, int nValue);
void *add();
void *load();
void SpinLock();
void SpinUnlock();
int counter = 0;
int m_s = 0;
pthread_spinlock_t lock;
void SpinLock()
{
       int prev_s;
       do
       {
               prev_s = testnset(&m_s,1);
               if(m_s == 0 && prev_s == 1)
               {
                       break;
               }
       }
       while (1);
}
void SpinUnlock()
{
       testnset(&m_s, 1);
}
main()
{
   /* Create independent threads each of which will execute function */

       pthread_t thr1, thr2, thr3, thr4, thr5, thr6, thr7, thr8, thr9, thr0;

       pthread_create( &thr1, NULL, &load, NULL);
       pthread_create( &thr2, NULL, &load, NULL);
       pthread_create( &thr3, NULL, &load, NULL);
       pthread_create( &thr4, NULL, &load, NULL);
       pthread_create( &thr5, NULL, &load, NULL);
       pthread_create( &thr6, NULL, &load, NULL);
       pthread_create( &thr7, NULL, &load, NULL);
       pthread_create( &thr8, NULL, &load, NULL);
       pthread_create( &thr9, NULL, &load, NULL);
       pthread_create( &thr0, NULL, &load, NULL);

    /* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */

    pthread_join( thr1, NULL);
    pthread_join( thr2, NULL);
    pthread_join( thr3, NULL);
    pthread_join( thr4, NULL);
    pthread_join( thr5, NULL);
    pthread_join( thr6, NULL);
    pthread_join( thr7, NULL);
    pthread_join( thr8, NULL);
    pthread_join( thr9, NULL);
    pthread_join( thr0, NULL);

    printf("Total sum equals: %d\n",counter);
       exit(0);
}
int testnset(int *lockAddress, int one)
{
       int a = *lockAddress; int b = one;
       int out;
   asm
   (
       "movl %0, %%edx         ;\n"
       "movl %1, %%eax         ;\n"
       "xchg %%edx, %%eax      ;\n"
       :"=r"(out)
       :"r"(a),"r"(b)

   );
       return out != 0;
   // mov = 1 CPU cycle
   // xchg = 3 CPU cycles
}
void *load()
{
       pthread_t      tid = pthread_self();

       printf("->Thread [%d] Start Count\n", (tid));

       int i=0;
       for(i;i<100000;i++){
               add();
       }
       printf("<-Thread [%d] Count Finish (%d)\n",(tid),counter);
}
void *add()
{
       SpinLock();
       counter++;
       SpinUnlock();
}