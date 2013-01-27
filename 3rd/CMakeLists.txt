if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  # No warning for third party
  add_definitions("-w")
endif()

add_subdirectory(dev)
