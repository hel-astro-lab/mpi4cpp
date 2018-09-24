# MPI2++

Crazy (weekend) project to make a header-only version of the boost::mpi for modern C++-17 and newest MPI versions.


## Refs / Notes

All kudos go to [boost::mpi](https://github.com/boostorg/mpi/)!

My hope is that by dropping compiler support of all the ancient C++ versions one can greatly simplify the boost::mpi and MPI interfaces.


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
    - [x] std::vector
    - [x] std::vector for known size
- [ ] blockers/synchronization
    - [ ] barrier
    - [ ] waitany
    - [ ] waitsome
    - [ ] waitall
- [ ] advanced serialization & optimization

other not so urgent implementations:
- [ ] sendrecv
- [ ] collectives


