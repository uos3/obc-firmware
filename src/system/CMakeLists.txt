# CMakeLists.txt for system modules

# Data Pool module is required by almost everything so it's added in the master
# CMAKE lists before any other subdir

# Event Manager module
add_subdirectory(event_manager)