cmake_minimum_required(VERSION 3.16)

project(Debil_Browser VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools WebEngineWidgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools WebEngineWidgets Multimedia)

set(TS_FILES Debil_Browser_en_001.ts)

set(PROJECT_SOURCES
        src/main.cpp


        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Debil_Browser
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        src/main.cpp
        src/core/browserview.h src/core/browserview.cpp
        src/core/mainpannel.h src/core/mainpannel.cpp

        src/core/browser_framework.h src/core/browser_framework.cpp
        src/ui/navigationwidget.h src/ui/navigationwidget.cpp
        src/web/searchengine_factory.h
        src/web/search_engines/isearchengine.h
        src/web/search_engines/googlesearchengine.h
        src/web/search_engines/yandexsearchengine.h
        src/ui/addressbarwidget.h src/ui/addressbarwidget.cpp
        src/custom_widgets/popupmenuwidget.h src/custom_widgets/popupmenuwidget.cpp
        src/web/searchengineselector.h src/web/searchengineselector.cpp
        src/core/headerpanel.h src/core/headerpanel.cpp



        src/core/tabmanager.h src/core/tabmanager.cpp
        src/custom_widgets/browsertab.h src/custom_widgets/browsertab.cpp
        src/ui/tabcontent.h src/ui/tabcontent.cpp
        src/core/mergered_tabscontent.h src/core/mergered_tabscontent.cpp
        src/ui/windowbar.h src/ui/windowbar.cpp



    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Debil_Browser APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(Debil_Browser SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(Debil_Browser
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

qt_add_resources(BrowserResources
    resources.qrc
)

target_sources(Debil_Browser PRIVATE
    ${BrowserResources}
)


target_link_libraries(Debil_Browser PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                                            Qt${QT_VERSION_MAJOR}::WebEngineWidgets
                                            Qt${QT_VERSION_MAJOR}::Multimedia)



# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.Debil_Browser)
endif()
set_target_properties(Debil_Browser PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS Debil_Browser
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Debil_Browser)
endif()

include(CTest)

if(BUILD_TESTING)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

    add_executable(Debil_BrowserTests
        src/tests/browser_components_test.cpp
        src/ui/addressbarwidget.h src/ui/addressbarwidget.cpp
        src/web/searchengineselector.h src/web/searchengineselector.cpp
        src/custom_widgets/popupmenuwidget.h src/custom_widgets/popupmenuwidget.cpp
        src/web/searchengine_factory.h
        src/web/search_engines/isearchengine.h
        src/web/search_engines/googlesearchengine.h
        src/web/search_engines/yandexsearchengine.h
    )

    target_link_libraries(Debil_BrowserTests PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
    )

    add_test(NAME Debil_BrowserTests COMMAND Debil_BrowserTests)
    set_tests_properties(Debil_BrowserTests PROPERTIES
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    )
endif()
