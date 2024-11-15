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

    for (unsigned int i=1; i<strlen(location); i++)
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
// returns CHESS_EXCEEDED_GAMES or CHESS_SUCCESS
ChessResult insertPlayersToMaps(int first_player, int second_player, int play_time, TournamentKey tournament_key,
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

    bool created_first_player_tournament_data = false;
    bool created_first_player_chess_data = false;
    bool created_second_player_tournament_data = false;
    bool created_second_player_chess_data = false;

    // if the players are in the system, get the data.
    // else, initialize them
    if (mapContains(chess->players_map, first_player_key))
    {
        first_player_chess_data = (PlayerData)mapGet(chess->players_map, first_player_key);
    }
    else
    {
        first_player_chess_data = playerDataCreate();
        created_first_player_chess_data = true;
    }

    if (mapContains(chess->players_map, second_player_key))
    {
        second_player_chess_data = (PlayerData)mapGet(chess->players_map, second_player_key);
    }
    else
    {
        second_player_chess_data = playerDataCreate();
        created_second_player_chess_data = true;
    }

    // if the players are in the tournament, get the data
    // else, initialize them and add them to players_count
    if (mapContains(tournament_data->players_map, first_player_key))
    {
        first_player_tournament_data = (PlayerData)mapGet(tournament_data->players_map, first_player_key);
    }
    else
    {
        first_player_tournament_data = playerDataCreate();
        created_first_player_tournament_data = true;
        tournament_data->players_count++;
    }

    if (mapContains(tournament_data->players_map, second_player_key))
    {
        second_player_tournament_data = (PlayerData)mapGet(tournament_data->players_map, second_player_key);
    }
    else{
        second_player_tournament_data = playerDataCreate();
        created_second_player_tournament_data = true;
        tournament_data->players_count++;
    }

    // check if player's played more than allowed
    if(first_player_tournament_data->games_count == tournament_data->max_games_per_player ||
       second_player_tournament_data->games_count == tournament_data->max_games_per_player)
       {
           return CHESS_EXCEEDED_GAMES;
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
    mapPut(chess->tournaments_map, tournament_key, tournament_data); 

    freePlayerKey(first_player_key);
    freePlayerKey(second_player_key);


    if (created_first_player_tournament_data)
    {
        freePlayerData(first_player_tournament_data);
    }
    if (created_first_player_chess_data)
    {
        freePlayerData(first_player_chess_data);
    }
    if (created_second_player_tournament_data)
    {
        freePlayerData(second_player_tournament_data);
    }
    if (created_second_player_chess_data)
    {
        freePlayerData(second_player_chess_data);
    }
    
    return CHESS_SUCCESS;
}

// function that removes a player from a tournament and updates it's oponent info 
void tournamentRemovePlayer(ChessSystem chess ,TournamentData tournament_data, int player_id)
{
    // iterate tournament's games_map
    GameKey game_key = (GameKey)mapGetFirst(tournament_data->games_map);
    while (game_key != NULL)
    {
        GameData game_data = (GameData)mapGet(tournament_data->games_map, game_key);
        if (game_key->first_player_id == player_id)
        {
            game_key->player_deleted = true; // why isnt it updating?
            PlayerKey sec_player_key = playerKeyCreate(game_key->second_player_id);
            PlayerData sec_player_data = (PlayerData)mapGet(tournament_data->players_map, sec_player_key);
            PlayerData sec_player_chess_data = (PlayerData)mapGet(chess->players_map, sec_player_key);
            if (game_data->winner == FIRST_PLAYER)
            {
                sec_player_data->num_losses -= 1;
                sec_player_data->num_wins +=1;
                sec_player_chess_data->num_losses -=1;
                sec_player_chess_data->num_wins +=1;
            }
            if (game_data->winner == DRAW)
            {
                sec_player_data->num_draws -= 1;
                sec_player_data->num_wins += 1;
                sec_player_chess_data->num_draws -= 1;
                sec_player_chess_data->num_wins += 1;
            }
            freePlayerKey(sec_player_key);
        }
        if (game_key->second_player_id == player_id)
        {
            game_key->player_deleted = true; // why isnt it updating?
            PlayerKey first_player_key = playerKeyCreate(game_key->first_player_id);
            PlayerData first_player_data = (PlayerData)mapGet(tournament_data->players_map, first_player_key);
            PlayerData first_player_chess_data = (PlayerData)mapGet(chess->players_map, first_player_key);
            if (game_data->winner == SECOND_PLAYER)
            {
                first_player_data->num_losses -= 1;
                first_player_data->num_wins +=1;
                first_player_chess_data->num_losses -= 1;
                first_player_chess_data->num_wins +=1;

            }
            if (game_data->winner == DRAW)
            {
                first_player_data->num_draws -= 1;
                first_player_data->num_wins += 1;
                first_player_chess_data->num_draws -= 1;
                first_player_chess_data->num_wins += 1;
            }
            freePlayerKey(first_player_key);           
        }
        GameKey tmp_to_remove = game_key;
        game_key = (GameKey)mapGetNext(tournament_data->games_map);
        freeGameKey(tmp_to_remove);
    }
}   

// function that set the score of each player in a tournament
void setPlayersScore(TournamentData tournament_data)
{
    //iterate turnament's players map
    PlayerKey player_key = (PlayerKey)mapGetFirst(tournament_data->players_map);
    while (player_key != NULL)
    {
        PlayerData player_data = (PlayerData)mapGet(tournament_data->players_map, player_key);
        player_data->score= ( 2*(player_data->num_wins) + 1*(player_data->num_draws) );
        PlayerKey tmp_to_remove = player_key;
        player_key = (PlayerKey)mapGetNext(tournament_data->players_map);
        freePlayerKey(tmp_to_remove);
    }
}

// function that finds and returns tournament's winner
int getTournamentWinner(TournamentData tournament_data)
{
    
    PlayerKey curr_winner_key = (PlayerKey)mapGetFirst(tournament_data->players_map); //cant be NULL
    PlayerKey next_player_key = (PlayerKey)mapGetNext(tournament_data->players_map);
    while (next_player_key != NULL)
    {
        PlayerData curr_winner_data = (PlayerData)mapGet(tournament_data->players_map, curr_winner_key);
        PlayerData next_player_data = (PlayerData)mapGet(tournament_data->players_map, next_player_key);
        if (next_player_data->score > curr_winner_data->score)
        {
            curr_winner_key = next_player_key; 
        }
        else if ((next_player_data->score == curr_winner_data->score) && (next_player_data->num_losses < curr_winner_data->num_losses))
        {
            curr_winner_key = next_player_key;
        }
        else if ((next_player_data->score == curr_winner_data->score) && (next_player_data->num_losses == curr_winner_data->num_losses)
            && (next_player_data->num_wins > curr_winner_data->num_wins))
        {
            curr_winner_key = next_player_key;
        }
        else if ( (next_player_data->score == curr_winner_data->score) && (next_player_data->num_losses == curr_winner_data->num_losses)
            && (next_player_data->num_wins == curr_winner_data->num_wins) && (next_player_key->player_id < curr_winner_key->player_id))
        {
            curr_winner_key = next_player_key;
        }
        next_player_key = (PlayerKey)mapGetNext(tournament_data->players_map);
    }

    int winner = curr_winner_key->player_id;
    return winner;
}

// function that sets the level of each player in the system
void setPlayersLevel(ChessSystem chess)
{
    PlayerKey curr_player_key = (PlayerKey)mapGetFirst(chess->players_map);
    while(curr_player_key != NULL)
    {
        PlayerData curr_data = (PlayerData)mapGet(chess->players_map, curr_player_key);
        if (curr_data->games_count == 0)
        {
            curr_data->level = (double)0;
        }
        else
        {
            curr_data->level = (double)(curr_data->num_wins*6 + curr_data->num_draws*2 - curr_data->num_losses*10)/(double)(curr_data->games_count);
        }
        curr_player_key = mapGetNext(chess->players_map);
    }
}

// function that insert players key to array
void insertPlayersToArray(ChessSystem chess, PlayerKey* keys_array)
{
    PlayerKey curr_player_key = (PlayerKey)mapGetFirst(chess->players_map);
    int i = 0; // index for current array cell
    while (curr_player_key != NULL)
    {
        keys_array[i] = curr_player_key;
        curr_player_key = (PlayerKey)mapGetNext(chess->players_map);
        i++;
    }
}

// function that sort keys array by level and then ID
void sortArray(Map players_map, PlayerKey* keys_array, int players_map_size)
{
    for (int i=0; i < players_map_size; i++)
    {
        for (int j=i+1; j<players_map_size; j++)
        {
            PlayerData curr_player_data = (PlayerData)mapGet(players_map, keys_array[i]);
            PlayerData next_player_data = (PlayerData)mapGet(players_map, keys_array[j]);
            if(curr_player_data->level < next_player_data->level)
            {
                PlayerKey tmp_key = keys_array[i];
                keys_array[i] = keys_array[j];
                keys_array[j] = tmp_key;
            }
        }
    }

}

// returns tournament's longest game time
int getTournamentLongestGame(TournamentData tournament_data)
{
    GameKey game_key = (GameKey)mapGetFirst(tournament_data->games_map);
    int longest_time = 0;
    while (game_key != NULL)
    {
        GameData game_data = (GameData)mapGet(tournament_data->games_map, game_key);
        if (game_data->game_length > longest_time)
        {
            longest_time = game_data->game_length;
        }
        game_key = (GameKey)mapGetNext(tournament_data->games_map);
    }
    return longest_time;
}

// returns tournament's avarage games time
double getTournamenrAvaragetime(TournamentData tournament_data)
{
    GameKey game_key = (GameKey)mapGetFirst(tournament_data->games_map);
    double total_time = 0;
    while (game_key != NULL)   
    {
        GameData game_data = (GameData)mapGet(tournament_data->games_map, game_key);
        total_time = total_time + game_data->game_length;
        game_key = (GameKey)mapGetNext(tournament_data->games_map);
    } 
    return (double)((double)total_time / (double)mapGetSize(tournament_data->games_map));
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
    if(chess == NULL || tournament_location == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

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
    if(chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

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
    TournamentData tournament_data = copyTournamentData(mapGet(chess->tournaments_map, tournament_key));
    if (tournament_data->ended)
    {
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
    if(play_time < 0)
    {
        freeTournamentData(tournament_data); // not sure if needed
        freeTournamentKey(tournament_key);
        freeGameKey(game_key);
        return CHESS_INVALID_PLAY_TIME;         
    }

    // insert the player's updated data to the maps
    if(insertPlayersToMaps(first_player, second_player, play_time, tournament_key, tournament_data, winner, chess)== CHESS_EXCEEDED_GAMES)
    {
        freeTournamentData(tournament_data); // not sure if needed
        freeTournamentKey(tournament_key);
        freeGameKey(game_key);
        return CHESS_EXCEEDED_GAMES;
    }

    // create the data for the game we want to insert
    GameData game_data = gameDataCreate(winner, play_time);

    // create another key&data for the tournament's games_map
    GameKey game_key_tour = gameKeyCreate(first_player, second_player); // not sure if needed
    GameData game_data_tour = gameDataCreate(winner, play_time); // not sure if needed

    // insert the game to the tournament's games_map and the system's games_map
    mapPut(chess->games_map, game_key, game_data);
    mapPut(tournament_data->games_map, game_key_tour, game_data_tour);
    mapPut(chess->tournaments_map, tournament_key, tournament_data); //updating the map with the new data

    // free allocated structures?

    return CHESS_SUCCESS;
}

ChessResult chessRemoveTournament (ChessSystem chess, int tournament_id)
{
    // check input correction
    if(chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

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
    
    // if the player only played in this tournament, remove him
    player_key = (PlayerKey)mapGetFirst(tournament_data->players_map);
    int players_count = mapGetSize(tournament_data->players_map);
    int curr_player = 0;
    while(curr_player < players_count)
    {
        PlayerData player_data = (PlayerData)mapGet(chess->players_map, player_key);
        if(player_data->games_count == 0)
        {
            mapRemove(chess->players_map, player_key);
        }

        player_key = (PlayerKey)mapGetNext(tournament_data->players_map);
        curr_player+=1;
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
    // check input correction
    if(chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    if (player_id <= 0)
    {
        return CHESS_INVALID_ID;
    }
    
    PlayerKey player_key = playerKeyCreate(player_id);

    if (!mapContains(chess->players_map, player_key))
    {
        freePlayerKey(player_key);
        return CHESS_PLAYER_NOT_EXIST;
    }

    // iterate system's tournaments to remove the player from all
    TournamentKey tournament_key = (TournamentKey)mapGetFirst(chess->tournaments_map);
    while (tournament_key != NULL)
    {
        TournamentData tournament_data = (TournamentData)mapGet(chess->tournaments_map, tournament_key);
        if (!(tournament_data->ended))
        {
            tournamentRemovePlayer(chess, tournament_data, player_id);
            mapRemove(tournament_data->players_map, player_key);
        }
        tournament_key = (TournamentKey)mapGetNext(chess->tournaments_map);
    }
    mapRemove(chess->players_map, player_key);
    freeTournamentKey(tournament_key); // is this neccessery?
    tournament_key= NULL;
    freePlayerKey(player_key);
    player_key = NULL;
    return CHESS_SUCCESS;
}

ChessResult chessEndTournament (ChessSystem chess, int tournament_id)
{
    // check input correction
    if(chess == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    if(tournament_id <= 0)
    {
        return CHESS_INVALID_ID;
    }

    TournamentKey tournament_to_end_key = tournamentKeyCreate(tournament_id);
    if (!mapContains(chess->tournaments_map, tournament_to_end_key))
    {   
        freeTournamentKey(tournament_to_end_key);
        return CHESS_TOURNAMENT_NOT_EXIST;
    }

    TournamentData tournament_to_end_data = (TournamentData)mapGet(chess->tournaments_map, tournament_to_end_key);
    if (tournament_to_end_data->ended)
    {
        freeTournamentKey(tournament_to_end_key);
        return CHESS_TOURNAMENT_ENDED;
    }

    if(mapGetSize(tournament_to_end_data->games_map) == 0)
    {
        freeTournamentKey(tournament_to_end_key);
        return CHESS_NO_GAMES;
    }

    tournament_to_end_data->ended = true;

    // set tournament's players score
    setPlayersScore(tournament_to_end_data);

    // sort the players by level, losses, wins, id 
    int winner = getTournamentWinner(tournament_to_end_data);

    // set the tournament's winner
    tournament_to_end_data->winner = winner;

    return CHESS_SUCCESS;
}

double chessCalculateAveragePlayTime (ChessSystem chess, int player_id, ChessResult* chess_result)
{
    // check input correction
    if(chess_result == NULL)
    {
        return 0;
    }

    if(chess == NULL)
    {
        *chess_result = CHESS_NULL_ARGUMENT;
        return 0;
    }

    if (player_id <= 0)
    {
        *chess_result = CHESS_INVALID_ID;
        return 0;
    }

    PlayerKey player_key = playerKeyCreate(player_id);
    if (!mapContains(chess->players_map, player_key))
    {
        freePlayerKey(player_key);
        *chess_result = CHESS_PLAYER_NOT_EXIST;
        return 0;
    }

    double average_play_time;
    PlayerData player_data = (PlayerData)mapGet(chess->players_map, player_key);
    if (player_data->games_count == 0)
    {
        average_play_time = 0;
    }
    else
    {
        average_play_time = ((double)(player_data->games_length))/((double)(player_data->games_count));
    }
    
    *chess_result = CHESS_SUCCESS;
    return average_play_time;
}

ChessResult chessSavePlayersLevels(ChessSystem chess, FILE* file)
{
    // check input correction
    if(chess == NULL || file == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    // set each player's level
    setPlayersLevel(chess);

    // create arrays for player's key&data
    int players_map_size = mapGetSize(chess->players_map);
    PlayerKey* keys_array = malloc((unsigned long)players_map_size * sizeof(*keys_array)); // allocating the array
    if (keys_array==NULL)
    {
        return CHESS_SAVE_FAILURE;
    }

    // insert system's player's keys to array
    insertPlayersToArray(chess, keys_array);

    // sort the array by level&id
    sortArray(chess->players_map, keys_array, players_map_size);

    // print sorted array to the file
    for (int i=0; i<players_map_size; i++)
    {
        PlayerKey player_key = keys_array[i];
        int player_id = player_key->player_id;
        PlayerData player_data = (PlayerData)mapGet(chess->players_map, player_key);
        double player_level = player_data->level;
        fprintf(file, "%d %.2f\n", player_id, player_level);
    }
    return CHESS_SUCCESS;
}

ChessResult chessSaveTournamentStatistics (ChessSystem chess, char* path_file)
{
    // check input correction
    if(chess == NULL || path_file == NULL)
    {
        return CHESS_NULL_ARGUMENT;
    }

    bool no_tournaments_ended = true;
    TournamentKey tournament_key= (TournamentKey)mapGetFirst(chess->tournaments_map);
    while(tournament_key!=NULL)
    {
        TournamentData tournament_data = (TournamentData)mapGet(chess->tournaments_map, tournament_key);
        if(tournament_data->ended == true)
        {
            no_tournaments_ended = false;
            break;
        }
        tournament_key = (TournamentKey)mapGetNext(chess->tournaments_map);
    }

    if(no_tournaments_ended)
    {
        // free key?
        // free data?
        return CHESS_NO_TOURNAMENTS_ENDED;
    }

    // open the file to write mode
    FILE* file = fopen(path_file, "w"); // should i check that it worked?
    tournament_key= (TournamentKey)mapGetFirst(chess->tournaments_map);
    while (tournament_key != NULL)
    {
        TournamentData tournament_data = (TournamentData)mapGet(chess->tournaments_map, tournament_key);
        if(tournament_data->ended)
        {
            fprintf(file, "%d\n", tournament_data->winner);
            fprintf(file, "%d\n", getTournamentLongestGame(tournament_data));
            fprintf(file, "%.2f\n", getTournamenrAvaragetime(tournament_data));
            fprintf(file, "%s\n", tournament_data->location);
            fprintf(file, "%d\n", mapGetSize(tournament_data->games_map));
            fprintf(file, "%d\n", tournament_data->players_count);
        }
        tournament_key = mapGetNext(chess->tournaments_map);
    }

    // close the file we opend
    fclose(file);

    return CHESS_SUCCESS;
}

