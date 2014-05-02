set( IntersonCxx_HEADERS
  IntersonCxxControlsHWControls.h
  IntersonCxxImagingScan2DClass.h
  IntersonCxxImagingScanConverter.h
  IntersonCxxIntersonClass.h
  )
install( FILES
  ${IntersonCxx_HEADERS}
  DESTINATION "${INSTALL_INCLUDE_DIR}" COMPONENT Development
  )
