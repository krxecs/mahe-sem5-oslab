CPMAddPackage("gh:krxecs/aes-c#main")

add_executable(lab7-q-server q_server.c)
target_link_libraries(lab7-q-server osl-vec osl-helper aes-c::aes-c)

add_executable(lab7-q-client q_client.c)
target_link_libraries(lab7-q-client osl-vec osl-helper aes-c::aes-c)
