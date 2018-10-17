add_executable(
  ce30_demo
    main.cpp
)

target_link_libraries(
  ce30_demo
    ce30_driver
)

install(TARGETS ce30_demo
  # IMPORTANT: Add the ce30_demo executable to the "export-set"
  EXPORT ce30_driverTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
