/*Hunter Russ   CSCE 4600  
  Program to store Cycles and Memory*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

/*Global Variable*/

int Sigma1 = 0;   int sigma1 = 0;
int Sigma2 = 0;   int sigma2 = 0;
int Sigma3 = 0;   int sigma3 = 0;
int Sigma4 = 0;   int sigma4 = 0;
int Sigma5 = 0;   int sigma5 = 0;
int Sigma6 = 0;   int sigma6 = 0;
int Sigma7 = 0;   int sigma7 = 0;
int Sigma8 = 0;   int sigma8 = 0;

int Timer = 0;
int Lock;  ////////////////////////////////////////for LL and BT

/*Linked List & Tree*/

typedef struct process {
   double Cycle;
   double Memory;
   struct process *Next;  } process;

pthread_mutex_t count_mutex;
process *Head;
struct process *End;
struct process *temp;
               
   
struct tree {
   int space;
   struct tree * Left;
   struct tree * Right; 
   int open; 
   struct tree *up; };

struct tree *Root;


/*Mutex Lock & Unlock*/

pthread_mutex_t free_mutex, s_free_mutex, malloc_mutex;


pthread_mutex_t count_mutex;
long long count;

void
increment_count()
{
        pthread_mutex_lock(&count_mutex);
    count = count + 1;
        pthread_mutex_unlock(&count_mutex);
}

long long
get_count()
{
    long long c;
    
    pthread_mutex_lock(&count_mutex);
        c = count;
    pthread_mutex_unlock(&count_mutex);
        return (c);
}      
                  
/*Standard Deviation*/

double SD (int min, int max, int mean, int counter, char unit, int k)  { 

   int sigma = (2 * mean - 2 * min) / 8;   /*create sigma*/

   int s0 = min;                  /*seperate sections of bell curve*/
   int s1 = min + 1 * sigma;
   int s2 = min + 2 * sigma;
   int s3 = min + 3 * sigma;
   int s4 = mean;
   int s5 = mean + 1 * sigma;
   int s6 = mean + 2 * sigma;
   int s7 = mean + 3 * sigma;
   int s8 = max ;

   double t1 = k * .002;   /*.2% Bell Curve*/
   double t2 = k * .021;   /*2.1% Bell Curve*/
   double t3 = k * .136;   /*13.6% Bell Curve*/
   double t4 = k * .341;   /*34.1% Bell Curve*/

   double n =  rand() % (max - min) + min;  /*Generate number between min and max*/

   if (unit == 'm')  {   /*Memory*/

      if (n >= s0 && n < s1 && sigma1 < t1)  {
         sigma1 += 1;
         return n;
      }
      else if (n >= s1 && n < s2 && sigma2 < t2)  {
         sigma2 += 1;
         return n;
      }        
      else if (n >= s2 && n < s3 && sigma3 < t3)   {
         sigma3 += 1;
         return n;
      }               
      else if (n >= s3 && n < s4 && sigma4 < t4)  {
         sigma4 += 1;
         return n;
      }
      else if (n >= s4 && n < s5 && sigma5 < t4)  {
         sigma5 += 1;
         return n;
      }
      else if (n >= s5 && n < s6 && sigma6 < t3)  {
         sigma6 += 1;
         return n;
      }
      else if (n >= s6 && n < s7 && sigma7 < t2)  {
         sigma7 += 1;
         return n;
      }
      else if (n >= s7 && n <= s8 && sigma8 < t1)  {
         sigma8 += 1;
         return n;
      }
      else 
         SD (min, max, mean, counter, unit, k);
    }

   else if (unit == 'c')  {   /*Cycle*/

      if (n >= s0 && n < s1 && Sigma1 < t1)  {
         Sigma1 += 1;
         return n;
      }
      else if (n >= s1 && n < s2 && Sigma2 < t2)  {
         Sigma2 += 1;
         return n;
      }
      else if (n >= s2 && n < s3 && Sigma3 < t3)   {
         Sigma3 += 1;
         return n;
      }
      else if (n >= s3 && n < s4 && Sigma4 < t4)  {
         Sigma4 += 1;
         return n;
      }
      else if (n >= s4 && n < s5 && Sigma5 < t4)  {
         Sigma5 += 1;
         return n;
      }
      else if (n >= s5 && n < s6 && Sigma6 < t3)  {
         Sigma6 += 1;
         return n;
      }
      else if (n >= s6 && n < s7 && Sigma7 < t2)  {
         Sigma7 += 1;
         return n;
      }
      else if (n >= s7 && n <= s8 && Sigma8 < t1)  {
         Sigma8 += 1;
         return n;
      }
      else
         SD (min, max, mean, counter, unit, k);
   }

}

