INCLUDE(FindSubversion)
Subversion_WC_INFO(${SOURCE_DIR} SYNTHESE)
FILE(WRITE src/15_server/svnversion.h.txt "#define SYNTHESE_REVISION \"${SYNTHESE_WC_REVISION}\"\n")
FILE(APPEND src/15_server/svnversion.h.txt "#define SYNTHESE_BUILD_DATE \"${SYNTHESE_WC_LAST_CHANGED_DATE}\"\n")
FILE(APPEND src/15_server/svnversion.h.txt "#define SYNTHESE_SVN_URL \"${SYNTHESE_WC_URL}\"\n")
EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy_if_different
      src/15_server/svnversion.h.txt src/15_server/svnversion.h)
