cmake_minimum_required(VERSION 3.26)
project(DoomDepthsC_G21 C)

set(CMAKE_C_STANDARD 11)

add_executable(DoomDepthsC_G21 main.c
        Player.c
        Player.h
        Monster.c
        Monster.h
        fight.c
        heal.c
        level.c
        level.h
        sqlite3.c
        sqlite3.h
        sqlite3ext.h
        gameState.c
        loadSave.c
        Weapon.h
        Weapon.c
        Armor.c
        Armor.h
        Sort.c
        Sort.h
        Display.c
)
