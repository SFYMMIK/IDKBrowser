cmake_minimum_required(VERSION 3.10)
project(WebBrowser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

# Enable Qt MOC and UI auto-processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Find required packages
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Widgets WebEngineWidgets)

# Source files
add_executable(WebBrowser
    src/main.cpp
    src/Browser.cpp
    src/NetworkManager.c
    src/PrivacyManager.cpp
    src/MainWindow.cpp
)

# Include Qt headers
target_include_directories(WebBrowser PRIVATE
    ${CURL_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(WebBrowser
    CURL::libcurl
    OpenSSL::SSL
    OpenSSL::Crypto
    Qt5::Widgets
    Qt5::WebEngineWidgets
    Qt5::WebChannel
)