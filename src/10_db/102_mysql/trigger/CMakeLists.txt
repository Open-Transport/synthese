# Path for MySQL include directory
include_directories(${MYSQL_INCLUDE_DIR})
include_directories(${CURL_INCLUDE_DIRS})

add_definitions("-DHAVE_DLOPEN")
add_library(synthese_mysql_udf MODULE synthese.c synthese.def)

target_link_libraries(synthese_mysql_udf ${CURL_LIBRARIES})

# TODO: this should probably be installed into /usr/lib/mysql/plugin/synthese_mysql_udf.so (without the lib prefix!)
# (at least on Debian and derivatives using default MySQL package)
install(TARGETS synthese_mysql_udf DESTINATION lib/mysql_udf_plugin)

install(FILES trigger_udf.sql DESTINATION share/synthese/mysql_udf_plugin)
