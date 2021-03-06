/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"


/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements. 
*/
typedef struct _job_t
{
  int job_number; 
  int time_start; 
  int running_time; 
  int priority;

  int time_stop;//for preemptive
  int just_switched;
  int core_number;
} job_t;

typedef struct _core_t
{
  int core_number;
  int available;
  job_t *job;
} core_t;

priqueue_t job_queue;
priqueue_t core_queue;
// int *core_array;
// int num_cores;

int time_current;

int fcfs  (const void* a, const void* b);
int sjf   (const void* a, const void* b);
int psjf  (const void* a, const void* b);
int pri   (const void* a, const void* b);
int ppri  (const void* a, const void* b);
int rr    (const void* a, const void* b);

int fcfs(const void* a, const void* b){
  // job_t* a_2 = (job_t*)a;
  // job_t* b_2 = (job_t*)b;
  // return a_2->time_start - b_2->time_start;
  return 1;
}

int sjf(const void* a, const void* b){
  job_t* a_2 = (job_t*)a;
  job_t* b_2 = (job_t*)b;
  if(b_2->core_number > -1)//running so let it be
    return 1;
  else//not been started before, just compare running_time
    return a_2->running_time - b_2->running_time;
}

int psjf(const void* a, const void* b){
  job_t* a_2 = (job_t*)a;
  job_t* b_2 = (job_t*)b;

  //FIXED CODE??
  //if(b_2->core_number > -1 && b_2->just_switched == 0)//just_switched is toggled to the core_number when a job_finishes and a core switches 
  //  b_2->running_time--;
  //else if(b_2->just_switched == 1)
  //  just_switched = 0;
  //return a_2->running_time - b_2->running_time;


  // if(b_2->time_stop > -1)//it has been stopped before
  //   return a_2->running_time - (b_2->running_time - (b_2->time_stop - b_2->time_start));
  // else if(b_2->time_start > -1)//its been started
  //   return a_2->running_time - (b_2->running_time - (time_current - b_2->time_start));
  // else//not been started before, just compare running_time
    // return a_2->running_time - b_2->running_time;

  if(b_2->time_stop >= b_2->time_start)//it has been stopped before and the new running_time has been calculated
    return a_2->running_time - b_2->running_time;
  if(b_2->time_start > -1)//its been started, its running?
    return a_2->running_time - (b_2->running_time - (time_current - b_2->time_start));
  return a_2->running_time - b_2->running_time;//not been started before, just compare running_time
}

int pri(const void* a, const void* b){
  job_t* a_2 = (job_t*)a;
  job_t* b_2 = (job_t*)b;
  if(b_2->core_number > -1)//running so let it be
    return 1;
  return a_2->priority - b_2->priority;
}

int ppri(const void* a, const void* b){
  job_t* a_2 = (job_t*)a;
  job_t* b_2 = (job_t*)b;
  return a_2->priority - b_2->priority;
}

int rr(const void* a, const void* b){
  // job_t* a_2 = (job_t*)a;
  // job_t* b_2 = (job_t*)b;
  // return -1;
  return 1;
}


/**
  Initalizes the scheduler.
 
  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called once once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
  // priqueue_init(&queue, NULL);
  // priqueue_offer(&queue, 69);
  // priqueue_offer(&queue, 69);
  // priqueue_offer(&queue, 420);
  // priqueue_offer(&queue, 69);
  // priqueue_offer(&queue, 420);
  // priqueue_offer(&queue, 69);
  // priqueue_offer(&queue, 420);
  // priqueue_offer(&queue, 69);
  // printf("TEST%dSIZE\n",(queue.size));
  // printf("TEST%dAT2\n",priqueue_node_at(&queue,1));
  // printf("TEST%dREM_ALL69\n",priqueue_remove(&queue,69));
  // printf("TEST%dSIZE\n",(queue.size));
  // printf("TEST%dAT2\n",priqueue_node_at(&queue,0));
  // printf("TEST%dSIZE\n",(queue.size));
  // printf("TEST%dPOLL\n",priqueue_poll(&queue));
  // printf("TEST%dSIZE\n",(queue.size));
  // //printf("TEST%dDATA\n",(queue.head->data));
  // printf("TEST%dPOLL\n",priqueue_poll(&queue));
  // printf("TEST%dSIZE\n",(queue.size));
  // printf("TEST%dPOLL\n",priqueue_poll(&queue));
  // printf("TEST%dTEST",priqueue_poll(&queue));

  // num_cores = cores;
  // core_array = calloc(cores, sizeof(int));
  // int *p = core_array;
  // for(int i = 0; i < cores; ++i){
  //   *(p + i) = 1;
  // }

  priqueue_init(&core_queue, NULL);
  for(int i = 0; i < cores; ++i){
    core_t *core = malloc(sizeof(core_t));
    core->core_number = i;
    core->available = 1;
    core->job = NULL;
    priqueue_offer(&core_queue, core);
  }


  switch(scheme){
    case FCFS:
      priqueue_init(&job_queue, fcfs);
      break;
    case SJF:
      priqueue_init(&job_queue, sjf);
      break;
    case PSJF:
      priqueue_init(&job_queue, psjf);
      break;
    case PRI:
      priqueue_init(&job_queue, pri);
      break;
    case PPRI:
      priqueue_init(&job_queue, ppri);
      break;
    case RR:
      priqueue_init(&job_queue, rr);
      break;
  }

  job_queue.ptr_flag = 1;
  core_queue.ptr_flag = 1;
}


/**
  Called when a new job arrives.
 
  If multiple cores are idle, the job should be assigned to the core with the
  lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made. 
 
 */
