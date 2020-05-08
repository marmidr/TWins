################################################################################
#   TWins library
#   (c) 2020 Mariusz Midor
#   https://bitbucket.org/mmidor/twins/
################################################################################

set(TARGETNAME TWinsDemo)

add_executable(${TARGETNAME}
    src/demo.cpp
    src/demo_wnd.cpp
)

target_include_directories(${TARGETNAME}
    PRIVATE
        inc
)

target_link_libraries(${TARGETNAME}
    twins
)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${GCC_SANITIZERS}")
