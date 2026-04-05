/* Start Header ************************************************************************/
/*!
\file        Audio.cpp
\author     Brandon Choo, b.choo, 2501888
\par        b.choo@digipen.edu
\brief		This is the audio code where for the Player, Mobs, UIs, Room, Background Music and Menu + Charging of gifts

Copyright (C) 2026 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents
without the prior written consent of DigiPen Institute of
Technology is prohibited.
*/
/* End Header **************************************************************************/
#include "Audio.h"
#include "Loaders/DataLoader.h"

// Player grp 
AEAudioGroup playerAudio;
AEAudio dmgTaken;
AEAudio footstep1;
AEAudio footstep2;
AEAudio footstep3;
AEAudio pickUp;
AEAudio throwSound; // throwing of gifts

// Charging grp for the charge up of throwing the gifts
AEAudioGroup chargingAudio;
AEAudio chargingThrow;
AEAudio chargingThrowMax;

// UI grp
AEAudioGroup uiAudio;
AEAudio buttonSound;
AEAudio newAlmanacEntry;
AEAudio almanacSound;
AEAudio hoverSound;
AEAudio loseSound;
AEAudio winSound;
AEAudio bigPageFlipAlmanac;

// Mob grp
AEAudioGroup mobAudio;
AEAudio hydroAttackSound;
AEAudio friendSuccess;
AEAudio roar;
AEAudio swordAttackSound;
AEAudio tom;
AEAudio boogerStick;
AEAudio projectileSound;
AEAudio fireballSound;
AEAudio roundingProjectileSound;
AEAudio crabAttack;
AEAudio goblin;
AEAudio poprocks;
AEAudio sponge;
AEAudio melee;
AEAudio campfire;
AEAudio oven;

// Room grp
AEAudioGroup roomAudio;
AEAudio doorSound;
AEAudio forestAudio;
AEAudio iceAudio;
AEAudio fireAudio;
AEAudio oceanAudio;


// BGM grp
AEAudioGroup bgmAudio;
AEAudio bgm1;
AEAudio bgm2;
AEAudio bgm3;
AEAudio bossBgm;
AEAudio fightMusic;

// Menu grp
AEAudioGroup menuAudio;
AEAudio menuBgm;

static float chargingTimer = 0.0f; 
static bool maxPlaying = false;  // when charging is at its max
static bool chargingStarted = false; 
static float mobSoundCooldown = 0.0f; // so mobs sound dont get spammed when they interact with the gifts
static bool buttonAudioState = false;

