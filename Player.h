#ifndef DOOMDEPTHSC_G21_PLAYER_H
#define DOOMDEPTHSC_G21_PLAYER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct st_player
{
    int maxLife; //stats player
    int currentLife;
    int maxMana;
    int currentMana;
    int minAttack;
    int maxAttack;
    int attack;
    int defense;
    int gold;
    int* weapons;
    int* armors;
} st_player;

st_player *create_player(int id_db);

st_player *delete_player(st_player *p_player);

int display_player(st_player *p_player);

void changeMinAndMaxAttackValues(st_player *p_player);

void changeDefenseValue(st_player *p_player);

#endif //DOOMDEPTHSC_G21_PLAYER_H
