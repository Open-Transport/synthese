set_source_groups()

set(map_SRCS
Drawable.cpp
Drawable.h
DrawableLine.cpp
DrawableLine.h
DrawableLineComparator.cpp
DrawableLineComparator.h
DrawableLineIndex.cpp
DrawableLineIndex.h
DrawablePhysicalStop.cpp
DrawablePhysicalStop.h
Geometry.cpp
Geometry.h
HtmlMapRenderer.cpp
HtmlMapRenderer.h
JpegRenderer.cpp
JpegRenderer.h
Map.cpp
Map.h
MapBackground.cpp
MapBackground.h
MapBackgroundManager.cpp
MapBackgroundManager.h
MapBackgroundTile.cpp
MapBackgroundTile.h
MapInfoRenderer.cpp
MapInfoRenderer.h
MapModule.cpp
MapModule.gen.cpp
MapModuleRegister.cpp
MapModule.h
MapModule.inc.cpp
MapRequest.cpp
MapRequest.h
PostscriptCanvas.cpp
PostscriptCanvas.h
PostscriptRenderer.cpp
PostscriptRenderer.h
Rectangle.cpp
Rectangle.h
Renderer.cpp
Renderer.h
RenderingConfig.cpp
RenderingConfig.h
TestMapAdmin.cpp
TestMapAdmin.h
TileGrid.cpp
TileGrid.h
WFSService.cpp
WFSService.hpp
WFSType.hpp
WFSTypeTemplate.hpp
XmlBuilder.cpp
XmlBuilder.h
)

set_source_files_properties(MapModule.gen.cpp MapModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(39_map ${map_SRCS})

target_link_libraries(39_map
  ${Boost_LIBRARIES}
  01_util
  35_pt
)

install(TARGETS 39_map DESTINATION lib)
