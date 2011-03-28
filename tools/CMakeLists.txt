configure_file(
    "${PROJECT_SOURCE_DIR}/tools/synthese_bootstrap.py"
    "${PROJECT_BINARY_DIR}/synthese.py"
    @ONLY
)

install(
  FILES "${PROJECT_BINARY_DIR}/synthese.py"
  DESTINATION bin
  PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ GROUP_EXECUTE GROUP_READ WORLD_EXECUTE WORLD_READ
)

install(
  DIRECTORY .
  DESTINATION share/synthese/tools
  PATTERN ".svn" EXCLUDE
  PATTERN "*.pyc" EXCLUDE
  PATTERN "pip_download_cache" EXCLUDE
)

install(CODE "EXECUTE_PROCESS(COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_INSTALL_PREFIX}/bin/synthese.py --create-pyenv-only)")
