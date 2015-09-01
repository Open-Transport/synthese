set_source_groups()

set(util_SRCS
alphanum.hpp
AlphanumericFilter.h
Base64.cpp
Base64.hpp
Clock.hpp
Clock.cpp
ConcurrentQueue.hpp
ConstantReturner.h
Conversion.cpp
Conversion.h
EMail.cpp
EMail.h
Env.cpp
Env.h
EnvException.cpp
EnvException.h
FactorableTemplate.h
Factory.h
FactoryBase.h
FactoryException.h
FakeClock.hpp
FakeClock.cpp
IConv.cpp
IConv.hpp
Log.cpp
Log.h
MacAddressGetter.cpp
MacAddressGetter.h
MD5.cpp
MD5.h
MD5Wrapper.cpp
MD5Wrapper.h
MimeType.cpp
MimeType.hpp
MimeTypes.cpp
MimeTypes.hpp
ObjectNotFoundException.h
ObjectNotFoundInRegistryException.h
ParametersMap.cpp
ParametersMap.h
PlainCharFilter.h
Registrable.cpp
Registrable.h
Registry.h
RegistryBase.cpp
RegistryBase.h
RegistryException.h
RegistryKeyException.h
RGBColor.cpp
RGBColor.h
shared_recursive_mutex.hpp
Stacktrace.h
StringUtils.cpp
StringUtils.hpp
T9Filter.h
UniqueStringsSet.cpp
UniqueStringsSet.h
URI.cpp
URI.hpp
UtilConstants.h
UtilTypes.cpp
UtilTypes.h
XmlException.cpp
XmlException.h
XmlParser.cpp
XmlParser.h
XmlToolkit.cpp
XmlToolkit.h
ZipWriter.cpp
ZipWriter.hpp
concurrent/locking_queue.hpp
concurrent/SynchronizedQueue.h
)

if(WIN32)
  include_directories("${ICONV_INCLUDE_DIRS}")
endif()

add_library(01_util ${util_SRCS})

target_link_libraries(01_util
  ${Boost_LIBRARIES}
  00_framework
)

if(WIN32)
  target_link_libraries(01_util iconv)
endif()

add_subdirectory(iostreams)
add_subdirectory(threads)

install(TARGETS 01_util DESTINATION lib)
