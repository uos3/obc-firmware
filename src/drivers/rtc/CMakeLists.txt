# RTC CMakeLists.txt

if (${UOS3_TARGET_TM4C})
    add_library(Rtc
        Rtc_public_common.c
        Rtc_public_tm4c.c
    )
else()
    add_library(Rtc
        Rtc_public_common.c
        Rtc_public_linux.c
    )
endif()
target_link_libraries(Rtc
    ${STANDARD_LINK_LIBS}
    Delay
)