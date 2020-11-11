# CMakeLists.txt for tests.

FIND_PACKAGE(CMOCKA REQUIRED)
INCLUDE_DIRECTORIES(${LIBCMOCKA_INCLUDE_DIR})

add_executable(test_all_modules
    ${STARTUP_SOURCE}
    test_all_modules.c
)
target_link_libraries(test_all_modules
    ${STANDARD_LINK_LIBS}
    ${CMOCKA_LIBRARY}
    EventManager
)