if(WITH_40_PDF_CMS)

  # Haru
  find_package(Haru)
  if(NOT HARU_FOUND)
    message(FATAL_ERROR "libharu not found")
  endif()

  # Png
  find_package(Png)
  if(NOT PNG_FOUND)
    message(FATAL_ERROR "libpng not found")
  endif()
  add_definitions(-DPNG_NO_MMX_CODE)

  set_source_groups()

  file(GLOB pdf_cms_SRCS "*.cpp" "*.hpp")

  set_source_files_properties(PDFCMSModule.gen.cpp PDFCMSModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

  include_directories(${HARU_INCLUDE_DIRS})


  add_library(40_pdf_cms ${pdf_cms_SRCS})

  target_link_libraries(40_pdf_cms
    ${PNG_LIBRARIES}
    ${HARU_LIBRARIES}
    11_cms
  )

  install(TARGETS 40_pdf_cms DESTINATION lib)

endif(WITH_40_PDF_CMS)
