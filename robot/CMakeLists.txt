

enable_testing ()
add_test (
  NAME cases
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/result
  COMMAND pybot --pythonpath ${PROJECT_SOURCE_DIR}/resources/s3-fakeclient --variable S3_SERVER_BINARY:${PROJECT_SOURCE_DIR}/../server/build/src/bin/server/s3-server ${PROJECT_SOURCE_DIR}/cases
)

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)


