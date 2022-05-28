// scheduler.h 
//	Data structures for the thread dispatcher and scheduler.
//	Primarily, the list of threads that are ready to run.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "copyright.h"
#include "list.h"
#include "thread.h"

// The following class defines the scheduler/dispatcher abstraction -- 
// the data structures and operations needed to keep track of which 
// thread is running, and which threads are ready but not running.

enum SchedulerType {
        RR,     // Round Robin
        SJF,
        Priority
};

class Scheduler {
  public:
	Scheduler();		// Initialize list of ready threads 
	~Scheduler();				// De-allocate ready list

	void ReadyToRun(Thread* thread);	
    					// Thread can be dispatched.
	Thread* FindNextToRun();	// Dequeue first thread on the ready 
					// list, if any, and return thread.
	void Run(Thread* nextThread, bool finishing);
	    				// Cause nextThread to start running
	void CheckToBeDestroyed();	// Check if thread that had been
    					// running needs to be deleted
	void Print();			// Print contents of ready list
   
    // SelfTest for scheduler is implemented in class Thread
<<<<<<< HEAD

	
    
  private:
	SchedulerType schedulerType;
	// List<Thread *> *readyList;	// queue of threads that are ready to run,but not running
	SortedList<Thread *> *readyList;
	//<TODO>
	//Variable definition of sorting rule of readyQueue
	
	
=======
	int setBurstTime(int burstTime){
		return this->nowBurstTime = burstTime;
	}

	int setpreviousPrediction(int previousPrediction){
		return this->previousPrediction = previousPrediction;
	}

	inline int getBurstTime(){
		return this->nowBurstTime;
	}

	inline int getPreviousPrediction(){
		return this->previousPrediction;
	}

    
  private:
	SchedulerType schedulerType;
	// List<Thread *> *readyList;	// queue of threads that are ready to run, but not running

	SortedList<Thread *> *readyList;

	//<TODO>
	//Variable definition of sorting rule of readyQueue
	int previousPrediction; //T - 1
	int nowBurstTime;		//T
>>>>>>> 1f921c96d033756b65da6832577f736a0705acb9
	//<TODO>

	Thread *toBeDestroyed;		// finishing thread to be destroyed
    					// by the next thread that runs
};

#endif // SCHEDULER_H
