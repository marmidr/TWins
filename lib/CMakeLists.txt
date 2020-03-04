################################################################################
#   TWins library
#   (c) 2020 Mariusz Midor
#   https://bitbucket.org/mmidor/twins/
################################################################################

set(TARGETNAME twins)

include_directories(inc)

add_library(${TARGETNAME}
    src/twins.cpp
)
