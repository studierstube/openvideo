FILE(GLOB_RECURSE SOURCES *.c *.cpp *.cxx)
ADD_LIBRARY(openvideo SHARED ${SOURCES})
TARGET_LINK_LIBRARIES(openvideo ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${ACE_LIBRARIES} ${AVIFILE_LIBRARIES} ${TINY_XML_MOD_LIBRARY})
INSTALL(TARGETS openvideo DESTINATION ${CMAKE_INSTALL_LIBDIR})
SET_TARGET_PROPERTIES(openvideo PROPERTIES VERSION ${CPACK_PACKAGE_VERSION} SOVERSION ${CPACK_PACKAGE_VERSION_MAJOR})