void InitAudio() {
	buttonAudioState = true;
	// Player grp
	playerAudio = AEAudioCreateGroup();
	// GetSound is used in Josiah's dataloader is being used to get the path for the sound
	dmgTaken = DataLoader::GetSound("DmgTaken");
	footstep1 = DataLoader::GetSound("Footstep_1");
	footstep2 = DataLoader::GetSound("Footstep_2");
	footstep3 = DataLoader::GetSound("Footstep_3");
	pickUp = DataLoader::GetSound("PickUp");
	throwSound = DataLoader::GetSound("Throw");

	// Charging grp
	chargingAudio = AEAudioCreateGroup();
	chargingThrow = DataLoader::GetSound("ChargingThrow");
	chargingThrowMax = DataLoader::GetSound("ChargingThrowMax");

	// UI grp
	uiAudio = AEAudioCreateGroup();
	buttonSound = DataLoader::GetSound("ButtonSound");
	newAlmanacEntry = DataLoader::GetSound("NewAlmanacEntry");
	almanacSound = DataLoader::GetSound("AlmanacSound");
	hoverSound = DataLoader::GetSound("HoverSound");
	loseSound = DataLoader::GetSound("LoseSound");
	winSound = DataLoader::GetSound("WinSound");
	bigPageFlipAlmanac = DataLoader::GetSound("BigPageFlip");


	// Mob grp
	mobAudio = AEAudioCreateGroup();
	hydroAttackSound = DataLoader::GetSound("HydroAttackSound");
	friendSuccess = DataLoader::GetSound("FriendSuccess");
	roar = DataLoader::GetSound("Roar");
	swordAttackSound = DataLoader::GetSound("SwordAttackSound");
	tom = DataLoader::GetSound("Tom");
	boogerStick = DataLoader::GetSound("BoogerStick");
	projectileSound = DataLoader::GetSound("ProjectileSound");
	fireballSound = DataLoader::GetSound("FireballSound");
	roundingProjectileSound = DataLoader::GetSound("RoundingProjectileSound");
	crabAttack = DataLoader::GetSound("CrabAttack");
	goblin = DataLoader::GetSound("Goblin");
	poprocks = DataLoader::GetSound("Poprocks");
	sponge = DataLoader::GetSound("Sponge");
	melee = DataLoader::GetSound("Melee");
	campfire = DataLoader::GetSound("Campfire");
	oven = DataLoader::GetSound("Oven");

	// Room grp
	roomAudio = AEAudioCreateGroup();
	doorSound = DataLoader::GetSound("DoorSound");
	forestAudio = DataLoader::GetSound("ForestAudio");
	iceAudio = DataLoader::GetSound("IceAudio");
	fireAudio = DataLoader::GetSound("FireAudio");
	oceanAudio = DataLoader::GetSound("OceanAudio");

	//  BGM
	bgmAudio = AEAudioCreateGroup();
	bgm1 = DataLoader::GetSound("BGM1");
	bgm2 = DataLoader::GetSound("BGM2");
	bgm3 = DataLoader::GetSound("BGM3");
	bossBgm = DataLoader::GetSound("BossBGM");	
	fightMusic = DataLoader::GetSound("FightMusic");

}

void FreeAudio() { // Use before dataloader::unload pls
	AEAudioUnloadAudioGroup(playerAudio);
	AEAudioUnloadAudioGroup(chargingAudio);
	AEAudioUnloadAudioGroup(uiAudio);
	AEAudioUnloadAudioGroup(mobAudio);
	AEAudioUnloadAudioGroup(roomAudio);
//	AEAudioUnloadAudioGroup(MenuAudio);
	AEAudioUnloadAudioGroup(bgmAudio);
	buttonAudioState = false;
}


//--------------------------------------------------------------------
//         Individual functions to call the specific audio
//--------------------------------------------------------------------


// Player
void PlayerDmgAudio() {
	AEAudioPlay(dmgTaken, playerAudio, 1.0f, 1.0f, 0);
}

void PlayerFootstepAudio() { //  random between 3 different footsteps audio
	int step = rand() % 3;
	if (step == 0)      AEAudioPlay(footstep1, playerAudio, 0.4f, 1.0f, 0);
	else if (step == 1) AEAudioPlay(footstep2, playerAudio, 0.4f, 1.0f, 0);
	else                AEAudioPlay(footstep3, playerAudio, 0.4f, 1.0f, 0);
}

void PlayerPickUpAudio() {
	AEAudioPlay(pickUp, playerAudio, 1.0f, 1.0f, 0);
}

void PlayerThrowAudio() {
	AEAudioPlay(throwSound, playerAudio, 1.0f, 1.0f, 0);
}
  
void ChargingThrowAudio(float dt) { // charging throw sound
	if (!chargingStarted) {
		AEAudioPlay(chargingThrow, chargingAudio, 1.0f, 1.0f, 0);
		chargingStarted = true;
	}

	if (!maxPlaying) {
		chargingTimer += dt;
		if (chargingTimer >= 1.0f) {
			AEAudioStopGroup(chargingAudio);
			AEAudioPlay(chargingThrowMax, chargingAudio, 1.0f, 1.0f, -1);
			maxPlaying = true;
		}
	}
}

void StopChargingAudio() { 
	chargingTimer = 0.0f;
	maxPlaying = false;
	chargingStarted = false;
	AEAudioStopGroup(chargingAudio);
}

// UI
void ButtonAudio() {
	if (!buttonAudioState) return;
	AEAudioPlay(buttonSound, uiAudio, 1.0f, 1.0f, 0);
}

