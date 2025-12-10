# CMakeList.txt : CMake project for Sorts, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Sorts")

# Add source to this project's executable.
add_executable (Sorts "src/main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Sorts PROPERTY CXX_STANDARD 20)
endif()

# Указываем где искать заголовки
target_include_directories(Sorts PRIVATE
    ${CMAKE_SOURCE_DIR}/headers
    ${CMAKE_SOURCE_DIR}/src    
)

# Опция для указания пути к тестам
set(TEST_DIR "${CMAKE_SOURCE_DIR}/Tests" CACHE PATH "Path to test files")

# Копируем тесты в build directory
file(COPY ${TEST_DIR} DESTINATION ${CMAKE_BINARY_DIR})

# Передаем путь к тестам в код
target_compile_definitions(Sorts PRIVATE 
   TEST_DIR="${CMAKE_BINARY_DIR}/Tests")