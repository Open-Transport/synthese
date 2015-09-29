
add_test (
  NAME ${CMAKE_CURRENT_LIST_DIR}
  COMMAND pybot --pythonpath ${PROJECT_SOURCE_DIR}/resources/s3-fakeclient --variablefile ${CMAKE_BINARY_DIR}/modules.py --variable S3_SERVER_BINARY:${S3_SERVER_BINARY} ${CMAKE_CURRENT_LIST_DIR}
)
