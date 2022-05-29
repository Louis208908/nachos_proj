// scheduler.cc 
//	Routines to choose the next thread to run, and to dispatch to
//	that thread.
//
// 	These routines assume that interrupts are already disabled.
//	If interrupts are disabled, we can assume mutual exclusion
//	(since we are on a uniprocessor).
//
// 	NOTE: We can't use Locks to provide mutual exclusion here, since
// 	if we needed to wait for a lock, and the lock was busy, we would 
//	end up calling FindNextToRun(), and that would put us in an 
//	infinite loop.
//
// 	Very simple implementation -- no priorities, straight FIFO.
//	Might need to be improved in later assignments.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "debug.h"
#include "scheduler.h"
#include "main.h"

//----------------------------------------------------------------------
// Scheduler::Scheduler
// 	Initialize the list of ready but not running threads.
//	Initially, no ready threads.
//----------------------------------------------------------------------


//<TODO>
// Declare sorting rule of SortedList
// Hint: Funtion Type should be "static int"
static int cmp(Thread *a, Thread *b);
//<TODO>
int SJFcmp(Thread* a, Thread *b){


    if(a->getPredictedBurstTime() != b->getPredictedBurstTime())
        return a->getPredictedBurstTime() > b->getPredictedBurstTime();
    return a->getID() < b->getID();
}
//<TODO>
// Initialize ReadyQueue
Scheduler::Scheduler()
{
	//	schedulerType = type;
    readyList = new SortedList<Thread *>(cmp);
	// readyList = new List<Thread *>;
	toBeDestroyed = NULL;
    this->previousPrediction = 0;
    this->nowBurstTime = 0;
}
//<TODO>

//----------------------------------------------------------------------
// Scheduler::~Scheduler
// 	De-allocate the list of ready threads.
//----------------------------------------------------------------------

// <TODO>
// Remove readyQueue
Scheduler::~Scheduler()
{ 
    delete readyList; 
} 
//<TODO>

//----------------------------------------------------------------------
// Scheduler::ReadyToRun
// 	Mark a thread as ready, but not running.
//	Put it on the ready list, for later scheduling onto the CPU.
//
//	"thread" is the thread to be put on the ready list.
//----------------------------------------------------------------------

//<TODO>
// Hint: readyQueue is preemptive SJF(Shortest Job First).
// When putting a new thread into readyQueue, you need to check whether preemption or not.
void
Scheduler::ReadyToRun (Thread *thread)
{
	ASSERT(kernel->interrupt->getLevel() == IntOff);
	DEBUG(dbgThread, "Putting thread on ready list: " << thread->getName());
    thread->setStatus(READY);
    int previous = kernel->scheduler->getPreviousPrediction();
    thread->setPredictedBurstTime(0.5 * kernel->scheduler->getBurstTime( ) + 0.5 * previous);
    DEBUG(dbgSJF,
          "<U> Tick ["  << kernel->stats->totalTicks << "]: Thread ["
                        << thread->getID( )
                        << "] update approximate burst time, from: ["
                        << previous
                        << "] + ["
                        << kernel->scheduler->getBurstTime()
                        << "], to ["
                        << thread->getPredictedBurstTime()
                        << "]");
    kernel->scheduler->setpreviousPrediction(thread->getPredictedBurstTime());

    readyList->Insert(thread);
    if(thread->getPredictedBurstTime() < kernel->currentThread->getPredictedBurstTime()){
        // preemption should occurs
        // DEBUG(dbgSJF, "process should preempt CPU");
        DEBUG(dbgSJF,
              "<YS> Tick [" << kernel->stats->totalTicks << "]: Thread ["
                            << kernel->currentThread->getID( )
                            << "] is now selected for execution, thread ["
                            << thread->getID( )
                            << "] is replaced, and it has executed ["
                            << thread->getBurstTime( ) << "] ticks");

        // kernel->currentThread->Yield(thread);
        kernel->currentThread->Yield();
        // kernel->currentThread->Sleep(false);
    }
    else{
        DEBUG(dbgSJF,
              "<I> Tick [" << kernel->stats->totalTicks << "]: Thread ["
                           << thread->getID( )
                           << "] is inserted into readyQueue"
                           );
        // no preemption, thread push into queue
        // readyList->Insert(thread);
    }
    // DEBUG(dbgSJF,
    //         "<I> Tick [" << kernel->stats->totalTicks << "]: Thread ["
    //                     << thread->getID( )
    //                     << "] is inserted into readyQueue"
    //                     );
    // no preemption, thread push into queue

	
}
//<TODO>

//----------------------------------------------------------------------
// Scheduler::FindNextToRun
// 	Return the next thread to be scheduled onto the CPU.
//	If there are no ready threads, return NULL.
// Side effect:
//	Thread is removed from the ready list.
//----------------------------------------------------------------------

//<TODO>
// a.k.a. Find Next (Thread in ReadyQueue) to Run
Thread *
Scheduler::FindNextToRun ()
{
	ASSERT(kernel->interrupt->getLevel() == IntOff);

	if (readyList->IsEmpty()) {
		return NULL;
	}
	else {
        // Print();
        Thread *naive = readyList->RemoveFront();
        DEBUG(dbgSJF,
              "<R> Tick [" << kernel->stats->totalTicks << "]: Thread ["
                           << naive->getID( )
                           << "] is removed from readyQueue"
                           );
        return naive; 
	}
}
//<TODO>

