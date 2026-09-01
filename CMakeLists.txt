cmake_minimum_required(VERSION 3.16)

project(android_prime VERSION 0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Устанавливаем стандарт (например, 20), иначе CMAKE_CXX_STANDARD_REQUIRED не сработает
set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_TRANSLATIONS OFF CACHE BOOL "" FORCE)
set(BUILD_TEST_APPLICATION OFF CACHE BOOL "" FORCE)
set(BUILD_WITH_QT6 ON CACHE BOOL "" FORCE)
# 1. Жестко блокируем поиск в Homebrew, чтобы маковский Qt не ломал кросс-компиляцию
list(APPEND CMAKE_IGNORE_PATH "/opt/homebrew")

# 2. Разрешаем CMake искать зависимости по абсолютным путям вне NDK sysroot
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)

# 3. Принудительно ставим Android-версию Qt на самое первое место в поиске
list(PREPEND CMAKE_PREFIX_PATH "/Users/philingosling/Qt/6.10.0/android_arm64_v8a")

# Поиск зависимостей через Conan (CMakeDeps)
find_package(Qt6 REQUIRED COMPONENTS Quick)
qt_standard_project_setup(REQUIRES 6.10)

find_package(Boost REQUIRED COMPONENTS json log thread)
find_package(OpenSSL REQUIRED)
find_package(libsodium REQUIRED)
find_package(Protobuf REQUIRED)

# Генерация кода Protobuf (лучше сделать статическую библиотеку для Android)
add_library(proto_objects STATIC message.proto)
target_link_libraries(proto_objects PUBLIC protobuf::libprotobuf)
protobuf_generate(TARGET proto_objects LANGUAGE cpp)
target_include_directories(proto_objects PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

# Подключение QML и исходников
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/importedcontent/CMakeLists.txt")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/importedcontent)
endif()

qt_add_executable(appandroid_prime main.cpp)

qt_add_qml_module(appandroid_prime
    URI android_prime
    QML_FILES
        Main.qml
        RegisterScreen.qml
        LoginScreen.qml
        ChatsScreen.qml
    SOURCES
        client.h client.cpp
        websocket.h websocket.cpp
    # Убрали $<TARGET_OBJECTS:proto_objects> отсюда, слинкуем ниже
)

set_target_properties(appandroid_prime PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Линковка всех зависимостей
target_link_libraries(appandroid_prime
    PRIVATE 
        Qt6::Quick 
        Boost::json 
        Boost::log 
        Boost::thread
        OpenSSL::SSL 
        OpenSSL::Crypto
        libsodium::libsodium
        protobuf::libprotobuf
        proto_objects # Линкуем сгенерированные прото-классы
)

install(TARGETS appandroid_prime
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)