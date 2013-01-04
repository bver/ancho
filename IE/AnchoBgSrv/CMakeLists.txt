cmake_minimum_required (VERSION 2.8)
project (AnchoBgSrv)

set(${PROJECT_NAME}_GENERATED_FILES_DIR ${${PROJECT_NAME}_BINARY_DIR})
set(${PROJECT_NAME}_INC_DIRS ${${PROJECT_NAME}_SOURCE_DIR})
set(${PROJECT_NAME}_SRC_DIR ${${PROJECT_NAME}_SOURCE_DIR})
set(${PROJECT_NAME}_RC_DIR ${${PROJECT_NAME}_SOURCE_DIR})


include_directories("${${PROJECT_NAME}_INC_DIRS}")

set(${PROJECT_NAME}_HEADERS
	"${${PROJECT_NAME}_INC_DIRS}/AnchoAddonService.h"
	"${${PROJECT_NAME}_INC_DIRS}/AnchoBackgroundAPI.h"
	"${${PROJECT_NAME}_INC_DIRS}/AnchoBackground.h"
	"${${PROJECT_NAME}_INC_DIRS}/AnchoBgSrvModule.h"
	"${${PROJECT_NAME}_INC_DIRS}/BackgroundWindow.h"
	"${${PROJECT_NAME}_INC_DIRS}/CommandQueue.h"
	"${${PROJECT_NAME}_INC_DIRS}/Exceptions.h"
	"${${PROJECT_NAME}_INC_DIRS}/IECookie.h"
	"${${PROJECT_NAME}_INC_DIRS}/IECookieManager.h"
	"${${PROJECT_NAME}_INC_DIRS}/LogView.h"
	"${${PROJECT_NAME}_INC_DIRS}/LogViewRE.h"
	"${${PROJECT_NAME}_INC_DIRS}/LogWindow.h"
	"${${PROJECT_NAME}_INC_DIRS}/PopupWindow.h"
	"${${PROJECT_NAME}_INC_DIRS}/resource.h"
	"${${PROJECT_NAME}_INC_DIRS}/stdafx.h"
	"${${PROJECT_NAME}_INC_DIRS}/targetver.h"
	"${${PROJECT_NAME}_INC_DIRS}/XmlHttpRequest.h"
	)

set(${PROJECT_NAME}_SOURCES 
	"${${PROJECT_NAME}_SRC_DIR}/AnchoAddonService.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/AnchoBackgroundAPI.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/AnchoBackground.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/AnchoBgSrv.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/AnchoBgSrvModule.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/BackgroundWindow.cpp"
	#	"${${PROJECT_NAME}_SRC_DIR}/dlldata.c"
	"${${PROJECT_NAME}_SRC_DIR}/IECookie.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/IECookieManager.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/LogView.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/LogViewRE.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/LogWindow.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/PopupWindow.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/stdafx.cpp"
	"${${PROJECT_NAME}_SRC_DIR}/XmlHttpRequest.cpp"
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
	${${PROJECT_NAME}_RC_DIR}/AnchoAddonService.rgs
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


add_executable(${PROJECT_NAME} WIN32 
	${${PROJECT_NAME}_HEADERS} 
	${${PROJECT_NAME}_SOURCES}
	${${PROJECT_NAME}_GENERATED_FILES}
	${${PROJECT_NAME}_RC_FILES}
	)
target_link_libraries(${PROJECT_NAME} libbhohelper anchocommons libprothandlers Wininet)

# set UNICODE
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS _UNICODE,UNICODE)

add_custom_command(TARGET ${PROJECT_NAME}
                     POST_BUILD
                     COMMAND "\$(TargetPath)" ARGS "/RegServer" "/s"
                     )


set(${PROJECT_NAME}_INC_DIRS ${${PROJECT_NAME}_INC_DIRS} PARENT_SCOPE)
set(${PROJECT_NAME}_GENERATED_FILES_DIR ${${PROJECT_NAME}_GENERATED_FILES_DIR} PARENT_SCOPE)

message(STATUS "Generated ${PROJECT_NAME}")
