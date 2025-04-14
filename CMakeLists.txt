
cmake_minimum_required(VERSION 3.11.0)
project(MainMapApps VERSION 0.1.0)

###############################################################################
# 3RD PARTY LIBS
###############################################################################

# FETCH CONTENT LIBS
include(FetchContent) 
# set(FETCHCONTENT_FULLY_DISCONNECTED ON)

FetchContent_Declare(glfw GIT_REPOSITORY https://github.com/glfw/glfw) 
FetchContent_MakeAvailable(glfw)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (WIN32)
  set(CMAKE_USE_SCHANNEL ON)
endif()

FetchContent_Declare(curl
URL                    https://github.com/curl/curl/releases/download/curl-7_75_0/curl-7.75.0.tar.xz
URL_HASH               SHA256=fe0c49d8468249000bda75bcfdf9e30ff7e9a86d35f1a21f428d79c389d55675 # the file hash for curl-7.75.0.tar.xz
USES_TERMINAL_DOWNLOAD TRUE) 
FetchContent_MakeAvailable(curl)
add_library(curl_int INTERFACE)
target_link_libraries(curl_int INTERFACE libcurl)
target_include_directories(curl_int INTERFACE ${curl_SOURCE_DIR}/include ${curl_BINARY_DIR}/include/curl)
add_library(CURL::libcurl ALIAS curl_int)

#below doesn't work
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()


# LOCAL LIBS

# GLAD
add_subdirectory(3rdparty/glad)

###############################################################################
# IMGUI
###############################################################################

set(IMGUI_HEADERS
    ../imgui/imconfig.h
    ../imgui/imgui_internal.h
    ../imgui/imgui.h
    ../imgui/imstb_rectpack.h
    ../imgui/imstb_textedit.h
    ../imgui/imstb_truetype.h    
    ../imgui/backends/imgui_impl_glfw.h
    ../imgui/backends/imgui_impl_opengl3.h
    ../imgui/backends/imgui_impl_opengl3_loader.h
    ../imgui/misc/cpp/imgui_stdlib.h
	../imgui/backends/imgui_impl_opengl3.h
)

set(IMGUI_SRC
    ../imgui/imgui_demo.cpp
    ../imgui/imgui_draw.cpp
    ../imgui/imgui_tables.cpp
    ../imgui/imgui_widgets.cpp
    ../imgui/imgui.cpp
    ../imgui/backends/imgui_impl_glfw.cpp
    ../imgui/backends/imgui_impl_opengl3.cpp
    ../imgui/misc/cpp/imgui_stdlib.cpp
	../imgui/backends/imgui_impl_opengl3.cpp
)


find_package(OpenGL REQUIRED)

add_library(imgui ${IMGUI_HEADERS} ${IMGUI_SRC})
if(MSVC)
  target_compile_options(imgui PRIVATE /W4 /WX /arch:AVX2 /fp:fast)
endif()
target_link_libraries(imgui PUBLIC glfw glad OpenGL::GL imm32)
target_compile_definitions(imgui PRIVATE IMGUI_DLL_EXPORT)

include_directories(../imgui/ ../imgui/examples ../imgui/examples/libs/gl3w ../imgui/backends ../imgui/misc/cpp)


project(MapBase)

set(
    source_list
    3rdparty/cxxopts.hpp
	
	Modules/POI.cpp
	Modules/POI.h
	Modules/route.h
	Modules/route.cpp
	Modules/DtedFile.hpp
	Modules/DtedFile.cpp
		
	shapeFiles/safileio.c
	shapeFiles/shpopen.c
	shapeFiles/shapefil.h		
	shapeFiles/shapefilemodule.cpp		
	shapeFiles/shapefilemodule.h
	shapeFiles/triangulate.h
	
	geodysy/geocalcs.cpp
	geodysy/geocalcs.h
	geodysy/geodesic.cpp
	geodysy/geodesic.h
	
	MainMapApp/about.cpp
	MainMapApp/AppOptions.cpp
	MainMapApp/GeoTool.cpp
	MainMapApp/hasMain.cpp
	MainMapApp/hasMain.h
	MainMapApp/help.cpp
	MainMapApp/ImBGUtil.cpp
	MainMapApp/ImBGUtil.h
	MainMapApp/ImGuiApp.h
	MainMapApp/maps.h
	MainMapApp/MapsToast.h
	MainMapApp/mapsupdate.cpp
	MainMapApp/mapsutility.cpp
	MainMapApp/mapsmapping.cpp
	MainMapApp/MouseManager.cpp
	MainMapApp/MouseManager.h
	MainMapApp/slippymaps.cpp
	MainMapApp/slippymaps.h
	MainMapApp/tileserver.h
	MainMapApp/tilemanager.cpp
	MainMapApp/tilemanager.h
	MainMapApp/RadarAppGlobals.cpp

	common/AppIni.h
	common/AppIni.cpp
	common/AppLogger.h
	common/AppLogger.cpp
	common/BGUtil.h
	common/BGUtil.cpp
	common/circular_buffer.h
	common/Helpers.h
	common/Image.h
	common/ImVec2Double.h
	common/L2DFileDialog.h
 	common/Mediator.h
	common/Mediator.cpp
	common/pugiconfig.hpp
	common/pugixml.hpp
	common/pugixml.cpp
	common/resource.h
	common/SocketStuff.cpp
	common/SocketStuff.h
	
	common/Fonts/font_awesome_5.h
	common/Fonts/FontAwesome5.cpp
	common/Fonts/FontAwesome5Brands.cpp
	common/Fonts/Fonts.h
	common/Fonts/IconsFontAwesome5.h
	common/Fonts/IconsFontAwesome5Brands.h
	common/Fonts/RobotoBold.cpp
	common/Fonts/RobotoItalic.cpp
	common/Fonts/RobotoMonoBold.cpp
	common/Fonts/RobotoMonoItalic.cpp
	common/Fonts/RobotoMonoRegular.cpp
	common/Fonts/RobotoRegular.cpp
	common/rapidjson/document.h
	common/rapidjson/writer.h
	common/rapidjson/stringbuffer.h
	common/libmgrs/mgrs.c
	common/libmgrs/mgrs.h
	common/libmgrs/polarst.c
	common/libmgrs/polarst.h
	common/libmgrs/tranmerc.c
	common/libmgrs/tranmerc.h
	common/libmgrs/ups.c
	common/libmgrs/ups.h
	common/libmgrs/utm.c
	
	SQLSupport/SQL_POIList.cpp
	SQLSupport/SQL_Common.cpp
	SQLSupport/SQL_Common.h
	SQLSupport/SQL_RouteList.h
	SQLSupport/SQL_RouteList.cpp
	SQLSupport/sqlite3.h
	SQLSupport/sqlite3.c	
)

add_executable(MapBase ${source_list})

foreach(source IN LISTS source_list)
    get_filename_component(source_path "${source}" PATH)
    string(REPLACE "/" "\\" source_path_msvc "${source_path}")
    source_group("${source_path_msvc}" FILES "${source}")
endforeach()

set_property(TARGET MapBase PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/RunFolder")
#set_property(TARGET MapBase PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/RunFolder")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MapBase)


target_link_libraries(MapBase imgui CURL::libcurl)
target_compile_features(MapBase PRIVATE cxx_std_17)
target_include_directories(MapBase PUBLIC 3rdparty)
target_include_directories(MapBase PUBLIC common)
target_include_directories(MapBase PUBLIC geodysy)
target_include_directories(MapBase PUBLIC MainMapApp)
target_include_directories(MapBase PUBLIC Modules)
target_include_directories(MapBase PUBLIC SQLSupport)
target_include_directories(MapBase PUBLIC shapeFiles)
set(HEADER_FILES 3rdparty/stb/stb_image.h )
set(HEADER_FILES 3rdparty/cxxopts.hpp )


