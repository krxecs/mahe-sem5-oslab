add_executable(q1_producer-consumer q1_producer-consumer.c)
target_link_libraries(q1_producer-consumer osl-helper)

add_executable(q2_pipe-demo q2_pipe-demo.c)
target_link_libraries(q2_pipe-demo osl-helper)

add_executable(q4_shelf-producer q4_shelf-producer.c)
target_link_libraries(q4_shelf-producer osl-helper)

add_executable(q4_shelf-consumer q4_shelf-consumer.c)
target_link_libraries(q4_shelf-consumer osl-helper)
