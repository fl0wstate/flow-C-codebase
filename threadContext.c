#include <malloc.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define N 130

pthread_key_t key;
/* pthread_key_t keyb; */
pthread_once_t once = PTHREAD_ONCE_INIT;
/* destructor function that will be called each time the thread function is
 * closed */
void desctructor(void *ptr) { free(ptr); }

/* ensures that each thread is initialized with it's own static global vairable
 * which can be accessed through pthread_getspecific(key) &
 * pthread_setspecific(key) */
void key_create()
{
  pthread_key_create(&key, desctructor);
  /* pthread_key_create(&keyb, desctructor); */
}

/* function pointer that will be runned as a routine in the thread */
void *strerror1(void *ptr)
{

  int *n;
  n = (int *)ptr;
  char *str;

  /* call that function only once */
  pthread_once(&once, key_create);

  /* acquire that global variable that was created immediately the thread become
   * of existence */
  str = pthread_getspecific(key);

  /* if the value is NULL allocate memory to be able to hold the value to be
   * stored over there */
  if (str == NULL)
  {
    str = malloc(sizeof(char) * 50);
    pthread_setspecific(key, str);
  }

  /* to stop overwritting the actual data each time we call the a new thread we
   * need to ensure that we are only copying the contents */
  strcpy(str, strerror(*n));
  printf("[%d]:%s\n", *n, str);
  return NULL;
}

int main()
{

  int i = 0;
  int err[N];
  pthread_t thread[N];

  for (i = 0; i < N; i++)
  {
    err[i] = i;
    pthread_create(&thread[i], NULL, strerror1, (void *)&err[i]);
  }

  for (i = 0; i < N; i++)
  {
    pthread_join(thread[i], NULL);
  }

  return 0;
}
