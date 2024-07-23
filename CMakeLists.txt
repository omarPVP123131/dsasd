cmake_minimum_required(VERSION 3.5)

project(POS VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Buscar Qt (esto maneja tanto Qt5 como Qt6)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Sql Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Sql Network)

# Definir las fuentes del proyecto
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    inventario.cpp
    inventario.h
    bloc_de_notas.cpp
    bloc_de_notas.h
    calculadora.cpp
    calculadora.h
    clientes.cpp
    clientes.h
    configuracion.cpp
    configuracion.h
    estadisticas.cpp
    estadisticas.h
    historial.cpp
    historial.h
    punto_de_venta.cpp
    punto_de_venta.h
    proveedores.cpp
    proveedores.h
    servidor_local.cpp
    servidor_local.h
    resources.qrc
    styles/icons/calculadora.png
    styles/icons/clientes.png
    styles/icons/configuracion.png
    styles/icons/estadisticas.png
    styles/icons/historial.png
    styles/icons/inventario.png
    styles/icons/notas.png
    styles/icons/proveedores.png
    styles/icons/servidor.png
    styles/icons/venta.png
    reloj.cpp
    reloj.h
    DialogoPrevisualizacion.cpp
    DialogoPrevisualizacion.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(POS
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

    )
else()
    if(ANDROID)
        add_library(POS SHARED ${PROJECT_SOURCES})
    else()
        add_executable(POS ${PROJECT_SOURCES})
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(POS PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Sql)

if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.POS)
endif()

set_target_properties(POS PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS POS
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(POS)
endif()