int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
  time_current = time;

  job_t* job = malloc(sizeof(job_t));
  job->job_number = job_number;
  job->time_start = -1;//never run before
  job->running_time = running_time;
  job->priority = priority;
  job->time_stop = -1;//never run before
  int idx = priqueue_offer(&job_queue, job);

  //find the first available core index
  // int *p = core_array;
  // for(int i = 0; i < num_cores; ++i){
  //   if(*(p + i) == 1){
  //     *(p + i) = 0;
  //     return i;
  //   }
  // }
  node *i = core_queue.head;
  while(i){
    core_t *core = i->data;
    if(core->available){
      core->available = 0;
      core->job = job;
      job->core_number = core->core_number;
      job->time_start = time;
      return core->core_number;
    }
    i = i->next;
  }

  //FOR PREEMPTIVE ALGO: SOMEWHERE IN THIS FUNCTION NEED TO INSERT NEW JOB ON QUEUE, CHECK QUEUE --IF JOB IS IN FRONT OF ANY JOBS THAT AREN'T IDLE-- AND SEE IF THE JOB IS IDLE, SET THE LAST-MOST NON-IDLE JOB TO IDLE (the last job with core_number > 0)
  //core not found, check if new job is in front of already-running jobs and need to kick some other job off a core
  i = priqueue_node_at(&job_queue, idx);//get new job's idx
  if(i->next){//in front of jobs (don't know if they are running or not yet)

    job_t *job_2 = i->next->data;
    if(job_2->core_number > -1){//if in front of already-running jobs
      i = i->next;
      node *i_prev = i;
      while(i){
        job_2 = i->data;
        if(job_2->core_number < 0){//prev one is not idle
          // //look at prev job (which is not idle)
          // job_2 = i_prev->data;
          // //take prev job's core and put new job on it instead
          // job->core_number = job_2->core_number;
          // job_2->core_number = -1;
          // //mark time as the time last run for prev job
          // job->time_start = time;
          // job_2->time_stop = time;
          // //get the core to put the new job on
          // i = priqueue_node_at(&core_queue, job->core_number);
          // core_t *core = i->data;
          // core->job = job;
          // return core->core_number;
          break;
        }
        i_prev = i;
        i = i->next;
      }
      //reached the last idx and they're all running, so kick the last one off
      job_2 = i_prev->data;
      //take prev job's core and put new job on it instead
      job->core_number = job_2->core_number;
      job_2->core_number = -1;
      //mark time as the time last run for prev job
      job->time_start = time;
      if(job_2->time_start > -1){//if its been run before
        job_2->time_stop = time;
        //update the running time
        job_2->running_time -= (job_2->time_stop - job_2->time_start);
      }
      //get the core to put the new job on
      i = priqueue_node_at(&core_queue, job->core_number);
      core_t *core = i->data;
      core->job = job;
      return core->core_number;
    }
    else{//not in front of any running jobs
      job->core_number = -1;
      return -1;
    }
  }

  job->core_number = -1;
	return -1;
}


/**
  Called when a job has completed execution.
 
  The core_id, job_number and time parameters are provided for convenience. You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.
 
  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
  time_current = time;

  // int *p = core_array;
  // *(p + core_id) = 1;
  node *i = priqueue_node_at(&core_queue, core_id);
  core_t *core = i->data;
  core->available = 1;

  priqueue_remove(&job_queue, core->job);
  free(core->job);
  core->job = NULL;

  //find first job that isn't running in job_queue (i.e. idle)
  //put that job on core
  i = job_queue.head;
  while(i){
    job_t *job = i->data;
    if(job->core_number < 0){//if idle
      core->available = 0;
      core->job = job;
      job->core_number = core->core_number;
      job->time_start = time;
      return job->job_number;
    }
    i = i->next;
  }

	return -1;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.
 
  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator. 
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
    time_current = time;


  core_t *core = priqueue_at(&core_queue, core_id);

  //poll the front of the queue, move the job to the back of the queue, find the first idle job, switch core_number between first idle job and job at the back of the queue
  job_t *job_expired = core->job;//priqueue_poll(&job_queue);
  priqueue_remove(&job_queue, job_expired);
  priqueue_offer(&job_queue, job_expired);
  node *i = job_queue.head;
  while(i){
    job_t *job = i->data;
    if(job->core_number < 0){
      job->core_number = job_expired->core_number;
      job_expired->core_number = -1;
      core->job = job;
      return job->job_number;
    }
    i = i->next;
  }

  return job_expired->job_number;

  return -1;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	return 0.0;
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return 0.0;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return 0.0;
}


/**
  Free any memory associated with your scheduler.
 
  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{
  // node *i = job_queue.head;
  // while(i){
  //   node *i_free = i;
  //   i = i->next;
  //   free(i_free->data);
  //   free(i_free);
  // }

  // i = core_queue.head;
  // while(i){
  //   node *i_free = i;
  //   i = i->next;
  //   free(i_free->data);
  //   free(i_free);
  // }
  priqueue_destroy(&job_queue);
  priqueue_destroy(&core_queue);
}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)  
  
  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
  node *i = job_queue.head;
  while(i){
    job_t *job = i->data;
    printf("%d(%d) ", job->job_number, job->core_number);
    i = i->next;
  }
  printf("\n");
}