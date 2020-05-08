################################################################################
#   TWins library
#   (c) 2020 Mariusz Midor
#   https://bitbucket.org/mmidor/twins/
################################################################################

set(TARGETNAME twins)

add_library(${TARGETNAME}
    src/twins.cpp
    src/twins_colors.cpp
    src/twins_widget.cpp
    src/twins_widget_draw.cpp
    src/twins_utf8str.cpp
    src/twins_string.cpp
    src/twins_input_posix.cpp
    src/twins_ansi_esc_decoder.cpp
)

target_include_directories(${TARGETNAME}
    PUBLIC
        inc
)

if (${TWINS_BUILD_UT})
    # https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html
    # set(GCC_SANITIZERS "-fno-omit-frame-pointer -fsanitize=leak -fsanitize=address -fsanitize=undefined")
    # set(GCC_SANITIZERS "-fsanitize=leak,undefined")

    if (MINGW OR MSYS)
        set(GCC_SANITIZERS "")
        message(STATUS "MINGW / MSYS")
        message(STATUS "Sanitizers not supported on MINGW / MSYS")
    elseif (UNIX)
        # target_compile_options(${TARGETNAME} PRIVATE -static-libasan)
        # target_compile_options(${TARGETNAME} PRIVATE -static-libubsan)
        set(GCC_SANITIZERS "-fsanitize=leak,address,undefined")
        message(STATUS "UNIX")
    else()
        message(STATUS "Unknown target")
        set(GCC_SANITIZERS "")
    endif()

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_SANITIZERS}")

    # enable coverage only for source files
    target_compile_options(${TARGETNAME} PRIVATE --coverage)

    add_subdirectory(tests)
endif()
