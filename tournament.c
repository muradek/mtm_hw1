#include "map.h"
#include "chessSystem.h"
#include "tournament.h"
#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

//the data each tournament contains
struct tournament_data_t
{
    const char* location;
    int winner;
    int max_games_per_player;
    Map game_map;
};

//the key to compare different tournaments in a map
struct tournament_key_t
{
    int key;
};

//functions to create Data&Key structs
TournamentData tournamentDataCreate(const char* location, int winner, int max_games_per_player, Map game_map)
{
    TournamentData tournament= malloc(sizeof(*tournament));
    if(!tournament)
    {
        return NULL;
    }
    tournament->location=location;
    tournament->winner=winner;
    tournament->max_games_per_player=max_games_per_player;
    tournament->game_map= game_map;
    return tournament;
}

TournamentKey tournamentKeyCreate(int ID)
{
    TournamentKey tournament_key= malloc(sizeof(*tournament_key));
    if (!tournament_key)
    {
        return NULL;
    }
    tournament_key->key= ID;
    return tournament_key;
}

//functions for a Map of tournaments
TournamentData copyTournamentData(TournamentData old_data)
{
    TournamentData new_data= malloc(sizeof(*new_data));
    new_data->location= old_data->location;
    new_data->winner=old_data->winner;
    new_data->max_games_per_player=old_data->max_games_per_player;
    new_data->game_map= mapCopy(old_data->game_map);
    return new_data;
}

TournamentKey copyTournamentKey(TournamentKey old_key)
{
    TournamentKey new_key= malloc(sizeof(new_key));
    if (!new_key)
    {
        return NULL;
    }
    new_key->key=old_key->key;
    return new_key;
}

void freeTournamentData(TournamentData data_to_free)
{
    mapDestroy(data_to_free->game_map);
    free((void*)(data_to_free->location));
    free(data_to_free);
}

void freeTournamentKey(TournamentKey key_to_free)
{
    free(key_to_free);
}

int compareTournamentKey(TournamentKey key_1, TournamentKey key_2)
{
    if (key_1->key < key_2->key)
    {
        return -1;
    }

    if (key_1->key > key_2->key)
    {
        return 1;
    }
    return 0;
}

int main()
{

Map tournament_map= mapCreate((copyMapDataElements) &copyTournamentData, (copyMapKeyElements) &copyTournamentKey, 
        (freeMapDataElements) &freeTournamentData, (freeMapKeyElements) &freeTournamentKey, (compareMapKeyElements) &compareTournamentKey);

Map game_map = mapCreate((copyMapDataElements) &copyGameData, (copyMapKeyElements) &copyGameKey, 
        (freeMapDataElements) &freeGameData, (freeMapKeyElements) &freeGameKey, (compareMapKeyElements) &compareGameKey);

//TournamentData tournament_data= tournamentDataCreate("Eilat", 2, 7, game_map);
/*
TournamentKey tournament_key= tournamentKeyCreate(3);

mapPut(tournament_map, tournament_key, tournament_data);
*/
mapDestroy(tournament_map);
mapDestroy(game_map);
return 0;
}