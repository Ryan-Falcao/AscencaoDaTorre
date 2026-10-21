cmake_minimum_required(VERSION 3.20)

project(AscensaoDaTorre VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(raylib CONFIG REQUIRED)

add_executable(ascensao_torre
    src/main.c
    src/input.c
    src/audio.c
    src/sprites.c
    src/fx.c
    src/playtest.c
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
    src/patterns.c src/motion_patterns.c
    src/hazards.c
    src/qte.c
    src/ui.c
)

target_include_directories(ascensao_torre PRIVATE include)
target_compile_options(ascensao_torre PRIVATE -Wall -Wextra)
target_link_libraries(ascensao_torre PRIVATE raylib)
add_custom_command(TARGET ascensao_torre POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets/fonts"
        "$<TARGET_FILE_DIR:ascensao_torre>/assets/fonts")

option(BUILD_TYPE_TESTS "Build the type matrix test" ON)
if(BUILD_TYPE_TESTS)
    add_executable(test_battle tests/test_battle.c src/battle.c src/player.c src/enemy.c
        src/items.c src/types.c src/qte.c src/input.c src/audio.c src/fx.c src/ui.c
        src/bullets.c src/patterns.c src/motion_patterns.c src/hazards.c src/sprites.c)
    target_include_directories(test_battle PRIVATE include)
    target_compile_options(test_battle PRIVATE -Wall -Wextra)
    target_link_libraries(test_battle PRIVATE raylib)

    add_executable(test_ui tests/test_ui.c src/sprites.c src/types.c)
    target_include_directories(test_ui PRIVATE include)
    target_compile_options(test_ui PRIVATE -Wall -Wextra)
    target_link_libraries(test_ui PRIVATE raylib)

    add_executable(test_types tests/test_types.c src/types.c)
    target_include_directories(test_types PRIVATE include)
    target_compile_options(test_types PRIVATE -Wall -Wextra)

    add_executable(test_qte tests/test_qte.c src/qte.c src/input.c src/ui.c src/sprites.c src/types.c)
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

    add_executable(test_balance tests/test_balance.c)
    target_include_directories(test_balance PRIVATE include)
    target_compile_options(test_balance PRIVATE -Wall -Wextra)

    add_executable(test_sprites tests/test_sprites.c src/sprites.c src/types.c)
    target_include_directories(test_sprites PRIVATE include)
    target_compile_options(test_sprites PRIVATE -Wall -Wextra)
    target_link_libraries(test_sprites PRIVATE raylib)

    add_executable(test_phase10 tests/test_phase10.c src/player.c src/items.c src/enemy.c src/types.c)
    target_include_directories(test_phase10 PRIVATE include)
    target_compile_options(test_phase10 PRIVATE -Wall -Wextra)
    target_link_libraries(test_phase10 PRIVATE raylib)

    add_executable(test_bullets tests/test_bullets.c src/bullets.c src/patterns.c src/motion_patterns.c src/hazards.c src/player.c src/types.c src/input.c src/ui.c src/sprites.c src/items.c src/enemy.c)
    target_include_directories(test_bullets PRIVATE include)
    target_compile_options(test_bullets PRIVATE -Wall -Wextra)
    target_link_libraries(test_bullets PRIVATE raylib)

    get_target_property(PATTERN_TEST_SOURCES test_bullets SOURCES)
    list(REMOVE_ITEM PATTERN_TEST_SOURCES tests/test_bullets.c)
    add_executable(test_patterns tests/test_patterns.c ${PATTERN_TEST_SOURCES})
    target_include_directories(test_patterns PRIVATE include)
    target_compile_options(test_patterns PRIVATE -Wall -Wextra)
    target_link_libraries(test_patterns PRIVATE raylib)
    add_executable(test_motion tests/test_motion.c ${PATTERN_TEST_SOURCES})
    target_include_directories(test_motion PRIVATE include)
    target_compile_options(test_motion PRIVATE -Wall -Wextra)
    target_link_libraries(test_motion PRIVATE raylib)

    add_executable(test_hazards tests/test_hazards.c src/hazards.c src/player.c src/items.c src/types.c src/sprites.c)
    target_include_directories(test_hazards PRIVATE include)
    target_compile_options(test_hazards PRIVATE -Wall -Wextra)
    target_link_libraries(test_hazards PRIVATE raylib)

    add_executable(test_equipment tests/test_equipment.c src/player.c src/items.c src/enemy.c src/types.c)
    target_include_directories(test_equipment PRIVATE include)
    target_compile_options(test_equipment PRIVATE -Wall -Wextra)
    target_link_libraries(test_equipment PRIVATE raylib)
endif()

set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG" CACHE STRING "Release flags" FORCE)

if(UNIX AND NOT APPLE)
    target_link_libraries(ascensao_torre PRIVATE m)
endif()

foreach(defense_target ascensao_torre test_battle test_bullets test_patterns test_motion)
    if(TARGET ${defense_target})
        target_sources(${defense_target} PRIVATE src/pilot_patterns.c src/plant_patterns.c src/plant_visual.c src/water_patterns.c src/water_visual.c src/attack_warning.c)
    endif()
endforeach()

if(BUILD_TYPE_TESTS)
    add_executable(test_pilot tests/test_pilot.c ${PATTERN_TEST_SOURCES} src/pilot_patterns.c src/plant_patterns.c src/plant_visual.c src/water_patterns.c src/water_visual.c src/attack_warning.c src/route_validator.c src/tower.c)
    target_include_directories(test_pilot PRIVATE include)
    target_compile_options(test_pilot PRIVATE -Wall -Wextra -O2 -UNDEBUG)
    target_link_libraries(test_pilot PRIVATE raylib)
endif()

if(BUILD_TYPE_TESTS)
    get_target_property(PILOT_PREVIEW_SOURCES ascensao_torre SOURCES)
    list(REMOVE_ITEM PILOT_PREVIEW_SOURCES src/main.c src/game.c)
    add_executable(pilot_preview EXCLUDE_FROM_ALL tests/preview_pilot.c ${PILOT_PREVIEW_SOURCES})
    target_include_directories(pilot_preview PRIVATE include)
    target_link_libraries(pilot_preview PRIVATE raylib)
endif()

if(BUILD_TYPE_TESTS)
    get_target_property(WATER_TEST_SOURCES test_pilot SOURCES)
    list(REMOVE_ITEM WATER_TEST_SOURCES tests/test_pilot.c)
    add_executable(test_water tests/test_water.c ${WATER_TEST_SOURCES})
    target_include_directories(test_water PRIVATE include)
    target_compile_options(test_water PRIVATE -Wall -Wextra -O2 -UNDEBUG)
    target_link_libraries(test_water PRIVATE raylib)
endif()
