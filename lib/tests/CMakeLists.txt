################################################################################
#   TWins library - unit tests
#   (c) 2020 Mariusz Midor
#   https://bitbucket.org/mmidor/twins/
################################################################################

set(TARGETNAME TWinsUT)

add_executable(${TARGETNAME}
    src/test_main.cpp
    src/test_input_decoder.cpp
    src/test_string.cpp
    src/test_twins.cpp
    src/test_widget.cpp
)

target_link_libraries(${TARGETNAME}
    twins
    gtest
    gmock
)

target_include_directories(${TARGETNAME}
    PRIVATE
        inc
)

# ---

set(GCOVR_EXCLUDES ".*/googletest/.*")

add_custom_target(twins_cov_only
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND mkdir -p cover_html
    COMMAND gcovr -r ${PROJECT_SOURCE_DIR} -s -e ${GCOVR_EXCLUDES} -b --html-details -o cover_html/cover.html
    COMMAND ln -sf cover_html/cover.html cover.html
)

add_custom_target(twins_cov_clean
    COMMAND find . -name *.gcda -delete
)

# ---

add_test(NAME ${TARGETNAME} COMMAND $<TARGET_FILE:${TARGETNAME}>)
