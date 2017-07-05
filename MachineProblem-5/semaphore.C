/* 
    File: semaphore.C

            Department of Computer Science
            Texas A&M University
    Date  : 08/02/11

*/

/*--------------------------------------------------------------------------*/
/* DEFINES */
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* INCLUDES */
/*--------------------------------------------------------------------------*/

#include "semaphore.H"

/*--------------------------------------------------------------------------*/
/* DATA STRUCTURES */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* FORWARDS */ 
/*--------------------------------------------------------------------------*/

/* -- (none) -- */

/*--------------------------------------------------------------------------*/
/* PUBLIC METHODS FOR CLASS   S e m a p h o r e  */
/*--------------------------------------------------------------------------*/

int Semaphore::P(){
	
	int e = pthread_mutex_lock(&m);
	if(e != 0){
		return e;
	}
	while(value <= 0)	{
		e = pthread_cond_wait(&c, &m);
		if(e != 0){
			return e;
		}
	}
	value--;
	e = pthread_mutex_unlock(&m);
	if(e != 0){
		return e;
	}
	return 0;
	
	
}

int Semaphore::V(){
	
	int e = pthread_mutex_lock(&m);
	if(e != 0){
		return e;
	}
	value++;
	e = pthread_cond_broadcast(&c);
	if(e != 0){
		return e;
	}
	e = pthread_mutex_unlock(&m);
	if(e != 0){
		return e;
	}
	return 0;
	
}

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

Semaphore::Semaphore(int v){
	pthread_mutex_init(&m, NULL);
	pthread_cond_init(&c, NULL);
	value = v;
}

Semaphore::~Semaphore(){
	pthread_mutex_destroy(&m);
	pthread_cond_destroy(&c);
}