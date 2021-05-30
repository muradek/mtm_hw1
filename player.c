#include "map.h"
#include "chessSystem.h"
#include "player.h"
#include "tournament.h"
#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// function to create player's data element
PlayerData playerDataCreate()
{
    PlayerData new_player = malloc(sizeof(*new_player));
    if (!new_player)
    {
        return NULL;
    }
    new_player->games_length = 0;
    new_player->games_count = 0;
    new_player->num_wins = 0;
    new_player->num_losses = 0;
    new_player->num_draws = 0;
    new_player->score = 0;
    new_player->level = 0;
    return new_player;
}

// function to create player's key element
PlayerKey playerKeyCreate(int player_id)
{
    PlayerKey new_player = malloc(sizeof(*new_player));
    if (!new_player)
    {
        return NULL;
    }

    new_player->player_id= player_id;
    new_player->deleted = false;
    return new_player;
}

// functions for a map of players
PlayerData copyPlayerData(PlayerData old_player)
{
    PlayerData new_player = malloc(sizeof(*new_player));
    if(!new_player)
    {
        return NULL;
    }
    new_player->games_count = old_player->games_count;
    new_player->games_length= old_player->games_length;
    new_player->level = old_player->level;
    new_player->num_draws = old_player->num_draws;
    new_player->num_wins = old_player->num_wins;
    new_player->num_losses = old_player->num_losses;
    new_player->score = old_player->score;
    return new_player;

}

PlayerKey copyPlayerKey(PlayerKey old_player)
{
    PlayerKey new_player = malloc(sizeof(*new_player));
    if(!new_player)
    {
        return NULL;
    }
    new_player->player_id = old_player->player_id;
    new_player->deleted = old_player->deleted;
    return new_player;
}

void freePlayerData(PlayerData data_to_free)
{
    free(data_to_free);
}

void freePlayerKey(PlayerKey key_to_free)
{
    free(key_to_free);
}

int comparePlayerKey(PlayerKey key_1, PlayerKey key_2)
{
    if (key_1->player_id == key_2->player_id && key_1->deleted == key_2->deleted)
    {
        return 0;
    }
    
    if(key_1->player_id > key_2->player_id)
    {
        return 1;
    }
    return -1;
}