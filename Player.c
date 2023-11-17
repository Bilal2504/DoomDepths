#include "Player.h"
#include "Armor.h"
#include "Weapon.h"

// fonction de type * st_player pour créer le joueur au lancement du prg. Le void, on attend rien en paramètre d'entrée.
st_player * create_player (int id_db)
{
    st_player* p_player = malloc(sizeof(st_player)); //p_player = le joueur en gros.

    srand(time(NULL)); //pour que chaque génération diffère des précédentes / suivantes.


    if (id_db == 0)
    {

        //Création d'une arme et d'une armure par défaut
        Weapon *startWeapon = createWeapon(1);
        startWeapon->isEquipped = 1;
        startWeapon->name = "Baton en bois";



        Armor *startArmor = createArmor(1);
        startArmor->isEquipped = 1;
        startArmor->name = "Tenue simple";
        //printf("%s\n",startArmor->name);


        //default value, Hardcoded value.
        p_player->maxLife = 50;//rand() % 50; // la vie du MyPlayer. Valeur de base, à changer plus tard.
        p_player->currentLife = p_player->maxLife;
        p_player->maxMana = 80;
        p_player->currentMana = p_player->maxMana;
        p_player->gold = 0;
        p_player->minAttack = startWeapon->minDamage;
        p_player->attack = startWeapon->maxDamage;
        p_player->maxAttack = startWeapon->maxDamage;
        p_player->defense = startArmor->defense;
        p_player->weapons = NULL;
        p_player->armors = NULL;

        //Ajout des équipements par défaut dans la liste chainée
        p_player->weapons = addWeaponsPlayer(p_player, startWeapon);

        p_player->armors = addArmorsPlayer(p_player, startArmor);


    }
    else
    {
        //on a une DB
        int rc = 0;
        sqlite3 *db;
        rc = sqlite3_open("/Users/bilal/Documents/Cours/Piscine de C/DoomDepthsC_G21-2/DoomDepthsC_G21.db", &db);
        if (rc)
        {
            fprintf(stderr, "Impossible d'ouvrir la base de données : %s\n", sqlite3_errmsg(db));
            free (p_player);
            return NULL;
        }

        // PLAYER --------
        const char *select_player_sql = "SELECT * FROM JOUEUR WHERE id_sauvegarde = ?;";
        sqlite3_stmt *stmt;
        rc = sqlite3_prepare_v2(db, select_player_sql, -1, &stmt, 0);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "Erreur lors de la préparation de la requête : %s\n", sqlite3_errmsg(db));
        }

        sqlite3_bind_int(stmt, 1, id_db);


        sqlite3_step(stmt);

        int playerId = sqlite3_column_int(stmt, 0);
        int playerMaxLife = sqlite3_column_int(stmt, 1);
        int playerCurrentLife = sqlite3_column_int(stmt, 2);
        int playerAttack = sqlite3_column_int(stmt, 3);
        int gold = sqlite3_column_int(stmt, 4);
        int maxMana = sqlite3_column_int(stmt, 5);
        int currentMana = sqlite3_column_int(stmt, 6);


        p_player->maxLife = playerMaxLife;
        p_player->currentLife = playerCurrentLife;
        p_player->attack = playerAttack;
        p_player->gold = gold;
        p_player->maxMana = maxMana;
        p_player->currentMana = currentMana;
        p_player->weapons = NULL;
        p_player->armors = NULL;

        sqlite3_finalize(stmt);

        // WEAPON -----------------


        // LA REQUETE POUR REQUETE DES INFOS SUR LE NIVEAU
        const char *select_minMaxWeapon_sql = "SELECT COUNT(*) FROM WEAPON WHERE id_sauvegarde = ?;";
        sqlite3_stmt *stmt2;
        rc = sqlite3_prepare_v2(db, select_minMaxWeapon_sql, -1, &stmt2, 0);

        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "Erreur lors de la préparation de la requête : %s\n", sqlite3_errmsg(db));
        }

        // BIND LES ZONES INCONNUES AVEC DES VARIABLES
        sqlite3_bind_int(stmt2, 1, id_db);

        sqlite3_step(stmt2);

        int countWeapon = sqlite3_column_int(stmt2, 0);
        printf("Nombre d'armes : %d\n", countWeapon);

        // NOUVELLE REQUETE POUR RECUP LES DATA
        const char *select_weapons_sql = "SELECT W.nb_attack_per_round, W.minDamage, W.maxDamage, W.name, P.isEquiped "
                                         "FROM WEAPON W "
                                         "INNER JOIN PLAYER_WEAPON P ON W.id = P.weapon_id "
                                         "WHERE P.player_id = ?;";
        sqlite3_stmt *stmt3;
        rc = sqlite3_prepare_v2(db, select_weapons_sql, -1, &stmt3, 0);

        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "Erreur lors de la préparation de la requête : %s\n", sqlite3_errmsg(db));
        }

        sqlite3_bind_int(stmt3, 1, playerId);

        sqlite3_step(stmt3);

        int i = 0;

        while ((i < countWeapon))
        {

            int nbAttck = sqlite3_column_int(stmt3, 0);
            int minDamage = sqlite3_column_int(stmt3, 1);
            int maxDamage = sqlite3_column_int(stmt3, 2);
            const char *nameWeapon = (const char *)sqlite3_column_text(stmt3, 3);
            int isEquipedWeapon = sqlite3_column_int(stmt3, 4);

            Weapon *newWeapon = createWeaponSave(nbAttck, minDamage, maxDamage, nameWeapon);

            newWeapon->isEquipped = isEquipedWeapon;

            p_player->weapons = addWeaponsPlayer(p_player, newWeapon);

            i++; // Incrémentez le compteur
            sqlite3_step(stmt3);
        }
        sqlite3_finalize(stmt2);
        sqlite3_finalize(stmt3);

        // ARMOR -----------------

        // LA REQUETE POUR REQUETE DES INFOS SUR LE NIVEAU
        const char *select_minMaxArmor_sql = "SELECT COUNT(*) FROM ARMOR WHERE id_sauvegarde = ?;";
        sqlite3_stmt *stmt4;
        rc = sqlite3_prepare_v2(db, select_minMaxArmor_sql, -1, &stmt4, 0);

        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "Erreur lors de la préparation de la requête : %s\n", sqlite3_errmsg(db));
        }

        // BIND LES ZONES INCONNUES AVEC DES VARIABLES
        sqlite3_bind_int(stmt4, 1, id_db);

        sqlite3_step(stmt4);

        int countArmor = sqlite3_column_int(stmt4, 0);

        // NOUVELLE REQUETE POUR RECUP LES DATA
        const char *select_armors_sql = "SELECT A.name, A.defense, P.isEquiped "
                                        "FROM ARMOR A "
                                        "INNER JOIN PLAYER_ARMOR P ON A.id = P.armor_id "
                                        "WHERE P.player_id = ?;";
        sqlite3_stmt *stmt5;
        rc = sqlite3_prepare_v2(db, select_armors_sql, -1, &stmt5, 0);

        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "Erreur lors de la préparation de la requête : %s\n", sqlite3_errmsg(db));
        }

        sqlite3_bind_int(stmt5, 1, playerId);
        sqlite3_step(stmt5);

        int j = 0;

        while ((j < countArmor))
        {

            const char *nameArmor = (const char *)sqlite3_column_text(stmt5, 0);
            int defense = sqlite3_column_int(stmt5, 1);
            int isEquipedArmor = sqlite3_column_int(stmt5, 2);


            Armor *newArmor = createArmorSave(defense, nameArmor);

            newArmor->isEquipped = isEquipedArmor;

            p_player->armors = addArmorsPlayer(p_player, newArmor);

            j++; // Incrémentez le compteur
            sqlite3_step(stmt5);
        }

        sqlite3_finalize(stmt4);
        sqlite3_finalize(stmt5);

        sqlite3_close(db);

    }

    return p_player;
}


