set(DEPS
  01_util_iostreams
  10_db
)


# FIXME: wants boost-zlib on windows, which isn't there yet.
if(NOT WIN32)
  boost_test(Compression "${DEPS}")
endif()
