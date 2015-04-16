set_source_groups()

set(html_SRCS
ActionResultHTMLTable.cpp
ActionResultHTMLTable.h
AjaxForm.cpp
AjaxForm.hpp
AjaxVectorFieldEditor.cpp
AjaxVectorFieldEditor.hpp
Constants.h
EditArea.cpp
EditArea.hpp
HTMLForm.cpp
HTMLForm.h
HTMLList.cpp
HTMLList.h
HTMLModule.cpp
HTMLModule.h
HTMLPage.cpp
HTMLPage.hpp
HTMLTable.cpp
HTMLTable.h
PropertiesHTMLTable.cpp
PropertiesHTMLTable.h
ResultHTMLTable.cpp
ResultHTMLTable.h
SearchFormHTMLTable.cpp
SearchFormHTMLTable.h
TinyMCE.cpp
TinyMCE.hpp
)

add_library(05_html ${html_SRCS})

target_link_libraries(05_html
  01_util
)

install(TARGETS 05_html DESTINATION lib)
