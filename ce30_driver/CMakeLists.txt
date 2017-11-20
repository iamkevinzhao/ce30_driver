configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

add_library(
  ce30_driver
    SHARED
      utils.h
      utils.cpp
      config.h.in
      udp_socket.h
      udp_socket.cpp
      packet.h
      packet.cpp
)

set(
  PUBLIC_HEADERS
    utils.h
    udp_socket.h
    packet.h
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
