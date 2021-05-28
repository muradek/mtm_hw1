#ifndef PLAYER_H_
#define PLAYER_H_

typedef struct player_data_t *PlayerData;
typedef struct player_key_t *PlayerKey;
PlayerData playerDataCreate();
PlayerKey playerKeyCreate(int player_id);

PlayerData copyPlayerData(PlayerData old_player);
PlayerKey copyPlayerKey(PlayerKey old_player);
void freePlayerData(PlayerData data_to_free);
void freePlayerKey(PlayerKey key_to_free);
int comparePlayerKey(PlayerKey key_1, PlayerKey key_2);



#endif //PLAYER_H_