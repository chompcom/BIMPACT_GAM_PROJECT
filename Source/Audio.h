#pragma once
#include "AEEngine.h"
#include <string>
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
bool IsbuttonAudioState();
void GameWinAudio();
void BigPageFlip();

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
void CampfireAudio();
void OvenAudio();
void StopMobAudio();
void PlayMobSound(std::string const& soundName);
void UpdateMobAudioCD(float dt);

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
void ResetBGM();
void pauseBGM();
void ResumeBGM();

// Menu
void InitMenuAudio();
void MenuBGMAudio();
void FreeMenuAudio();


void PauseAllAudio();
void ResumeAllAudio();