void AlmanacAudio() {
	AEAudioPlay(almanacSound, uiAudio, 1.0f, 1.0f, 0);
}

void HoverAudio() {
	if (!buttonAudioState) return;
	AEAudioPlay(hoverSound, uiAudio, 1.0f, 1.0f, 0);
}

void GameLoseAudio() {
	AEAudioPlay(loseSound, uiAudio, 1.0f, 1.0f, 0);
}

void GameWinAudio() {
	AEAudioPlay(winSound, uiAudio, 1.0f, 1.0f, 0);
}

void BigPageFlip() {
	AEAudioPlay(bigPageFlipAlmanac, uiAudio, 0.5f, 1.0f, 0);
}

bool IsButtonAudioState() {
	return buttonAudioState;
}


// Mob
void HydroAttackAudio() {
	AEAudioPlay(hydroAttackSound, mobAudio, 1.0f, 1.0f, 0);
}

void FriendSuccessAudio() {
	AEAudioPlay(friendSuccess, mobAudio, 1.0f, 1.0f, 0);
}

void RoarAudio() {
	AEAudioPlay(roar, mobAudio, 1.0f, 1.0f, 0);
}

void SwordAttackAudio() {
	AEAudioPlay(swordAttackSound, mobAudio, 1.0f, 1.0f, 0);
}

void TomWhateverAudio() {
	AEAudioPlay(tom, mobAudio, 1.0f, 1.0f, 0);
}

void BoogerStickAudio() {
	AEAudioPlay(boogerStick, mobAudio, 1.0f, 1.0f, 0);
}

void ProjectileAudio() {
	AEAudioPlay(projectileSound, mobAudio, 1.0f, 1.0f, 0);
}

void FireballAudio() {
	AEAudioPlay(fireballSound, mobAudio, 1.0f, 1.0f, 0);
}

void RoundingProjectileAudio() {
	AEAudioPlay(roundingProjectileSound, mobAudio, 1.0f, 1.0f, 0);
}

void CrabAttackAudio() {
	AEAudioPlay(crabAttack, mobAudio, 1.0f, 1.0f, 0);
}

void GoblinAudio() {
	AEAudioPlay(goblin, mobAudio, 1.0f, 1.0f, 0);
}

void PoprocksAudio() {
	AEAudioPlay(poprocks, mobAudio, 1.0f, 1.0f, 0);
}

void SpongeAudio() {
	AEAudioPlay(sponge, mobAudio, 1.0f, 1.0f, 0);
}

void MeleeAudio() {
	AEAudioPlay(melee, mobAudio, 1.0f, 1.0f, 0);
}

void CampfireAudio() {
	AEAudioPlay(campfire, mobAudio, 1.0f, 1.0f, 0);
}
void OvenAudio() {
	AEAudioPlay(oven, mobAudio, 1.0f, 1.0f, 0);
}
void StopMobAudio() {
	AEAudioStopGroup(mobAudio);
}

void PlayMobSound(std::string const& soundName) {
	if (soundName.empty() || mobSoundCooldown > 0.0f)	return;
	AEAudioPlay(DataLoader::GetSound(soundName), mobAudio, 0.5f, 1.0f, 0);
	mobSoundCooldown = 1.0f; // 1 sec cooldown so that sound dont spam lel
}

void UpdateMobAudioCD(float dt) {
	if (mobSoundCooldown > 0.0f) mobSoundCooldown -= dt;
}

// Room
void RoomEnterAudio() {
	AEAudioPlay(doorSound, roomAudio, 0.5f, 2.0f, 0);
}

void ForestBiomeAudio() {
	AEAudioPlay(forestAudio, roomAudio, 0.5f, 2.0f, -1);
}

void IceBiomeAudio() {
	AEAudioPlay(iceAudio, roomAudio, 0.5f, 2.0f, -1);
}

void FireBiomeAudio() {
	AEAudioPlay(fireAudio, roomAudio, 0.5f, 2.0f, -1);
}

void OceanBiomeAudio() {
	AEAudioPlay(oceanAudio, roomAudio, 0.1f, 2.0f, -1);
}

