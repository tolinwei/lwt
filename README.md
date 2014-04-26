##lwt 

A Light-weight User Level Non-Preemptive Multi-Threading Library for GNU Environment

###Features

- lwt provides similar APIs to Pthreads (POSIX Threading) that allow multi-threading programming, including operations of creating, joining, dying and etc
- The threads of lwt are user level threads, which have far lower overhead of context switching than Pthreas, however, they're blocking threas
- Inspired by Google's Go programming language, lwt provides similar mechanism of [Go's channel](http://golang.org/doc/effective_go.html#channels) for communicating between threads
- Buffered channels are supported for asynchronous communication, and channels can be sent through channel for bidirectional message passing

###APIs

####thread-related data structures
- `struct lwt_tcb {}` defines a TCB, which is short for thread control block, similar to PCB (process control block). It stores necessary information of a thread, when performing a thread switching, the data of current thread will be saved into corresponding TCB of current thread, and data of next thread will be restored from the TCB of next thread, if exists. `*lwt_t` is a pointer to `struct lwt_tcb {}` to provide some easier operations
- By maintaning a run queue (linked list) of `struct lwt_tcb {}`s, the library can perform possible operations easily by manipulating these `struct lwt_tcb {}`s

####thread-related functions
- `int lwt_id(lwt_t)` takes in a pointer of `lwt_tcb`, returns the ID of that thread
- `lwt_t lwt_current(void)` returns the currently running thread, which is maintained by a global variable
- `lwt_t lwt_create(lwt_fn_t, void*)` takes in two parameters: first one is a function pointer that denotes which function the newly created thread will execute, the second one passes in a value to that function. It returns a pointer to this newly created thread
- `void *lwt_join(lwt_t)` receives a thread that to be joined, and blocking current thread until the execution of that thread is finished, and then returns a `void *` value from that thread. Notice that only parent thread can join their children thread, of zombie threads may exist in system
- `void lwt_die(void *)` can only be executed in threads to die themselves, it passes in a `void *` variable, which later will be returned from a `lwt_join()` function that performed on this thread
- `int lwt_yield(lwt_t)` is for thread switching, suspends current thread and saves the content of current thread ot its `struct lwt_tcb {}`, then continue the execution of destination thread by restoring the data of destination thread from its corresponding `struct lwt_tcb {}`

####channel-related data structures


####channel-related functions

###Usage and Examples

- `main.c` contains examples about how to use the APIs
- After cloned this repository into your local directory, use `$make` to copmile and link the project
- Then you'll get a executable file named `main`, run it by `$./main` to get some print out overhead measured by a inline assembly instruction [rdtsc](http://en.wikipedia.org/wiki/Time_Stamp_Counter)

###Author

**[Wei Lin](http://www.github.com/ivanlw)**



