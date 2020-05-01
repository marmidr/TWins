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
    # enable coverage only for source files
    target_compile_options(${TARGETNAME}
        PRIVATE
            --coverage
    )

    add_subdirectory(tests)
endif()
