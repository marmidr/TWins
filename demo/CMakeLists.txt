################################################################################
#   TWins library
#   (c) 2020 Mariusz Midor
#   https://bitbucket.org/mmidor/twins/
################################################################################

set(TARGETNAME TWinsDemo)

add_executable(${TARGETNAME}
    src/demo.cpp
)

target_link_libraries(${TARGETNAME}
    twins
)
