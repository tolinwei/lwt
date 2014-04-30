##lwt 

A light-weight user level non-preemptive multi-threading library for GNU environment

###Features

- lwt provides similar APIs to Pthreads (POSIX Threading) that allow multi-threading programming, including operations of creating, joining, dying and etc
- The threads of lwt are user level threads, which have far lower overhead of context switching than Pthreads, however, they're blocking threads
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

- `struct lwt_channel {}` defines the data structure of channel, which is used for message passing between/among threads. Each channel can be set up for multi senders but only one receiver. `*lwt_chan_t` is the pointer that points to `struct lwt_channel {}`
- `struct clist_head {}` is a linked list of senders of a channel, it is actually the head of the linked list, all the senders are chained by a member `struct clist_head *next` of this structure. `struct slist_head {}` is also the member of `struct lwt_channel {}`, as introduced before, denoting the sender(s) of a channel

####channel-related functions

- `lwt_chan_t lwt_chan(int sz)` creates a channel (by mallocing space and define the data structures inside) referenced by the return value. The thread that creates the channel is the receiver of this channel. The parameter says the buffer size of the channel, if size is larger than 1, asyncrounous message passing is supported
- `void lwt_chan_deref(lwt_chan_t)` inspired by the reference counting system of [Apple's Objective-C](https://developer.apple.com/library/mac/documentation/cocoa/conceptual/ProgrammingWithObjectiveC/Introduction/Introduction.html), used to deallocate a channel only if no thread still have reference to this channel
- `int lwt_snd(lwt_chan_t, void *)` regards the second parameter as data, and send it to the channel denotes at first parameter.
- `void *lwt_rcv(lwt_chan_t)` receives the data `void *` from a channel
- `int lwt_snd_chan(lwt_chan_t, lwt_chan_t)` is similar to `void lwt_snd(lwt_chan_t, void *`, the only difference is this function sends another channel through a channel. It's very useful if you're to implemente bidirectional communication
- `lwt_chan_t lwt_rcv_chan(lwt_chan_t)` receives a channel from a channel

###Usage and Examples

- `main.c` contains examples about how to use the APIs
- After cloned this repository into your local directory, use `$make` to copmile and link the project
- Then you'll get a executable file named `main`, run it by `$./main` to get some print out overhead measured by a inline assembly instruction [rdtsc](http://en.wikipedia.org/wiki/Time_Stamp_Counter)
- Tested through the compiler `gcc (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3`

###Author

**[Wei Lin](http://www.github.com/ivanlw)**



