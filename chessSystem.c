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


static bool checkLocation(const char* location)
{
    if (strlen(location)<=0)
    {
        return false;
    }

    if (isupper(location[0])==0)
    {
        return false;
    }

    for (int i=1; i<strlen(location); i++)
    {
        if (islower(location[i])==0 && isspace(location[i])==0)
        {
            return false;
        }
    }

    return true;
}

struct chess_system_t
{
    Map tournaments_map;
    Map games_map;
    Map players_map;
};

ChessSystem chessCreate()
{
    ChessSystem chess_system=malloc(sizeof(*chess_system));
    if (!chess_system)
    {
        return NULL;
    }

    chess_system->games_map = mapCreate((copyMapDataElements) &copyGameData, (copyMapKeyElements) &copyGameKey, 
        (freeMapDataElements) &freeGameData, (freeMapKeyElements) &freeGameKey, (compareMapKeyElements) &compareGameKey);
    
    chess_system->tournaments_map= mapCreate((copyMapDataElements) &copyTournamentData, (copyMapKeyElements) &copyTournamentKey, 
        (freeMapDataElements) &freeTournamentData, (freeMapKeyElements) &freeTournamentKey, (compareMapKeyElements) &compareTournamentKey);
    
    chess_system->players_map = mapCreate((copyMapDataElements) &copyPlayerData, (copyMapKeyElements) &copyPlayerKey,
    (freeMapDataElements) &freePlayerData, (freeMapKeyElements) &freePlayerKey, (compareMapKeyElements) &comparePlayerKey);

    return chess_system;
}

void chessDestroy(ChessSystem chess)
{
    if (!chess)
    {
        return;
    }

    mapDestroy(chess->games_map);
    mapDestroy(chess->players_map);
    mapDestroy(chess->tournaments_map);
    free(chess);
}

ChessResult chessAddTournament(ChessSystem chess, int tournament_id, int max_games_per_player, const char* tournament_location)
{
    if (tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }

    TournamentKey tournament_key= tournamentKeyCreate(tournament_id);

    if (mapContains(chess->tournaments_map, tournament_key))
    {
        freeTournamentKey(tournament_key);
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }

    if(!(checkLocation(tournament_location)))
    {
        freeTournamentKey(tournament_key);
        return CHESS_INVALID_LOCATION;
    }

    if(max_games_per_player <= 0)
    {
        freeTournamentKey(tournament_key);
        return CHESS_INVALID_MAX_GAMES;
    }

    TournamentData tournament_data= tournamentDataCreate(tournament_location, -1, max_games_per_player);

    mapPut(chess->tournaments_map, tournament_key, tournament_data);

    return CHESS_SUCCESS;
}

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player, int second_player, Winner winner, int play_time)
{
    if (tournament_id <= 0 || first_player <= 0 || second_player <= 0 || first_player == second_player)
    {
        return CHESS_INVALID_ID;
    }

    // check if the tournament is in the chess system
    TournamentKey tournament_key = tournamentKeyCreate (tournament_id);
    if (!mapContains(chess->tournaments_map, tournament_key))
    {
        freeTournamentKey(tournament_key);
        return CHESS_TOURNAMENT_NOT_EXIST;
    }

    // check if the tournament has already ended 
    TournamentData tournament_data = (TournamentData) mapGet(chess->tournaments_map, tournament_key);
    if (tournament_data->ended)
    {
        freeTournamentData(tournament_data); // not sure if needed
        freeTournamentKey(tournament_key);
        return CHESS_TOURNAMENT_ENDED;
    }

    // check if the game is alredy in ths chess system
    GameKey game_key = gameKeyCreate(first_player, second_player);
    if (mapContains(tournament_data->games_map, game_key))
    {
        freeTournamentData(tournament_data); // not sure if needed
        freeTournamentKey(tournament_key);
        freeGameKey(game_key);
        return CHESS_GAME_ALREADY_EXISTS;        
    }

    // check "play_time" input
    if(play_time <= 0)
    {
        freeTournamentData(tournament_data); // not sure if needed
        freeTournamentKey(tournament_key);
        freeGameKey(game_key);
        return CHESS_INVALID_PLAY_TIME;         
    }

    // create the data for the game we want to insert
    GameData game_data = gameDataCreate(winner, play_time);

    // insert the game to the tournament's games_map and the system's games_map
    mapPut(chess->games_map, game_key, game_data);
    mapPut(tournament_data->games_map, game_key, game_data);

    // create players new elements for the cases of which they are not in the system/tournament
    PlayerKey first_player_key = playerKeyCreate(first_player);
    PlayerData first_player_data = playerDataCreate();
    PlayerKey second_player_key = playerKeyCreate(second_player);
    PlayerData second_player_data = playerDataCreate();


    // if the players are not in the system, insert them to both maps
    if (!mapContains(chess->players_map, first_player_key))
    {
        mapPut(chess->players_map, first_player_key, first_player_data);
        mapPut(tournament_data->players_map, first_player_key, first_player_data);
    }
        
    if (!mapContains(chess->players_map, second_player_key))
    {
        mapPut(chess->players_map, second_player_key, second_player_data);
        mapPut(tournament_data, second_player_key, second_player_data);
    }

    // if the players are in the system but not in the tournament, insert only to the tournament map
    if (mapContains(chess->players_map, first_player_key) && !mapContains(tournament_data->players_map, first_player_key))
    {
        mapPut(tournament_data->players_map, first_player_key, first_player_data);
    }

    if (mapContains(chess->players_map, second_player_key) && !mapContains(tournament_data->players_map, second_player_key))
    {
        mapPut(tournament_data->players_map, second_player_key, second_player_data);
    }

    // at this point, both players are in the system&tournament for sure
    // update their data



       

    


    return CHESS_SUCCESS;
}

int main()
{
    return 0;
}