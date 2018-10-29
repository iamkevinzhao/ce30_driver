add_executable(
  ce30_demo
    main.cpp
)

set(UNIX_LINKS "")

if(UNIX)
set(
  UNIX_LINKS
    ${UNIX_LINKS}
    -lboost_system
)
endif()

target_link_libraries(
  ce30_demo
    ce30_driver
    ${UNIX_LINKS}
)

install(TARGETS ce30_demo
  # IMPORTANT: Add the ce30_demo executable to the "export-set"
  EXPORT ce30_driverTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
