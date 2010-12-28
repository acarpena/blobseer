add_executable (pmanager
			pmanager_mon_listener.cpp 
			adv_manager.cpp 
			publisher.cpp)

# Link the executable to the necessary libraries.
target_link_libraries (pmanager ${LIB_DEPENDENCIES})
