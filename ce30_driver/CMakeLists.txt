configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

find_package(Boost)
include_directories(${Boost_INCLUDE_DIRS})

IF(MSVC)
    ADD_DEFINITIONS("/EHsc")
ENDIF(MSVC)

if ("${Boost_CUSTOMIZED_LIBRARYDIR}" STREQUAL "")
  message("Using default Boost library directory")
else()
  message("Using customized Boost library directory")
  link_directories("${Boost_CUSTOMIZED_LIBRARYDIR}")
endif()

add_library(
  ce30_driver
    SHARED
      utils.h
      utils.cpp
      config.h.in
      udp_socket.cpp
      udp_socket.h
      timed_udp_socket.cpp
      timed_udp_socket.h
      packet.h
      packet.cpp
      export.h
      ce30_driver.h
)

# target_link_libraries(ce30_driver ${Boost_LIBRARIES})

set(
  PUBLIC_HEADERS
    utils.h
    udp_socket.h
    packet.h
    export.h
    ce30_driver.h
)

set_target_properties(ce30_driver PROPERTIES
  PUBLIC_HEADER
    "${PUBLIC_HEADERS}"
)

install(TARGETS ce30_driver
  # IMPORTANT: Add the ce30_driver library to the "export-set"
  EXPORT ce30_driverTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/ce30_driver"
    COMPONENT dev)
