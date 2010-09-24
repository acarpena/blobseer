# Build the interface
add_library (bfs-client SHARED BFS_interface.cpp)
add_library (bfs-client-c SHARED BFS_interface-c.cpp)

add_executable (testc test.c)

# Link the executable to the necessary libraries.
target_link_libraries (bfs-client blobseer)
target_link_libraries (bfs-client-c bfs-client)
target_link_libraries (testc bfs-client-c)
