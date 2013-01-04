cmake_minimum_required (VERSION 2.8)
project (ancho)

set(${PROJECT_NAME}_GENERATED_FILES_DIR ${${PROJECT_NAME}_BINARY_DIR})
set(${PROJECT_NAME}_INC_DIRS ${${PROJECT_NAME}_SOURCE_DIR})
set(${PROJECT_NAME}_SRC_DIR ${${PROJECT_NAME}_SOURCE_DIR})
set(${PROJECT_NAME}_RC_DIR ${${PROJECT_NAME}_SOURCE_DIR})

set(${PROJECT_NAME}_INC_DIRS ${${PROJECT_NAME}_INC_DIRS} PARENT_SCOPE)
include_directories(
	${${PROJECT_NAME}_INC_DIRS}
	${AnchoBgSrv_GENERATED_FILES_DIR}
	)

set(${PROJECT_NAME}_HEADERS
	"${${PROJECT_NAME}_INC_DIRS}/AnchoAddon.h"
	"${${PROJECT_NAME}_INC_DIRS}/AnchoBrowserEvents.h"
	"${${PROJECT_NAME}_INC_DIRS}/AnchoPassthruAPP.h"
	"${${PROJECT_NAME}_INC_DIRS}/AnchoRuntime.h"
	"${${PROJECT_NAME}_INC_DIRS}/dllmain.h"
	"${${PROJECT_NAME}_INC_DIRS}/DOMWindowWrapper.h"
	"${${PROJECT_NAME}_INC_DIRS}/HtmlPanel.h"
	"${${PROJECT_NAME}_INC_DIRS}/HtmlPanelUIHandler.h"
	"${${PROJECT_NAME}_INC_DIRS}/HtmlToolbarWindow.h"
	"${${PROJECT_NAME}_INC_DIRS}/resource.h"
	"${${PROJECT_NAME}_INC_DIRS}/stdafx.h"
	"${${PROJECT_NAME}_INC_DIRS}/targetver.h"
	"${${PROJECT_NAME}_INC_DIRS}/Toolbar.h"
	"${${PROJECT_NAME}_INC_DIRS}/Toolbar.ipp"
	)

set(${PROJECT_NAME}_SOURCES 
	"${${PROJECT_NAME}_SRC_DIR}/ancho.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/AnchoAddon.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/AnchoBrowserEvents.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/AnchoPassthruAPP.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/AnchoRuntime.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/dllmain.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/DOMWindowWrapper.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/HtmlPanel.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/HtmlPanelUIHandler.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/HtmlToolbarWindow.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/stdafx.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/Toolbar.cpp"
	)

set(${PROJECT_NAME}_MIDL_TLB_FILE "${${PROJECT_NAME}_GENERATED_FILES_DIR}/${PROJECT_NAME}.tlb")

set(${PROJECT_NAME}_MIDL_I_H_FILE	"${${PROJECT_NAME}_GENERATED_FILES_DIR}/${PROJECT_NAME}_i.h")

set(${PROJECT_NAME}_MIDL_I_C_FILE	"${${PROJECT_NAME}_GENERATED_FILES_DIR}/${PROJECT_NAME}_i.c")

set(${PROJECT_NAME}_MIDL_P_C_FILE	"${${PROJECT_NAME}_GENERATED_FILES_DIR}/${PROJECT_NAME}_p.c")

set(${PROJECT_NAME}_GENERATED_FILES
	"${${PROJECT_NAME}_MIDL_TLB_FILE}"
	"${${PROJECT_NAME}_MIDL_I_H_FILE}"
  "${${PROJECT_NAME}_MIDL_I_C_FILE}"
	#  "${${PROJECT_NAME}_MIDL_P_C_FILE}"
	)

set (${PROJECT_NAME}_IDL_FILE
  "${${PROJECT_NAME}_SRC_DIR}/${PROJECT_NAME}.idl"
)

set (${PROJECT_NAME}_RC_FILES
  ${${PROJECT_NAME}_RC_DIR}/${PROJECT_NAME}.rc
  ${${PROJECT_NAME}_RC_DIR}/${PROJECT_NAME}.rgs
	${${PROJECT_NAME}_RC_DIR}/AnchoRuntime.rgs
	${${PROJECT_NAME}_RC_DIR}/AnchoToolbar.rgs
)

set (${PROJECT_NAME}_DEF_FILES
  "${${PROJECT_NAME}_SRC_DIR}/${PROJECT_NAME}.def"
)

set_source_files_properties(
	${${PROJECT_NAME}_MIDL_TLB_FILE}
	${${PROJECT_NAME}_MIDL_I_H_FILE}
	${${PROJECT_NAME}_IDL_FILE}
	PROPERTIES
		HEADER_FILE_ONLY 1
		GENERATED 1
	)
set_source_files_properties(
	${${PROJECT_NAME}_MIDL_I_C_FILE}
	${${PROJECT_NAME}_MIDL_P_C_FILE}
	PROPERTIES
		GENERATED 1
	)

add_custom_command(OUTPUT ${${PROJECT_NAME}_GENERATED_FILES}
                   COMMAND midl.exe "${${PROJECT_NAME}_IDL_FILE}"
                     /D "_DEBUG"
                     /D "MODULES_HAVE_EXPORT_PROPERTY"
										 /I ${AnchoBgSrv_GENERATED_FILES_DIR}
                     /nologo
                     /char signed
                     /env win32
                     /Oicf
                     /tlb "${${PROJECT_NAME}_MIDL_TLB_FILE}"
										 /h "${${PROJECT_NAME}_MIDL_I_H_FILE}"
                     /iid "${${PROJECT_NAME}_MIDL_I_C_FILE}"
                     /proxy "${${PROJECT_NAME}_MIDL_P_C_FILE}"
                     /error stub_data
                   WORKING_DIRECTORY "${${PROJECT_NAME}_GENERATED_FILES_DIR}"
                   DEPENDS "${${PROJECT_NAME}_IDL_FILE}"
                   )


add_library(${PROJECT_NAME} SHARED 
	${${PROJECT_NAME}_HEADERS} 
	${${PROJECT_NAME}_SOURCES}
	${${PROJECT_NAME}_GENERATED_FILES}
  ${${PROJECT_NAME}_RC_FILES}
	${${PROJECT_NAME}_DEF_FILES}
	)
target_link_libraries(${PROJECT_NAME} libbhohelper anchocommons libprothandlers)

# set UNICODE
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS _UNICODE,UNICODE)

add_custom_command(TARGET ${PROJECT_NAME}
                     POST_BUILD
                     COMMAND "regsvr32" ARGS "/s" "\$(TargetPath)"
                     )

message(STATUS "Generated ${PROJECT_NAME}")
