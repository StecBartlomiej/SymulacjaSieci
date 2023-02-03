cmake_minimum_required(VERSION 3.13)

# Ustaw nazwę projektu.
project(SymulacjaSieci)

# Ustaw standard C++ (tu: C++17).
set(CMAKE_CXX_STANDARD 17)

# Dodaj flagi kompilacji wymuszające pisanie kodu zgodnego ze standardem.
add_compile_options(-Wall -Wextra -Werror -Wpedantic -pedantic-errors)

# Dodaj katalogi z plikami nagłówkowymi dla wszystkich konfiguracji.
include_directories(
        include
        mocks
)

# Ustaw zmienną `SOURCE_FILES`, która będzie przechowywać ścieżki do
# tych plików źródłowych, które będą niezbędne w każdej konfiguracji.
file(GLOB SOURCE_FILES
        src/*.cpp
        )

# Dodaj konfigurację typu `Debug`.
add_executable(${PROJECT_NAME} ${SOURCE_FILES} main.cpp)


# == Unit testing using Google Testing Framework ==

# Ustaw zmienną `SOURCES_FILES_TESTS`, która będzie przechowywać ścieżki do
# plików źródłowych z testami jednostkowymi.
set(SOURCES_FILES_TESTS
        test/test_package.cpp
        test/test_storage_types.cpp
        test/test_nodes.cpp
        test/test_Factory.cpp
        test/test_factory_io.cpp
        test/test_reports.cpp
        )

# Dodaj konfigurację typu `Test`.
add_executable(${PROJECT_NAME}_test ${SOURCE_FILES} ${SOURCES_FILES_TESTS} test/main_gtest.cpp)

# Dodaj katalogi z plikami nagłówkowymi wyłącznie do konkretnej konfiguracji (tu: `Test`).
target_include_directories(${PROJECT_NAME}_test PUBLIC
        googletest-master/googlemock/include
        googletest-master/googletest/include
        mocks
        )

# Dodaj podkatalog `googletest-master`.
add_subdirectory(googletest-master)

# Podlinkuj bibliotekę o identyfikatorze `gmock` (w pliku CMake) wyłącznie do konkretnej
# konfiguracji (tu: `Test`).
target_link_libraries(${PROJECT_NAME}_test gmock)
target_compile_definitions(${PROJECT_NAME}_test PUBLIC EXERCISE_ID=EXERCISE_ID_FACTORY)