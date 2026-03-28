#pragma once
#include "AEEngine.h"

void InitAudio();
void FreeAudio();
void StopAllAudio();

// Player
void PlayerDmgAudio();
void PlayerFootstepAudio();
void PlayerPickUpAudio();
void PlayerThrowAudio();
void ChargingThrowAudio(float dt);
void StopChargingAudio();

// UI
void ButtonAudio();
void AlmanacAudio();
void HoverAudio();
void GameLoseAudio();

// Mob
void HydroAttackAudio();
void FriendSuccessAudio();
void RoarAudio();
void SwordAttackAudio();
void TomWhateverAudio();
void BoogerStickAudio();
void ProjectileAudio();
void FireballAudio();
void RoundingProjectileAudio();
void CrabAttackAudio();
void GoblinAudio();
void PoprocksAudio();
void SpongeAudio();
void MeleeAudio();
void StopMobAudio();

// Room
void RoomEnterAudio();
void ForestBiomeAudio();
void IceBiomeAudio();
void FireBiomeAudio();
void OceanBiomeAudio();

// BGM
void RandomBGMAudio(float dt);
void BGM1Audio();
void BGM2Audio();
void BGM3Audio();
void BossBGMAudio();
void FightMusicAudio();

// Menu
void MenuBGMAudio();