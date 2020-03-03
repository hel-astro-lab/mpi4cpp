project (tests)

function(add_mpi_test name no_mpi_proc)
    #include_directories(${MY_TESTING_INCLUDES})
    add_executable(${name} test_${name}.c++)

if(APPLE)
    # Make sure to link MPI here too:
    target_link_libraries(${name} PRIVATE -lmpi)
endif()

    target_compile_options(${name} PRIVATE ${WARNING_FLAGS})
    set(test_parameters ${MPIEXEC_NUMPROC_FLAG} ${no_mpi_proc} "./${name}")

    add_test(NAME ${name} COMMAND ${MPIEXEC} ${test_parameters})
endfunction(add_mpi_test)


set (TEST_FILES 
     init
     send_recv_nodata
     send_recv
     send_recv_types
     arrays
     isend_irecv_nodata
     isend_irecv
     isend_irecv_types
     iarrays
     own_datatype
     )


# add all tests
foreach(i ${TEST_FILES})
    add_mpi_test(${i} 2)
endforeach()

