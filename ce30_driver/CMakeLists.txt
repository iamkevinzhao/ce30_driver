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

if (WIN32)
  # add_definitions(-DUDP_SOCKET_USE_WINSOCK2_API)
  add_definitions(-DUDP_SOCKET_USE_BOOST_API)
else()
  add_definitions(-DUDP_SOCKET_USE_BOOST_API)
endif()

find_package(Qt5Widgets QUIET)
if (Qt5Widgets_FOUND)
  add_definitions(-DQT5_WIDGETS_EXISTS)
  set(QT5_LINK
    Qt5::Widgets
  )
  set(QT5_DEPENDENT_SOURCE
    channel_type_widgets.cpp
    channel_type_widgets.h
  )
  set(QT5_DEPENDENT_HEADERS
    channel_type_widgets.h
  )
endif()

add_library(
  ce30_driver
    SHARED
      utils.h
      utils.cpp
      config.h.in
      udp_socket.cpp
      udp_socket_win_impl.cpp
      udp_socket.h
      timed_udp_socket.cpp
      timed_udp_socket.h
      packet.h
      packet.cpp
      udp_server.h
      udp_server.cpp
      data_types.h
      data_types.cpp
      export.h
      ce30_driver.h
      ${QT5_DEPENDENT_SOURCE}
)

target_link_libraries(
  ce30_driver
    ${Boost_LIBRARIES}
    ${QT5_LINK}
)

set(
  PUBLIC_HEADERS
    utils.h
    udp_socket.h
    packet.h
    export.h
    ce30_driver.h
    data_types.h
    udp_server.h
    ${QT5_DEPENDENT_HEADERS}
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
