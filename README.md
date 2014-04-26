##lwt 

A Light-weight User Level Non-Preemptive Multi-Threading Library for GNU Environment

###Features

- lwt provides similar APIs to Pthreads (POSIX Threading) that allow multi-threading programming, including operations of creating, joining, dying and etc
- The threads of lwt are user level threads, which have far lower overhead of context switching than Pthreas, however, they're blocking threas
- Inspired by Google's Go programming language, lwt provides similar mechanism of [Go's channel](http://golang.org/doc/effective_go.html#channels) for communicating between threads
- Buffered channels are supported for asynchronous communication

###APIs

####Threas part

####Channel part

###Author

**[Wei Lin](http://www.github.com/ivanlw)**



