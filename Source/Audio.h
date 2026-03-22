#include "AEEngine.h"
void InitAudio();
void FreeAudio();

// Player
void PlayerDmgAudio();
void PlayerFootstepAudio();
void PlayerPickUpAudio();
void PlayerThrowAudio();

// UI
void ButtonAudio();
void AlmanacAudio();

// Mob
void HydroAttackAudio();
void FriendSuccessAudio();
void RoarAudio();
void SwordAttackAudio();
void TomWhateverAudio();
void BoogerStickAudio();
void ProjectileAudio();
void FireballAudio();
void ChargingThrowAudio(float dt);
void StopChargingAudio();
void StopMobAudio();
void RoundingProjectileAudio();


// Room
void RoomEnterAudio();


void StopAllAudio(); // except music lol
