# CMakeLists.txt for system modules

# Data Pool and Kernel module are required by almost everything so they're 
# added in the master CMAKE lists before any other subdir

# Event Manager module
add_subdirectory(event_manager)

# MemStoreManager module
add_subdirectory(mem_store_manager)

# OpModeManager
add_subdirectory(opmode_manager)