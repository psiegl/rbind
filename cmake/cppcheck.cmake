add_custom_target(
  cppcheck
  COMMAND /usr/bin/cppcheck
          --enable=warning,performance,portability,information,missingInclude
          --std=c++11
          --template="[{severity}][{id}] {message} {callstack} \(On {file}:{line}\)"
          --verbose
          -I ${CMAKE_SOURCE_DIR}/inc
          ${CMAKE_SOURCE_DIR}/tests
)
