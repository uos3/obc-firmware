# Crypto module CMakeLists.txt

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Crypto_crc_tables.c
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Crypto_gen_crc_tables.py
    COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/Crypto_gen_crc_tables.py   
)
add_library(Crypto
    Crypto_public.c
    Crypto_crc_tables.c
)
target_link_libraries(Crypto
    ${STANDARD_LINK_LIBS}
)