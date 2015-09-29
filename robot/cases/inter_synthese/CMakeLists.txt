

add_test (
  NAME ${CMAKE_CURRENT_LIST_DIR}
  COMMAND pybot --pythonpath ${PROJECT_SOURCE_DIR}/resources/s3-fakeclient --variablefile ${CMAKE_BINARY_DIR}/modules.py --variable /server/s3-server ${CMAKE_CURRENT_LIST_DIR}/flux_creation_should_work.txt
)
