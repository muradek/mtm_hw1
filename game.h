#ifndef GAME_H_
#define GAME_H_

// the data each game contains
typedef struct game_data_t
{
    Winner winner;
    int game_length;
} *GameData;

// the key to compare different games in a map
typedef struct game_key_t
{
    int first_player_id;
    int second_player_id;
}*GameKey;

GameData gameDataCreate(Winner winner, int game_length);
GameKey gameKeyCreate(int first_id, int second_id);

GameData copyGameData(GameData old_game);
GameKey copyGameKey(GameKey old_game);
void freeGameData(GameData data_to_free);
void freeGameKey(GameKey key_to_free);
int compareGameKey(GameKey key_1, GameKey key_2);

#endif /* GAME_H_ */