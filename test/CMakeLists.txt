set( _tests
  IntersonCxxIntersonClassTest
  IntersonCxxControlsHWControlsTest
  )

foreach( test ${_tests} )
  add_executable( ${test} ${test}.cxx )
  target_link_libraries( ${test} IntersonCxx )
  add_test( NAME ${test} COMMAND $<TARGET_FILE:${test}> )
endforeach()
