################################################################################
#   TWins library - unit tests
#   (c) 2020 Mariusz Midor
#   https://bitbucket.org/mmidor/twins/
################################################################################

set(TARGETNAME twins_ut)

add_executable(${TARGETNAME}
    src/main.cpp
)

target_link_libraries(${TARGETNAME}
    twins
)
