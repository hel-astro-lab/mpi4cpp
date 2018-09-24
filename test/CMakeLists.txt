project (tests)

function(add_mpi_test name no_mpi_proc)
    #include_directories(${MY_TESTING_INCLUDES})

    add_executable(${name} test_${name}.c++)

    #add_dependencies(${name} googletest)

    # Make sure to link MPI here too:
    target_link_libraries(${name} PRIVATE -lmpi)
    target_compile_options(${name} PRIVATE ${WARNING_FLAGS})

    set(test_parameters ${MPIEXEC_NUMPROC_FLAG} ${no_mpi_proc} "./${name}")

    add_test(NAME ${name} COMMAND ${MPIEXEC} ${test_parameters})
endfunction(add_mpi_test)


set (TEST_FILES 
     init
     send_recv_nodata
     send_recv
     )


# add all tests
foreach(i ${TEST_FILES})
    message(${i})
    add_mpi_test(${i} 2)
endforeach()

