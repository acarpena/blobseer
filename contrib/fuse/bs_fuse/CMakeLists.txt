add_executable ( bs_fs main.cpp blobseer_fs.cpp)

# Link the executable to the necessary libraries.
target_link_libraries (bs_fs ${LIB_DEPENDENCIES} ${BLOBSEER_LIB})
