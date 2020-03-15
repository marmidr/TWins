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
    src/twins_utf8str.cpp
    src/twins_string.cpp
    src/twins_input_posix.cpp
    src/twins_input_decoder.cpp
)

target_include_directories(${TARGETNAME}
    PUBLIC
        inc
)
