
set(html_SRCS
ActionResultHTMLTable.cpp
ActionResultHTMLTable.h
AjaxForm.cpp
AjaxForm.hpp
AjaxVectorFieldEditor.cpp
AjaxVectorFieldEditor.hpp
Constants.h
HTMLFilter.h
HTMLForm.cpp
HTMLForm.h
HTMLList.cpp
HTMLList.h
HTMLModule.cpp
HTMLModule.h
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

install(TARGETS 05_html DESTINATION lib)
