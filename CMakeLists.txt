cmake_minimum_required(VERSION 3.5)

project(TimeIsMoneyExternalApi_CMake VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/home/felix/Qt/6.5.0/gcc_64")
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(IsLibrary ON)
set(TESTING OFF)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Sql Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Sql Network)

set(PROJECT_SOURCES
    externalapi.cpp
    externalapi.hpp
    externalapi.ui
    externalapi_db.cpp
    externalapi_db.hpp
    externalapi_helpers.hpp
    externalapi_helpers.cpp
    externalapi_send.hpp
    externalapi_send.cpp
)

if (TESTING)
    enable_testing()

    include(FetchContent)
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        f8d7d77c06936315286eb55f8de22cd23c188571 # release-1.10.0
    )

    FetchContent_MakeAvailable(googletest)

    link_directories(
            /home/felix/QtProjects/TimeIsMoney_CMake/TimeIsMoney-TimeIsMoneyDB_CMake
    )
    include_directories(
            ${GTEST_INCLUDE_DIRS}
            /home/felix/QtProjects/TimeIsMoney_CMake/TimeIsMoney-TimeIsMoneyDB_CMake
    )

    qt_add_executable(TimeIsMoneyExternalApi_CMake
            MANUAL_FINALIZATION
            tests/testmain.cpp
            ${PROJECT_SOURCES}
            tests/test_db.cpp
            tests/test_externalapi_send.cpp
    )
    target_link_libraries(TimeIsMoneyExternalApi_CMake
            PRIVATE
            gtest_main
            Qt${QT_VERSION_MAJOR}::Core
            Qt${QT_VERSION_MAJOR}::Widgets
            Qt${QT_VERSION_MAJOR}::Sql
            Qt${QT_VERSION_MAJOR}::Network
            /home/felix/QtProjects/TimeIsMoney_CMake/cmake-build-debug/TimeIsMoney-TimeIsMoneyDB_CMake/libTimeIsMoneyDB_CMake.so
    )

    include(GoogleTest)
    gtest_discover_tests(${PROJECT_NAME})
else ()
    if (IsLibrary)
    else ()
        link_directories(/home/felix/QtProjects/TimeIsMoney_CMake/TimeIsMoney-TimeIsMoneyDB_CMake)
        include_directories(/home/felix/QtProjects/TimeIsMoney_CMake/TimeIsMoney-TimeIsMoneyDB_CMake)

        set(PROJECT_SOURCES PROJECT_SOURCES main.cpp
                tests/test_externalapi_send.cpp)
    endif ()

    if (IsLibrary)
        qt_add_library(TimeIsMoneyExternalApi_CMake MANUAL_FINALIZATION ${PROJECT_SOURCES})

        target_link_libraries(TimeIsMoneyExternalApi_CMake
                PRIVATE
                Qt${QT_VERSION_MAJOR}::Core
                Qt${QT_VERSION_MAJOR}::Widgets
                Qt${QT_VERSION_MAJOR}::Sql
                Qt${QT_VERSION_MAJOR}::Network
        )
    else ()
        qt_add_executable(TimeIsMoneyExternalApi_CMake
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}
        )
        target_link_libraries(TimeIsMoneyExternalApi_CMake
                PRIVATE
                Qt${QT_VERSION_MAJOR}::Core
                Qt${QT_VERSION_MAJOR}::Widgets
                Qt${QT_VERSION_MAJOR}::Sql
                Qt${QT_VERSION_MAJOR}::Network
                /home/felix/QtProjects/TimeIsMoney_CMake/cmake-build-debug/TimeIsMoney-TimeIsMoneyDB_CMake/libTimeIsMoneyDB_CMake.so
        )
    endif ()
endif ()


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.TimeIsMoneyExternalApi_CMake)
endif()
set_target_properties(TimeIsMoneyExternalApi_CMake PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS TimeIsMoneyExternalApi_CMake
    BUNDLE DESTINATION TimeIsMoneyExternalApi_CMake
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(TimeIsMoneyExternalApi_CMake)
endif()
