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
    Map tournament_map;
    Map game_map;
};

ChessSystem chessCreate()
{
    ChessSystem chess_system=malloc(sizeof(*chess_system));
    if (!chess_system)
    {
        return NULL;
    }

    chess_system->game_map = mapCreate((copyMapDataElements) &copyGameData, (copyMapKeyElements) &copyGameKey, 
        (freeMapDataElements) &freeGameData, (freeMapKeyElements) &freeGameKey, (compareMapKeyElements) &compareGameKey);
    
    chess_system->tournament_map= mapCreate((copyMapDataElements) &copyTournamentData, (copyMapKeyElements) &copyTournamentKey, 
        (freeMapDataElements) &freeTournamentData, (freeMapKeyElements) &freeTournamentKey, (compareMapKeyElements) &compareTournamentKey);
    
    return chess_system;
}

void chessDestroy(ChessSystem chess) //some seg fault
{
    if (!chess)
    {
        return;
    }
    mapDestroy(chess->game_map);
    mapDestroy(chess->tournament_map);
    free(chess);
}

ChessResult chessAddTournament(ChessSystem chess, int tournament_id, int max_games_per_player, const char* tournament_location)
{
    if (tournament_id<0)
    {
        return CHESS_INVALID_ID;
    }

    TournamentKey tournament_key= tournamentKeyCreate(tournament_id);

    if (mapContains(chess->tournament_map, tournament_key))
    {
        freeTournamentKey(tournament_key);
        return CHESS_TOURNAMENT_ALREADY_EXISTS;
    }

    if(!(checkLocation(tournament_location)))
    {
        freeTournamentKey(tournament_key);
        return CHESS_INVALID_LOCATION;
    }

    if(max_games_per_player<=0)
    {
        freeTournamentKey(tournament_key);
        return CHESS_INVALID_MAX_GAMES;
    }

    Map game_map = mapCreate((copyMapDataElements) &copyGameData, (copyMapKeyElements) &copyGameKey, 
        (freeMapDataElements) &freeGameData, (freeMapKeyElements) &freeGameKey, (compareMapKeyElements) &compareGameKey);

    TournamentData tournament_data= tournamentDataCreate(tournament_location, -1, max_games_per_player, game_map);

    mapPut(chess->tournament_map, tournament_key, tournament_data);

    return CHESS_SUCCESS;
}

/*
int main(){

ChessSystem chess= chessCreate();

//chessAddTournament(chess, 7, 10, "Eilat");
//chessAddTournament(chess, 8, 20, "Hilat");
//chessAddTournament(chess, 6, 30, "Hilat");
chessDestroy(chess);
//printf("%d\n", mapGetSize(chess->tournament_map));
return 0;

}
*/

/*

ChessResult chessAddGame(ChessSystem chess, int tournament_id, int first_player, int second_player, Winner winner, int play_time)
{
    if (tournament_id<=0 || first_player<=0 || second_player<=0)
    {
        return CHESS_INVALID_ID;
    }

    TournamentKey tour_key= tournamentKeyCreate(tournament_id);
    if(!mapContains(chess->tournament_map, tournament_id))
    {
        freeTournamentKey(tour_key);
        return CHESS_TOURNAMENT_NOT_EXIST;
    }

    //tour ended??
    //if(mapContains())
}
*/