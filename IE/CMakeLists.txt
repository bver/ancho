cmake_minimum_required (VERSION 2.8)
project (AnchoFramework)


add_subdirectory(libbhohelper)
include_directories(${libbhohelper_INC_DIR})
link_directories(${libbhohelper_ARCHIVE_DIR})

add_subdirectory(Magpie)
add_dependencies(Magpie libbhohelper)
include_directories(${MagpieProject_INC_DIRS})

add_subdirectory(libprothandlers)
include_directories(${libprothandlers_INC_DIR})

add_subdirectory(passthruapp)
include_directories(${passthruapp_INC_DIR})

#Ancho components
add_subdirectory(anchocommons)
include_directories(${anchocommons_INC_DIR})

add_subdirectory(AnchoBgSrv)
add_dependencies(AnchoBgSrv anchocommons Magpie libbhohelper libprothandlers)# passthruapp)
include_directories(${AnchoBgSrv_INC_DIR})

add_subdirectory(ancho)
add_dependencies(ancho anchocommons AnchoBgSrv Magpie libbhohelper libprothandlers)

set_target_properties(Magpie PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${${PROJECT_NAME}_BINARY_DIR}/")
set_target_properties(ancho PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${${PROJECT_NAME}_BINARY_DIR}/")
set_target_properties(AnchoBgSrv PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${${PROJECT_NAME}_BINARY_DIR}/")


#get_cmake_property(_variableNames VARIABLES)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()
