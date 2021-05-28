#ifndef TOURNAMENT_H_
#define TOURNAMENT_H_

typedef struct tournament_data_t *TournamentData;
typedef struct tournament_key_t *TournamentKey;
TournamentData tournamentDataCreate(const char* location, int winner, int max_games_per_player);
TournamentKey tournamentKeyCreate(int ID);

TournamentData copyTournamentData(TournamentData old_data);
TournamentKey copyTournamentKey(TournamentKey old_key);
void freeTournamentData(TournamentData data_to_free);
void freeTournamentKey(TournamentKey key_to_free);
int compareTournamentKey(TournamentKey key_1, TournamentKey key_2);

#endif /* TOURNAMENT_H_ */