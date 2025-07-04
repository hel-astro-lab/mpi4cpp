function(add_mpi_test name no_mpi_proc)
    add_executable(${name} test_${name}.c++)

    target_link_libraries(${name} PRIVATE mpi4cpp mpi4cpp_warnings)

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