//----------------------------------------------------------------------
// Scheduler::Run
// 	Dispatch the CPU to nextThread.  Save the state of the old thread,
//	and load the state of the new thread, by calling the machine
//	dependent context switch routine, SWITCH.
//
//      Note: we assume the state of the previously running thread has
//	already been changed from running to blocked or ready (depending).
// Side effect:
//	The global variable kernel->currentThread becomes nextThread.
//
//	"nextThread" is the thread to be put into the CPU.
//	"finishing" is set if the current thread is to be deleted
//		once we're no longer running on its stack
//		(when the next thread starts running)
//----------------------------------------------------------------------

void
Scheduler::Run (Thread *nextThread, bool finishing)
{
    Thread *oldThread = kernel->currentThread;

//	cout << "Current Thread" <<oldThread->getName() << "    Next Thread"<<nextThread->getName()<<endl;

    ASSERT(kernel->interrupt->getLevel() == IntOff);

    if (finishing) {	// mark that we need to delete current thread
        ASSERT(toBeDestroyed == NULL);
        toBeDestroyed = oldThread;
    }

#ifdef USER_PROGRAM			// ignore until running user programs 
    if (oldThread->space != NULL) {	// if this thread is a user program,
        oldThread->SaveUserState(); 	// save the user's CPU registers
        oldThread->space->SaveState();
    }
#endif
    
    oldThread->CheckOverflow();		    // check if the old thread
					                // had an undetected stack overflow

    kernel->currentThread = nextThread;  // switch to the next thread
    nextThread->setStatus(RUNNING);      // nextThread is now running
    nextThread->setStartTime(kernel->stats->totalTicks);


    DEBUG(dbgSJF,
          "Start running a new process["
              << nextThread->getID( ) << "], start at Tick["
              << nextThread->getStartTime() << "]");
    // DEBUG(dbgThread, "Switching from: " << oldThread->getName() << " to: " << nextThread->getName());
    
    // This is a machine-dependent assembly language routine defined 
    // in switch.s.  You may have to think
    // a bit to figure out what happens after this, both from the point
    // of view of the thread and from the perspective of the "outside world".

    cout << "Switching from: " << oldThread->getID() << " to: " << nextThread->getID() << endl;
    SWITCH(oldThread, nextThread);

    // we're back, running oldThread
    // interrupts are off when we return from switch!
    ASSERT(kernel->interrupt->getLevel() == IntOff);

    DEBUG(dbgThread, "Now in thread: " << kernel->currentThread->getID());
    // DEBUG(dbgSJF, "Now in thread: " << kernel->currentThread->getID());

    CheckToBeDestroyed();		// check if thread we were running
					            // before this one has finished
					            // and needs to be cleaned up
    
#ifdef USER_PROGRAM
    if (oldThread->space != NULL) {	    // if there is an address space
        oldThread->RestoreUserState();     // to restore, do it.
	    oldThread->space->RestoreState();
    }
#endif
}

//----------------------------------------------------------------------
// Scheduler::CheckToBeDestroyed
// 	If the old thread gave up the processor because it was finishing,
// 	we need to delete its carcass.  Note we cannot delete the thread
// 	before now (for example, in Thread::Finish()), because up to this
// 	point, we were still running on the old thread's stack!
//----------------------------------------------------------------------

void
Scheduler::CheckToBeDestroyed()
{
    if (toBeDestroyed != NULL) {
        DEBUG(dbgThread, "toBeDestroyed->getID(): " << toBeDestroyed->getID());

        delete toBeDestroyed;
        toBeDestroyed = NULL;
    }

    // DEBUG(dbgSJF, "CheckToBeDestoyred Finished!");
}
 
//----------------------------------------------------------------------
// Scheduler::Print
// 	Print the scheduler state -- in other words, the contents of
//	the ready list.  For debugging.
//----------------------------------------------------------------------

//<TODO>
void
Scheduler::Print()
{
    cout << "Ready list contents:\n";
    readyList->Apply(ThreadPrint);
}
//<TODO>

//<TODO>
//Function definition of sorting rule of readyQueue
static int cmp(Thread *a, Thread *b){
    DEBUG(dbgSJF,
          "***Thread [" << a->getID( ) << "]'s and Thread ["
                        << b->getID( )
                        << "]'s burst time are ["
                        << a->getPredictedBurstTime( ) << "] and ["
                        << b->getPredictedBurstTime( )
                        << "] ***");
    // cout << "inside cmp\n";
    if(a->getPredictedBurstTime() == b->getPredictedBurstTime()){
        return a->getID() < b->getID() ? 0 : -1;
    }
    else{
        DEBUG(dbgSJF,
              "Thread [" << a->getID( ) << "]'s and Thread [" << b->getID( )
                         << "]'s burst time are ["
                         << a->getPredictedBurstTime( ) << "] and ["
                         << b->getPredictedBurstTime( ) << "]\n");
        return a->getPredictedBurstTime() < b->getPredictedBurstTime() ? 0 : -1;
    }
}
// <TODO>