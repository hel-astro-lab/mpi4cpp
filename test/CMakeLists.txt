project (tests)


set (TEST_FILES 
     hello.c++
     )


add_executable(tests
           ${TEST_FILES}
           )

target_link_libraries(tests PRIVATE -lmpi)
#target_include_directories(tests PUBLIC ${module_PUBLIC_LINK_TARGETS} PRIVATE ${module_PRIVATE_LINK_TARGETS})
target_compile_options(tests PRIVATE ${WARNING_FLAGS})

# installation
#install (TARGETS tests DESTINATION lib)
