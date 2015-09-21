set_source_groups()

file(GLOB pdf_cms_SRCS "*.cpp" "*.hpp")

set_source_files_properties(PDFCMSModule.gen.cpp PDFCMSModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

include_directories(${HARU_INCLUDE_DIRS})


add_library(40_pdf_cms ${pdf_cms_SRCS})

target_link_libraries(40_pdf_cms
  ${HARU_LIBRARIES}
  11_cms
)

install(TARGETS 40_pdf_cms DESTINATION lib)
