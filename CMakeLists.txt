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
        src/ui/windowcontroller.h src/ui/windowcontroller.cpp
        ${TS_FILES}
)

if(WIN32)
    list(APPEND PROJECT_SOURCES windows/app.rc)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Debil_Browser
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        src/main.cpp
        src/core/mainpannel.h src/core/mainpannel.cpp

        src/core/browser_framework.h src/core/browser_framework.cpp
        src/ui/navigationwidget.h src/ui/navigationwidget.cpp
        src/web/searchengine_factory.h
        src/web/search_engines/isearchengine.h
        src/web/search_engines/googlesearchengine.h
        src/web/search_engines/yandexsearchengine.h
        src/ui/addressbarwidget.h src/ui/addressbarwidget.cpp
        src/ui/addressbardisplaywidget.h src/ui/addressbardisplaywidget.cpp
        src/custom_widgets/popupmenuwidget.h src/custom_widgets/popupmenuwidget.cpp
        src/web/searchengineselector.h src/web/searchengineselector.cpp
        src/core/headerpanel.h src/core/headerpanel.cpp



        src/custom_widgets/Tabs/tabmanager.h src/custom_widgets/Tabs/tabmanager.cpp
        src/custom_widgets/Tabs/webtab.h src/custom_widgets/Tabs/webtab.cpp
        src/custom_widgets/Tabs/tabcontent.h src/custom_widgets/Tabs/tabcontent.cpp
        src/custom_widgets/Tabs/mergered_tabscontent.h src/custom_widgets/Tabs/mergered_tabscontent.cpp
        src/ui/windowbar.h src/ui/windowbar.cpp
        src/custom_widgets/Tabs/homepagetab.h src/custom_widgets/Tabs/homepagetab.cpp
        src/custom_widgets/Tabs/iapptabs.h
        src/custom_widgets/Tabs/apptabs_factory.h
        src/custom_widgets/Tabs/bghomepage.h src/custom_widgets/Tabs/bghomepage.cpp
        src/custom_widgets/Tabs/browserview.h src/custom_widgets/Tabs/browserview.cpp
        src/custom_widgets/Tabs/homepagewidgets.h src/custom_widgets/Tabs/homepagewidgets.cpp
        src/custom_widgets/clockwidget.h src/custom_widgets/clockwidget.cpp




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

add_library(BrowserResourcesObject OBJECT ${BrowserResources})
target_link_libraries(BrowserResourcesObject PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_sources(Debil_Browser PRIVATE $<TARGET_OBJECTS:BrowserResourcesObject>)


target_link_libraries(Debil_Browser PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                                            Qt${QT_VERSION_MAJOR}::WebEngineWidgets
                                            Qt${QT_VERSION_MAJOR}::Multimedia)

if(WIN32)
    target_link_libraries(Debil_Browser PRIVATE dwmapi)
endif()



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
        browser_all_components_test.cpp
        src/ui/windowcontroller.h src/ui/windowcontroller.cpp
        src/core/browser_framework.h src/core/browser_framework.cpp
        src/core/mainpannel.h src/core/mainpannel.cpp
        src/core/headerpanel.h src/core/headerpanel.cpp
        src/ui/navigationwidget.h src/ui/navigationwidget.cpp
        src/ui/addressbarwidget.h src/ui/addressbarwidget.cpp
        src/ui/addressbardisplaywidget.h src/ui/addressbardisplaywidget.cpp
        src/ui/windowbar.h src/ui/windowbar.cpp
        src/web/searchengineselector.h src/web/searchengineselector.cpp
        src/custom_widgets/popupmenuwidget.h src/custom_widgets/popupmenuwidget.cpp
        src/custom_widgets/clockwidget.h src/custom_widgets/clockwidget.cpp
        src/custom_widgets/Tabs/tabmanager.h src/custom_widgets/Tabs/tabmanager.cpp
        src/custom_widgets/Tabs/webtab.h src/custom_widgets/Tabs/webtab.cpp
        src/custom_widgets/Tabs/tabcontent.h src/custom_widgets/Tabs/tabcontent.cpp
        src/custom_widgets/Tabs/mergered_tabscontent.h src/custom_widgets/Tabs/mergered_tabscontent.cpp
        src/custom_widgets/Tabs/homepagetab.h src/custom_widgets/Tabs/homepagetab.cpp
        src/custom_widgets/Tabs/iapptabs.h
        src/custom_widgets/Tabs/apptabs_factory.h
        src/custom_widgets/Tabs/bghomepage.h src/custom_widgets/Tabs/bghomepage.cpp
        src/custom_widgets/Tabs/browserview.h src/custom_widgets/Tabs/browserview.cpp
        src/custom_widgets/Tabs/homepagewidgets.h src/custom_widgets/Tabs/homepagewidgets.cpp
        src/web/searchengine_factory.h
        src/web/search_engines/isearchengine.h
        src/web/search_engines/googlesearchengine.h
        src/web/search_engines/yandexsearchengine.h
    )

    target_sources(Debil_BrowserTests PRIVATE $<TARGET_OBJECTS:BrowserResourcesObject>)

    target_link_libraries(Debil_BrowserTests PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::WebEngineWidgets
        Qt${QT_VERSION_MAJOR}::Multimedia
    )

    add_test(NAME Debil_BrowserTests COMMAND Debil_BrowserTests)
    if(WIN32)
        target_link_libraries(Debil_BrowserTests PRIVATE dwmapi)
    endif()
    set_tests_properties(Debil_BrowserTests PROPERTIES
        TIMEOUT 180
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QTWEBENGINE_DISABLE_SANDBOX=1;QT_OPENGL=software;QTWEBENGINE_CHROMIUM_FLAGS=--disable-gpu --no-sandbox"
    )
endif()
