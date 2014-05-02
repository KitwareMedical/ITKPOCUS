if(CMAKE_CXX_FLAGS_DEBUG MATCHES "/RTC1")
  string(REPLACE "/RTC1" " " CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
endif()

if(CMAKE_CXX_FLAGS MATCHES "/EHsc")
  string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif()

set( IntersonCxx_SRCS
  IntersonCxxIntersonClass.cxx
  IntersonCxxControlsHWControls.cxx
  IntersonCxxImagingScan2DClass.cxx
  IntersonCxxImagingScanConverter.cxx
  )

include( GenerateExportHeader )
add_library( IntersonCxx SHARED
  ${IntersonCxx_SRCS}
  )
generate_export_header( IntersonCxx
  BASE_NAME IntersonCxx
  EXPORT_MACRO_NAME IntersonCxx_EXPORT
  EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/include/IntersonCxx_Export.h
  )
set_target_properties( IntersonCxx PROPERTIES
  COMPILE_FLAGS "/clr /EHa /AI${IntersonSDK_DIR}/Libraries"
  )

install( TARGETS IntersonCxx
  EXPORT IntersonSDKCxxTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT Runtime
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT Development
  )
if( BUILD_TESTING )
  # For running tests
  add_custom_command( TARGET IntersonCxx
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:IntersonCxx> "${PROJECT_BINARY_DIR}/test" 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${IntersonSDK_DIR}/Libraries/Interson.dll" "${PROJECT_BINARY_DIR}/test"
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:IntersonCxx> "${PROJECT_BINARY_DIR}/app/bin" 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${IntersonSDK_DIR}/Libraries/Interson.dll" "${PROJECT_BINARY_DIR}/app/bin"
    )
endif()
