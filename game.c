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


// function that creats game data element
GameData gameDataCreate(Winner winner, int game_length)
{
    GameData game_data= malloc(sizeof(*game_data));
    if (!game_data)
    {
        return NULL;
    }
    game_data->winner=winner;
    game_data->game_length=game_length;
    return game_data;
}

// function that creats game key element
GameKey gameKeyCreate(int first_id, int second_id)
{
    GameKey game_key= malloc(sizeof(*game_key));
    if (!game_key)
    {
        return NULL;
    }
    game_key->first_player_id=first_id;
    game_key->second_player_id=second_id;
    return game_key;
}

//functions for a Map of games
GameData copyGameData(GameData old_game)
{
    GameData new_game=malloc(sizeof(*new_game));
    if (!new_game)
    {
        return NULL;
    }
    new_game->winner=old_game->winner;
    new_game->game_length=old_game->game_length;
    return new_game;
}

GameKey copyGameKey(GameKey old_game)
{
    GameKey new_game=malloc(sizeof(*new_game));
    if (!new_game)
    {
        return NULL;
    }
    new_game->first_player_id= old_game->first_player_id;
    new_game->second_player_id= old_game->second_player_id;
    return new_game;
}

void freeGameData(GameData data_to_free)
{
    free(data_to_free);
}

void freeGameKey(GameKey key_to_free)
{
    free(key_to_free);
}

int compareGameKey(GameKey key_1, GameKey key_2)
{
    if((key_1->first_player_id==key_2->first_player_id && key_1->second_player_id==key_2->second_player_id) || 
        (key_1->first_player_id==key_2->second_player_id && key_1->second_player_id==key_2->first_player_id))
        {
            return 0;
        }

    else if (key_1->first_player_id>key_2->first_player_id) //is it ok?
    {
        return 1;
    }

    return -1;
}
