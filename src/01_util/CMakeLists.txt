set_source_groups()

set(util_SRCS
AlphanumericFilter.h
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
IConv.cpp
IConv.hpp
Log.cpp
Log.h
LowerCaseFilter.h
MacAddressGetter.cpp
MacAddressGetter.h
MD5.cpp
MD5.h
MD5Wrapper.cpp
MD5Wrapper.h
Named.h
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
SqlFilter.h
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
