set_source_groups()

add_executable(s3-proxy proxy.cpp)

target_link_libraries(s3-proxy
  ${Boost_LIBRARIES}
  ${CURL_LIBRARIES}
  15_server
  36_cms
)

install(TARGETS s3-proxy DESTINATION bin)
