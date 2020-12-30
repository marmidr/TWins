################################################################################
#   TWins library - unit tests
#   (c) 2020 Mariusz Midor
#   https://bitbucket.org/marmidr/twins/
################################################################################

set(TARGETNAME TWinsUT)

add_executable(${TARGETNAME}
    src/test_main.cpp
    src/test_ansi_esc_decoder.cpp
    src/test_string.cpp
    src/test_utils.cpp
    src/test_stack.cpp
    src/test_queue.cpp
    src/test_vector.cpp
    src/test_map.cpp
    src/test_twins.cpp
    src/test_input_posix.cpp
    src/test_widget.cpp
    src/test_cli.cpp
)

target_compile_options(${TARGETNAME} PRIVATE
    --coverage
    ${GCC_SANITIZERS}
)

target_link_libraries(${TARGETNAME}
    twins
    gtest
    gmock
)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.16")
    # https://cmake.org/cmake/help/v3.19/command/target_precompile_headers.html
    # 51s -> 40s
    target_precompile_headers(${TARGETNAME}
        PRIVATE
            <string>
            <vector>
            <gtest/gtest.h>
            <gmock/gmock.h>
    )
endif()

# ---

set(GCOVR_EXCLUDES
    -e ".*/googletest/.*"
    -e ".*/test_.*cpp"
    -e ".*/twins_window_state.hpp"
)

add_custom_target(twins_cov_only
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND mkdir -p cover_html
    COMMAND gcovr -r ${PROJECT_SOURCE_DIR} -s ${GCOVR_EXCLUDES} -b --html-details -o cover_html/cover.html
    COMMAND ln -sf cover_html/cover.html cover.html
)

add_custom_target(twins_cov_clean
    COMMAND find ${CMAKE_BINARY_DIR} -name *.gcda -delete
)

# ---

add_test(NAME ${TARGETNAME} COMMAND $<TARGET_FILE:${TARGETNAME}>)
