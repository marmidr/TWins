################################################################################
#   TWins library
#   (c) 2020 Mariusz Midor
#   https://bitbucket.org/mmidor/twins/
################################################################################

option(TWINS_USE_FASTFILL "Use ANSI repeat code for faster line fill" ON)
option(TWINS_PAL_FULLIMPL "Disable in case of incompatible platform" ON)
set(TWINS_THEME_PATH "" CACHE STRING "Full path to theme.hpp")


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


if (${TWINS_USE_FASTFILL})
    target_compile_definitions(${TARGETNAME} PRIVATE -DTWINS_FAST_FILL=1)
else()
    target_compile_definitions(${TARGETNAME} PRIVATE -DTWINS_FAST_FILL=0)
endif()

if (${TWINS_PAL_FULLIMPL})
    target_compile_definitions(${TARGETNAME} PUBLIC -DTWINS_PAL_FULLIMPL=1)
else()
    target_compile_definitions(${TARGETNAME} PUBLIC -DTWINS_PAL_FULLIMPL=0)
endif()

if (TWINS_THEME_PATH STREQUAL "")
else()
    set(TWINS_THEME_PATH ${PROJECT_SOURCE_DIR}/${TWINS_THEME_PATH})
    message(STATUS "TWins theme location: ${TWINS_THEME_PATH}")
    target_compile_definitions(${TARGETNAME} PUBLIC -DTWINS_THEME="${TWINS_THEME_PATH}")
endif()

if (${TWINS_BUILD_UT})
    option(TWINS_USE_SANITIZERS "Use GCC sanitizers for Unit Tests" ON)
    set(GCC_SANITIZERS "")

    # https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html
    # set(GCC_SANITIZERS "-fno-omit-frame-pointer -fsanitize=leak -fsanitize=address -fsanitize=undefined")
    # set(GCC_SANITIZERS "-fsanitize=leak,undefined")

    if (TWINS_USE_SANITIZERS)
        if (MINGW OR MSYS)
            set(GCC_SANITIZERS "")
            message(STATUS "Platform: MINGW / MSYS")
            message(STATUS "Sanitizers not supported on MINGW / MSYS")
        elseif (UNIX)
            # target_compile_options(${TARGETNAME} PRIVATE -static-libasan)
            # target_compile_options(${TARGETNAME} PRIVATE -static-libubsan)
            set(GCC_SANITIZERS "-fsanitize=leak,address,undefined")
            message(STATUS "Platform: UNIX")
        else()
            message(STATUS "Platform: Unknown")
        endif()
    endif()

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_SANITIZERS}")

    # enable coverage only for source files
    target_compile_options(${TARGETNAME} PRIVATE --coverage)

    add_subdirectory(tests)
endif()
