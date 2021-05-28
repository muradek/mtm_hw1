#ifndef PLAYER_H_
#define PLAYER_H_

// the data on each player in the players map 
typedef struct player_data_t
{
    int games_length;
    int games_count;
    int num_wins;
    int num_losses;
    int num_draws;
    int level;
}*PlayerData;

// the key of player for the players map
typedef struct player_key_t
{
    int player_id;
    bool deleted;
}*PlayerKey;

PlayerData playerDataCreate();
PlayerKey playerKeyCreate(int player_id);

PlayerData copyPlayerData(PlayerData old_player);
PlayerKey copyPlayerKey(PlayerKey old_player);
void freePlayerData(PlayerData data_to_free);
void freePlayerKey(PlayerKey key_to_free);
int comparePlayerKey(PlayerKey key_1, PlayerKey key_2);



#endif //PLAYER_H_