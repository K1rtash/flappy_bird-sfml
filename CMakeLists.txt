cmake_minimum_required(VERSION 3.10)
project("flappy_bird_sfml")

set(CMAKE_CXX_STANDARD 23)

# Recoger todos los .cpp de src/
file(GLOB SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")

SET(SFML3_DIR "C:/DEV/SFML/")

# Crear el ejecutable
add_executable(${PROJECT_NAME} ${SRC_FILES} game/resources.rc)

target_link_libraries(${PROJECT_NAME} PRIVATE
    "${SFML3_DIR}/lib/libsfml-graphics.a"
    "${SFML3_DIR}/lib/libsfml-window.a"
    "${SFML3_DIR}/lib/libsfml-system.a"
    "${SFML3_DIR}/lib/libsfml-audio.a"
)

target_include_directories(${PROJECT_NAME} PRIVATE 
    "C:/DEV/SFML/include"
    "${CMAKE_SOURCE_DIR}/include"
)

# Copiar assets
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR}/)
file(COPY ${CMAKE_SOURCE_DIR}/game/highscore.dat DESTINATION ${CMAKE_BINARY_DIR}/)

# Copiar DLLs de SFML
file(GLOB SFML_DLLS C:/DEV/SFML/bin/*.dll)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SFML_DLLS}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
)