st_player * delete_player (st_player * p_player) //Supprime le joueur.
{
    //débug de vérification du free de l'ensemble des armes du joueur
    //je le fais ici car les armes ont besoin d'être free seulement lorsque le joueur meurt.
    //printf("DEBUG %s() Line %d \n ",__FUNCTION__ ,__LINE__);
    deleteWeaponsPlayer(p_player);
    //printf("DEBUG %s() Line %d \n ",__FUNCTION__ ,__LINE__);
    deleteArmorsPlayer(p_player);
    //printf("DEBUG %s() Line %d \n ",__FUNCTION__ ,__LINE__);
    //printf("DEBUG :: free p_player %d = %p \n",1,p_player); // débug, pour vérifier qu'il est bien désallouer, faudras appeler cette fonction, quand notre MyPlayer, mourra par exemple, ce sera la fin de la game.
    free (p_player);
    //printf("DEBUG %s() Line %d \n ",__FUNCTION__ ,__LINE__);
    return NULL;
}

int display_player (st_player *p_player) // affichage des statistiques du joueur.
{
    printf("Vie : %d / %d point de vie. \n",p_player->currentLife, p_player->maxLife);
    printf("Mana : %d / %d point de Mana. \n",p_player->currentMana, p_player->maxMana);
    printf("Or: %d\n", p_player->gold);
    //Affiche l'arme équipée
    Weapon * tmp = (Weapon *)p_player->weapons;
    while (tmp != NULL)
    {
        if (tmp->isEquipped == 1)
        {
            printf("Arme equipee : %s (damage: %d-%d)\n", tmp->name, p_player->minAttack, p_player->maxAttack);
            break;
        }
        tmp = (Weapon *)tmp->next;
    }
    //Affiche l'armure équipée
    Armor *tmp2 = (Armor *)p_player->armors;
    while (tmp2 != NULL)
    {
        if (tmp2->isEquipped == 1)
        {
            printf("Armure equipee : %s (defense: %d)\n", tmp2->name, p_player->defense);
            break;
        }
        tmp2 = (Armor *)tmp2->next;
    }
    return 0;
}

void changeMinAndMaxAttackValues(st_player *p_player)
{
    Weapon *tmp = (Weapon *)p_player->weapons;
    while (tmp != NULL)
    {
        if (tmp->isEquipped == 1)
        {
            p_player->maxAttack = tmp->maxDamage;
            p_player->minAttack = tmp->minDamage;
            break;
        }
        tmp = (Weapon *)tmp->next;
    }
}

void changeDefenseValue(st_player *p_player)
{
    Armor *tmp = (Armor *)p_player->armors;
    while (tmp != NULL)
    {
        if (tmp->isEquipped == 1)
        {
            p_player->defense = tmp->defense;
            break;
        }
        tmp = (Armor *)tmp->next;
    }
}