#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "rapidwebsift::rapidwebsift" for configuration ""
set_property(TARGET rapidwebsift::rapidwebsift APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(rapidwebsift::rapidwebsift PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/librapidwebsift.a"
  )

list(APPEND _cmake_import_check_targets rapidwebsift::rapidwebsift )
list(APPEND _cmake_import_check_files_for_rapidwebsift::rapidwebsift "${_IMPORT_PREFIX}/lib/librapidwebsift.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
