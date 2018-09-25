# MPI4Cpp

Crazy (weekend) project to make a modern header-only C++ library of MPI. We rely on template metaprogramming to simplify and automate many communication routines.  Currently just a rip off of boost::mpi where boost dependencies are removed.


All kudos go to [boost::mpi](https://github.com/boostorg/mpi/)! See also their license.

My hope is that by dropping compiler support of all the ancient C++ versions one can greatly simplify the MPI interface of boost::mpi.


## Current implementation

- [x] mpi environment 
- [x] communicators
- [x] point-to-point communication  (`send`/`recv`)
    - [x] native types
    - [x] c-style arrays
    - [x] std::array
    - [x] std::vector
    - [x] std::vector for known size
- [ ] non-blocking (`isend`/`irecv`)
    - [x] native types
    - [x] c-style arrays
    - [x] std::array
    - [ ] std::vector
    - [x] std::vector for known size
- [x] blockers/synchronization
    - [x] barrier
    - [x] wait_any
    - [x] wait_some
    - [x] wait_all
- [ ] advanced serialization & optimization

other not so urgent implementations:
- [ ] sendrecv
- [ ] collectives


