cmake_minimum_required(VERSION 3.20)

project(AscensaoDaTorre VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

find_package(raylib CONFIG REQUIRED)

add_executable(ascensao_torre
    src/main.c
    src/input.c
    src/save.c
    src/render.c
    src/game.c
    src/tower.c
    src/battle.c
    src/player.c
    src/items.c
    src/enemy.c
    src/types.c
    src/bullets.c
    src/qte.c
    src/ui.c
)

target_include_directories(ascensao_torre PRIVATE include)
target_compile_options(ascensao_torre PRIVATE -Wall -Wextra)
target_link_libraries(ascensao_torre PRIVATE raylib)

option(BUILD_TYPE_TESTS "Build the type matrix test" ON)
if(BUILD_TYPE_TESTS)
    add_executable(test_types tests/test_types.c src/types.c)
    target_include_directories(test_types PRIVATE include)
    target_compile_options(test_types PRIVATE -Wall -Wextra)

    add_executable(test_qte tests/test_qte.c src/qte.c src/input.c)
    target_include_directories(test_qte PRIVATE include)
    target_compile_options(test_qte PRIVATE -Wall -Wextra)
    target_link_libraries(test_qte PRIVATE raylib)

    add_executable(test_phase6 tests/test_phase6.c src/player.c src/items.c src/enemy.c src/types.c)
    target_include_directories(test_phase6 PRIVATE include)
    target_compile_options(test_phase6 PRIVATE -Wall -Wextra)
    target_link_libraries(test_phase6 PRIVATE raylib)

    add_executable(test_tower tests/test_tower.c src/tower.c src/player.c src/items.c src/enemy.c src/types.c)
    target_include_directories(test_tower PRIVATE include)
    target_compile_options(test_tower PRIVATE -Wall -Wextra)
    target_link_libraries(test_tower PRIVATE raylib)

    add_executable(test_save tests/test_save.c src/save.c src/items.c)
    target_include_directories(test_save PRIVATE include)
    target_compile_options(test_save PRIVATE -Wall -Wextra)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(ascensao_torre PRIVATE m)
endif()
