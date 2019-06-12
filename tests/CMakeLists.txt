# Copyright 2019 - , Dr.-Ing. Patrick Siegl
# SPDX-License-Identifier: Apache-2.0

add_executable(bind bind.cpp)
add_test(NAME bind COMMAND bind coverage)
set_target_properties(
  bind PROPERTIES
  COMPILE_FLAGS "-fprofile-arcs -ftest-coverage"
  LINK_FLAGS "-lgcov -fprofile-arcs -ftest-coverage"
)

add_executable(call call.cpp)
add_test(NAME call COMMAND call coverage)
set_target_properties(
  call PROPERTIES
  COMPILE_FLAGS "-fprofile-arcs -ftest-coverage"
  LINK_FLAGS "-lgcov -fprofile-arcs -ftest-coverage"
)

add_custom_target(coverage)
add_custom_command(TARGET coverage
                  COMMAND lcov -c -i -d . -o .coverage.base
                  COMMAND lcov -c -d . -o .coverage.run
                  COMMAND lcov  --no-external -d . -a .coverage.base -a .coverage.run -o .coverage.total
                  COMMAND lcov --remove .coverage.total '/usr/include/*' 'src/*' -o .filt_coverage.total
                  COMMAND genhtml --no-branch-coverage -o . .filt_coverage.total)
