#ifndef TOURNAMENT_H_
#define TOURNAMENT_H_


// the data each tournament contains
typedef struct tournament_data_t
{
    const char* location;
    int winner;
    int max_games_per_player;
    Map games_map;
    Map players_map;
    bool ended;
    int players_count;    
}*TournamentData;

// the key to compare different tournaments in a map
typedef struct tournament_key_t
{
    int key;
} *TournamentKey;

TournamentData tournamentDataCreate(const char* location, int winner, int max_games_per_player);
TournamentKey tournamentKeyCreate(int ID);

TournamentData copyTournamentData(TournamentData old_data);
TournamentKey copyTournamentKey(TournamentKey old_key);
void freeTournamentData(TournamentData data_to_free);
void freeTournamentKey(TournamentKey key_to_free);
int compareTournamentKey(TournamentKey key_1, TournamentKey key_2);

#endif /* TOURNAMENT_H_ */