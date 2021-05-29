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

// function that check the Location input
// returns true if its valid
// returns false if it invalid 
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

// function that inserts players into players_map of chess&tournament
void insertPlayersToMaps(int first_player, int second_player, int play_time, TournamentKey tournament_key,
     TournamentData tournament_data, Winner winner, ChessSystem chess)
{
    // create players new key elements to find them in the maps
    PlayerKey first_player_key = playerKeyCreate(first_player);
    PlayerKey second_player_key = playerKeyCreate(second_player);

    // create players new data elements to insert/update
    PlayerData first_player_tournament_data;
    PlayerData first_player_chess_data;
    PlayerData second_player_tournament_data;
    PlayerData second_player_chess_data;

    // if the players are in the system, get the data.
    // else, initialize them
    if (mapContains(chess->players_map, first_player_key))
    {
        first_player_chess_data = (PlayerData)mapGet(chess->players_map, first_player_key);
    }
    else
    {
        first_player_chess_data = playerDataCreate();
    }

    if (mapContains(chess->players_map, second_player_key))
    {
        second_player_chess_data = (PlayerData)mapGet(chess->players_map, second_player_key);
    }
    else
    {
        second_player_chess_data = playerDataCreate();
    }

    // if the players are in the tournament, get the data
    // else, initialize them
    if (mapContains(tournament_data->players_map, first_player_key))
    {
        first_player_tournament_data = (PlayerData)mapGet(tournament_data->players_map, first_player_key);
    }
    else
    {
        first_player_tournament_data = playerDataCreate();
    }

    if (mapContains(tournament_data->players_map, second_player_key))
    {
        second_player_tournament_data = (PlayerData)mapGet(tournament_data->players_map, second_player_key);
    }
    else{
        second_player_tournament_data = playerDataCreate();
    }

    // update the data of the players in the tournament&chess
    first_player_chess_data->games_count+=1;
    first_player_chess_data->games_length+= play_time;
    first_player_tournament_data->games_count+=1;
    first_player_tournament_data->games_length+= play_time;

    second_player_chess_data->games_count+=1;
    second_player_chess_data->games_length+= play_time;
    second_player_tournament_data->games_count+=1;
    second_player_tournament_data->games_length+= play_time;    

    if (winner == FIRST_PLAYER)
    {
        first_player_chess_data->num_wins+=1;
        first_player_tournament_data->num_wins+=1;
        second_player_chess_data->num_losses+=1;
        second_player_tournament_data->num_losses+=1;
    }
    if (winner == SECOND_PLAYER)
    {
        first_player_chess_data->num_losses+=1;
        first_player_tournament_data->num_losses+=1;
        second_player_chess_data->num_wins+=1;
        second_player_tournament_data->num_wins+=1;
    }
    if (winner == DRAW)
    {
        first_player_chess_data->num_draws+=1;
        first_player_tournament_data->num_draws+=1;
        second_player_chess_data->num_draws+=1;
        second_player_tournament_data->num_draws+=1;
    }

    // insert the new data to the maps
    mapPut(chess->players_map, first_player_key, first_player_chess_data);
    mapPut(chess->players_map, second_player_key, second_player_chess_data);
    mapPut(tournament_data->players_map, first_player_key, first_player_tournament_data);
    mapPut(tournament_data->players_map, second_player_key, second_player_tournament_data);  

    // should i free created structures?
    // if i do, maybe a problem with the if-else conditions
}


// function that removes a player from a tournament and updates it's oponent info 
void tournamentRemovePlayer(TournamentKey tournament_key, TournamentData tournament_data, int player_id)
{
    // iterate tournament's games_map
    GameKey game_key = (GameKey)mapGetFirst(tournament_data->games_map);
    while (game_key != NULL)
    {
        GameData game_data = (GameData)mapGet(tournament_data->games_map, game_key);
        if (game_key->first_player_id == player_id)
        {
            PlayerKey sec_player_key = playerKeyCreate(game_key->second_player_id);
            PlayerData sec_player_data = (PlayerData)mapGet(tournament_data->games_map, sec_player_key);
            // game_data->player_is_deleted = true;

            if (game_data->winner == FIRST_PLAYER)
            {
                sec_player_data->num_losses -= 1;
                sec_player_data->num_wins +=1;
            }
            if (game_data->winner == DRAW)
            {
                sec_player_data->num_draws -= 1;
                sec_player_data->num_wins += 1;
            }
            freePlayerKey(sec_player_key);
            freePlayerData(sec_player_data);
            
        }
        if (game_key->second_player_id == player_id)
        {
            PlayerKey first_player_key = playerKeyCreate(game_key->first_player_id);
            PlayerData first_player_data = (PlayerData)mapGet(tournament_data->games_map, first_player_key);
            // game_data->player_is_deleted = true;

            if (game_data->winner == SECOND_PLAYER)
            {
                first_player_data->num_losses -= 1;
                first_player_data->num_wins +=1;
            }
            if (game_data->winner == DRAW)
            {
                first_player_data->num_draws -= 1;
                first_player_data->num_wins += 1;
            }
            freePlayerKey(first_player_key);
            freePlayerData(first_player_data);            
        }
        game_key = (GameKey)mapGetNext(tournament_data->games_map);
    }
    freeGameKey(game_key); // is thie enough? or shoul i use every iteration?
}    

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

    mapDestroy(chess->tournaments_map);
    mapDestroy(chess->games_map);
    mapDestroy(chess->players_map);
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
    // check input correction
    if (tournament_id <= 0 || first_player <= 0 || second_player <= 0 || first_player == second_player)
    {
        return CHESS_INVALID_ID;
    }

    // check if the tournament is in the chess system
    TournamentKey tournament_key = tournamentKeyCreate(tournament_id);
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

    // check if the game is already in ths chess system
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

    // create another key&data for the tournament's games_map
    GameKey game_key_tour = gameKeyCreate(first_player, second_player); // not sure if needed
    GameData game_data_tour = gameDataCreate(winner, play_time); // not sure if needed

    // insert the game to the tournament's games_map and the system's games_map
    mapPut(chess->games_map, game_key, game_data);
    mapPut(tournament_data->games_map, game_key_tour, game_data_tour);

    // insert the player's updated data to the maps
    insertPlayersToMaps(first_player, second_player, play_time, tournament_key, tournament_data, winner, chess);

    // free allocated structures?

    return CHESS_SUCCESS;
}

