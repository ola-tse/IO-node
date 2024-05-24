# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "/Users/olaoren/01_code/zephyrproject/projects/gorochu/build/zephyr/include/generated/syscalls"
  )
endif()