void BGM1Audio() {
	AEAudioPlay(bgm1, bgmAudio, 0.5f, 1.0f, 0);
}

void BGM2Audio() {
	AEAudioPlay(bgm2, bgmAudio, 0.5f, 1.0f, 0);
}

void BGM3Audio() {
	AEAudioPlay(bgm3, bgmAudio, 0.5f, 1.0f, 0);
}

void BossBGMAudio() {
	AEAudioPlay(bossBgm, bgmAudio, 0.5f, 1.0f, -1);
}

void FightMusicAudio() {
	AEAudioPlay(fightMusic, bgmAudio, 0.8f, 1.0f, -1);
}

static float bgmTimer = 0.0f;
static float bgmInterval = 0.0f;

void RandomBGMAudio(float dt) {
	bgmTimer += dt;
	if (bgmTimer >= bgmInterval) {
		AEAudioStopGroup(bgmAudio);

		int pick = rand() % 3;   // every interval random between 3 short bgm sounds will be played 
		if (pick == 0)      AEAudioPlay(bgm1, bgmAudio, 0.2f, 1.0f, 0);
		else if (pick == 1) AEAudioPlay(bgm2, bgmAudio, 0.2f, 1.0f, 0);
		else                AEAudioPlay(bgm3, bgmAudio, 0.2f, 1.0f, 0);

		bgmTimer = 0.0f;
		bgmInterval = 12.0f;
	}
}
void ResetBGM() {
	AEAudioStopGroup(bgmAudio);
}
void PauseBGM() {
	AEAudioPauseGroup(roomAudio);
	AEAudioPauseGroup(bgmAudio);
}
void ResumeBGM() {
	AEAudioResumeGroup(roomAudio);
	AEAudioResumeGroup(bgmAudio);
}

// Menu
void InitMenuAudio() {	
	buttonAudioState = true;
	menuAudio = AEAudioCreateGroup();
	menuBgm = AEAudioLoadSound("Assets/Audio/Menu/MenuBGM.ogg");
	uiAudio = AEAudioCreateGroup();

	// Button sound or else no button sound for main menu for some reason?? data no load? 
	Json::Value audioJson = DataLoader::LoadJsonFile("Assets/audio.json");
	for (Json::Value& val : audioJson["audio"]["ui"]) {
		std::string name = val["name"].asString();
		if (name == "ButtonSound") {
			buttonSound = AEAudioLoadSound(val["path"].asString().c_str());
		}
		else if (name == "HoverSound") {
			hoverSound = AEAudioLoadSound(val["path"].asString().c_str());
		}
	} 

}
void MenuBGMAudio() {
	AEAudioPlay(menuBgm, menuAudio, 0.5f, 1.0f, 0);
}
void FreeMenuAudio() {
	AEAudioStopGroup(menuAudio);
	AEAudioUnloadAudioGroup(menuAudio);

	AEAudioStopGroup(uiAudio);
	AEAudioUnloadAudioGroup(uiAudio);


	buttonAudioState = false;
}

void StopAllAudio() {
	AEAudioStopGroup(playerAudio);
	AEAudioStopGroup(chargingAudio);
	AEAudioStopGroup(uiAudio);
	AEAudioStopGroup(mobAudio);
	AEAudioStopGroup(roomAudio);
	AEAudioStopGroup(bgmAudio);

	chargingTimer = 0.0f;
	maxPlaying = false;
	chargingStarted = false;
}

void PauseAllAudio() {
	AEAudioPauseGroup(playerAudio);
	AEAudioPauseGroup(chargingAudio);
	AEAudioPauseGroup(uiAudio);
	AEAudioPauseGroup(mobAudio);
	AEAudioPauseGroup(roomAudio);
	AEAudioPauseGroup(bgmAudio);
}

void ResumeAllAudio() {
	AEAudioResumeGroup(playerAudio);
	AEAudioResumeGroup(chargingAudio);
	AEAudioResumeGroup(uiAudio);
	AEAudioResumeGroup(mobAudio);
	AEAudioResumeGroup(roomAudio);
	AEAudioResumeGroup(bgmAudio);
}