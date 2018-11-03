# MPI4Cpp

Crazy (weekend) project to make a modern header-only C++ library of MPI. We rely on template metaprogramming to simplify and automate many communication routines.  Currently heavily based on boost::mpi, but so that boost dependencies are removed.

We also take advantage of modern `C++14` metaprogramming features. By dropping compiler support of all the ancient C++ versions it is then possible to greatly simplify and compactify the original boost::mpi implementation.

## Example

Major pain in using MPI is to insert all the type information manually. Here we make use of the compiler to do this for us. And why should we not - compiler is far better at this then any human is.

Compare

```c++
#include <mpi4cpp/mpi.h>

mpi::environment env(argc, argv);
mpi::communicator world;

std::vector<double> msg(20);
if (world.rank() == 0) {
    for(int i=0; i<msg.size(); i++) msg[i] = 10.0;

    world.send(1, 0, msg);
    world.recv(1, 1, msg);
} else {
    for(int i=0; i<msg.size(); i++) msg[i] = 20.0;

    world.send(0, 1, msg);
    world.recv(0, 0, msg);
}

```

Original MPI:
```c++
#include <mpi.h>

int rank;
MPI_Init(&argc, &argv);
MPI_Comm_rank(MPI_COMM_WORLD, &rank);

std::vector<double> msg(20);

if (rank() == 0) {
    for(int i=0; i<msg.size(); i++) msg[i] = 10.0;

    MPI_Send(&msg[0], msg.size(), MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
    MPI_Recv(&msg[0], msg.size(), MPI_DOUBLE, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
} else {
    for(int i=0; i<msg.size(); i++) msg[i] = 20.0;

    MPI_Recv(&msg[0], msg.size(), MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(&msg[0], msg.size(), MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
}

MPI_Finalize();
```


## Library status

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
- [ ] user-defined structs
    - [x] single class
    - [x] nonblocking
    - [x] std::vector
    - [ ] nonblocking std::vector
- [ ] advanced serialization & optimization

other not so urgent implementations:
- [ ] sendrecv
- [ ] collectives


## References

Kudos to [boost::mpi](https://github.com/boostorg/mpi/)! See also their license.

