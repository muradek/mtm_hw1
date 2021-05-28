#ifndef GAME_H_
#define GAME_H_

typedef struct game_data_t *GameData;
typedef struct game_key_t *GameKey;
GameData gameDataCreate(Winner winner, int game_length);
GameKey gameKeyCreate(int first_id, int secind_id);

GameData copyGameData(GameData old_game);
GameKey copyGameKey(GameKey old_game);
void freeGameData(GameData data_to_free);
void freeGameKey(GameKey key_to_free);
int compareGameKey(GameKey key_1, GameKey key_2);

#endif /* GAME_H_ */