//Binary Buddy Tree

void *BBT () {  

   int need, time, pos;
   int power = 24;
     
   //write lock
   pthread_mutex_lock(&s_free_mutex);
   pthread_mutex_lock(&malloc_mutex);

   need = Head -> Next -> Memory;
   time = Head -> Next -> Cycle;

   struct tree *nav;
   nav = (struct tree*) malloc (sizeof(struct tree));
   nav -> Left = Root;
      
   int start = -1;   
   
   while (nav -> Left -> open == 1)  {  //while not used
      while (need </*=*/ (2 ^ (power-1)))  {  //and not greater then next smallest
         nav -> Left -> Left = (struct tree*) malloc (sizeof(struct tree));
         nav -> Left -> Right = (struct tree*) malloc (sizeof(struct tree));
         //make left child
         nav -> Left -> Left -> space = 2 ^ (power-1);
         nav -> Left -> Left -> Left = NULL;
         nav -> Left -> Left -> Right = NULL;
         nav -> Left -> Left -> up = nav -> Left;
         nav -> Left -> Left -> open = 1;
         //make right child
         nav -> Left -> Right -> space = 2 ^ (power-1); 
         nav -> Left -> Right -> Left = NULL;
         nav -> Left -> Right -> Right = NULL;
         nav -> Left -> Right -> up = nav -> Left;
         nav -> Left -> Right -> open = 1;
         //move nav
         nav -> Right = nav -> Left;
         nav -> Left = nav -> Left -> Left;
         nav -> Right -> open = 0;
         power--;  }
/*if can put value in*/
      nav -> Left -> open = 0;

      }
      pthread_mutex_unlock(&malloc_mutex);
      pthread_mutex_unlock(&s_free_mutex);

      
      start = Timer;
      while ((Timer - start) < time);
           
/*read lock*/
      pthread_mutex_lock(&malloc_mutex);
      if (free_count == 0) {
          ++free_count;
          pthread_mutex_lock(&malloc_mutex);
      }
      else {
          ++free_count;
      }

      //mark space as not open(0) for 'time'
      //unlock control   &&
      pthread_mutex_unlock(&free_mutex);
      // free memory (read)
      //when 'time' is up, merge up the tree                  
      --free_count;
      if (free_count == 0) {
          pthread_mutex_unlock(&malloc_mutex);          
      }
      pthread_mutex_unlock(&free_mutex);
/*else
   put at end of linked list and unlock*/   
   
   
   //split BBT till a spot with correct size is open
   //if all closed put in end of LL
   //After placed un-lock for other trees
      //wait for time to go
   //end threat and repeat
   


}
              
//Timer Counter

void *Timer_counter () {

   while(1)     //cycles, use wait or big for loop
      Timer++;
    
}          
                                                                                                                                                                                  
//Main Program

int main ()  {
   free_mutex = 0;
   s_free_mutex = 0;
   malloc_mutex = 1;


   Head = (struct process*) malloc (sizeof(struct process));
   End = (struct process*) malloc (sizeof(struct process));
   temp = (struct process*) malloc (sizeof(struct process));

   End -> Next = Head;
   Head -> Next = End;

   Root = (struct tree*) malloc (sizeof(struct tree));
   Root -> Left = NULL;
   Root -> Right = NULL;
   Root -> up = NULL;
   Root -> open = 1;
   Root -> space = 2^24; //~16MB

   int counter;
   pthread_t t1, t2, t3, t4;

   //Linked List with 50 nodes holding memory and cycle values
   for (counter = 0; counter < 50; counter++)  {
      temp -> Next = Head -> Next;
      Head -> Next =(struct process*) malloc (sizeof(struct process));
      if (counter == 0)  {
         End -> Next = Head -> Next;
         Head -> Next -> Next = NULL;  }
      else  
         Head -> Next -> Next = temp -> Next;
      /*                         (Min,    Max,    Mean,   counter, Type, Processes)*/
      Head -> Next -> Cycle = SD (50,     550,    300,    counter, 'c',  50);/*# can change*/           
      Head -> Next -> Memory = SD(100000, 300000, 200000, counter, 'm',  50);
   }
   
   //Thread with timer counting cycles
   /*             (thread, NULL, What is running, Passing Var.)*/
   pthread_create (&t1,    NULL, Timer_counter,   NULL);
   //pthread_create ((&t2,    NULL, BBT,   NULL);
   //pthread_create ((&t3,    NULL, BBT,   NULL);
   //pthread_create ((&t4,    NULL, BBT,   NULL);
   
   //lock
      //take from LL            
      //put in buddy BT  
   
return 0;  }

