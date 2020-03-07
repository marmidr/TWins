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
)

target_include_directories(${TARGETNAME}
    PUBLIC
        inc
)