ChessResult chessRemoveTournament (ChessSystem chess, int tournament_id)
{
    if (tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    
    TournamentKey tournament_key = tournamentKeyCreate(tournament_id);
    if(!mapContains(chess->tournaments_map, tournament_key))
    {
        freeTournamentKey(tournament_key);
        return CHESS_TOURNAMENT_NOT_EXIST;
    }

    // updating players info 
    // iterate on the tournament's players_map
    TournamentData tournament_data = (TournamentData)mapGet(chess->tournaments_map, tournament_key);
    PlayerKey player_key = (PlayerKey)mapGetFirst(tournament_data->players_map);
    while(player_key != NULL)
    {
        PlayerData player_tournament_data = (PlayerData)mapGet(tournament_data->players_map ,player_key);
        PlayerData player_system_data = (PlayerData)mapGet(chess->players_map, player_key);
        player_system_data->games_count -= player_tournament_data->games_count;
        player_system_data->games_length -= player_tournament_data->games_length;
        player_system_data->num_draws -= player_tournament_data->num_draws;
        player_system_data->num_wins -= player_tournament_data->num_wins;
        player_system_data->num_losses -= player_tournament_data->num_losses;
        // should I use mapPut to insert the changes?
        player_key = (PlayerKey)mapGetNext(tournament_data->players_map);
        // should i free player key? player data?
    }
    
    // deleting games from systems games_map
    // iterate on the tournament's games_map
    GameKey game_key = (GameKey)mapGetFirst(tournament_data->games_map);
    while (game_key != NULL)
    {
        mapRemove(chess->games_map, game_key);
        game_key = (GameKey)mapGetNext(tournament_data->games_map);
    }

    mapRemove(chess->tournaments_map, tournament_key);
    freeTournamentKey(tournament_key);
    return CHESS_SUCCESS;
}

ChessResult chessRemovePlayer(ChessSystem chess, int player_id)
{
    // checking input
    if (player_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    
    if (!mapContains(chess->players_map, player_id))
    {
        return CHESS_PLAYER_NOT_EXIST;
    }

    PlayerKey player_key = playerKeyCreate(player_id);
    // iterate system's tournaments to remove the player from all
    TournamentKey tournament_key = (TournamentKey)mapGetFirst(chess->tournaments_map);
    while (tournament_key != NULL)
    {
        TournamentData tournament_data = (TournamentData)mapGet(chess->tournaments_map, tournament_key);
        if (!(tournament_data->ended))
        {
            tournamentRemovePlayer(tournament_key, tournament_data, player_id);
            mapRemove(tournament_data->players_map, player_key);
        }
        tournament_key = (TournamentKey)mapGetNext(chess->tournaments_map);
    }
    mapRemove(chess->players_map, player_key);
    freeTournamentKey(tournament_key); // is this neccessery?
    freePlayerKey(player_key);
    return CHESS_SUCCESS;
}

int main()
{
    ChessSystem cs = chessCreate();

    printf("%d\n", chessAddTournament(cs, 2, 2, "Eilat"));
    //printf("%d\n", chessAddTournament(cs, 2, 2, "Eilat"));
    //printf("%d\n", chessAddTournament(cs, 1, 2, "Eilat"));
    printf("%d\n", chessAddGame(cs, 2, 1, 2, DRAW, 15));
    printf("%d\n", chessAddGame(cs, 2, 1, 2, DRAW, 15));

    // chessDestroy(cs);  
    return 0;
}