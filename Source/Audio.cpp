#include "Audio.h"
// Player
AEAudioGroup PlayerAudio;
AEAudio DmgTaken;
AEAudio Footstep_1;
AEAudio Footstep_2;
AEAudio Footstep_3;
AEAudio PickUp;
AEAudio Throw;

AEAudioGroup ChargingAudio;
AEAudio ChargingThrow;
AEAudio ChargingThrowMax;

// UI
AEAudioGroup UIAudio;
AEAudio ButtonSound;
AEAudio AlmanacSound;

// Mob
AEAudioGroup MobAudio;
AEAudio HydroAttackSound;
AEAudio FriendSuccess;
AEAudio Roar;
AEAudio SwordAttackSound;
AEAudio Tom_Whatever;
AEAudio BoogerStick;
AEAudio ProjectileSound;
AEAudio FireballSound;
AEAudio RoundingProjectileSound;


// Room
AEAudioGroup RoomAudio;
AEAudio DoorSound;


static float chargingTimer = 0.0f;
static bool maxPlaying = false;
static bool chargingStarted = false;

void InitAudio() {
	// Player Group
	PlayerAudio = AEAudioCreateGroup();
	DmgTaken = AEAudioLoadSound("Assets/Audio/Player/DamageTaken.wav");
	Footstep_1 = AEAudioLoadSound("Assets/Audio/Player/Footstep_1.wav");
	Footstep_2 = AEAudioLoadSound("Assets/Audio/Player/Footstep_2.wav");
	Footstep_3 = AEAudioLoadSound("Assets/Audio/Player/Footstep_3.wav");
	PickUp = AEAudioLoadSound("Assets/Audio/Player/PickUp.wav");
	Throw = AEAudioLoadSound("Assets/Audio/Player/Throw.wav");

	// Throw charge Group
	ChargingAudio = AEAudioCreateGroup();
	ChargingThrow = AEAudioLoadSound("Assets/Audio/Player/ChargingThrow.wav");
	ChargingThrowMax = AEAudioLoadSound("Assets/Audio/Player/ChargingMax.wav");

	// UI Group
	 UIAudio = AEAudioCreateGroup();
	 ButtonSound = AEAudioLoadSound("Assets/Audio/UI/Button.wav");
	 AlmanacSound = AEAudioLoadSound("Assets/Audio/UI/NewEntry.wav");

	// Mob Group
	 MobAudio = AEAudioCreateGroup();
	 HydroAttackSound = AEAudioLoadSound("Assets/Audio/Mobs/Hydro_Attack.wav");
	 FriendSuccess = AEAudioLoadSound("Assets/Audio/Mobs/Friend.wav");
	 Roar = AEAudioLoadSound("Assets/Audio/Mobs/Roar.wav");
	 SwordAttackSound = AEAudioLoadSound("Assets/Audio/Mobs/Sword_Attack.wav");
	 Tom_Whatever = AEAudioLoadSound("Assets/Audio/Mobs/Tom_Whatever.wav");
	 BoogerStick = AEAudioLoadSound("Assets/Audio/Mobs/BoogerStick.wav");
	 ProjectileSound = AEAudioLoadSound("Assets/Audio/Mobs/Projectile.wav");
	 FireballSound = AEAudioLoadSound("Assets/Audio/Mobs/Fireball.wav");
	 RoundingProjectileSound = AEAudioLoadSound("Assets/Audio/Mobs/RoundingProjectile.wav");
	 


	 // Room Group
	 RoomAudio = AEAudioCreateGroup();
	 DoorSound = AEAudioLoadSound("Assets/Audio/Room/Door.wav");
}

void FreeAudio() {
	AEAudioUnloadAudioGroup(PlayerAudio);
	AEAudioUnloadAudioGroup(UIAudio);
	AEAudioUnloadAudioGroup(MobAudio);
	AEAudioUnloadAudioGroup(RoomAudio);
	AEAudioUnloadAudioGroup(ChargingAudio); 

}

// Player
void PlayerDmgAudio() {
	AEAudioPlay(DmgTaken, PlayerAudio, 1.0f, 1.0f, 0);
}
void PlayerFootstepAudio() {
	int step = rand() % 3;
	if (step == 0) AEAudioPlay(Footstep_1, PlayerAudio, 1.0f, 1.0f, 0);
	else if (step == 1) AEAudioPlay(Footstep_2, PlayerAudio, 1.0f, 1.0f, 0);
	else AEAudioPlay(Footstep_3, PlayerAudio, 1.0f, 1.0f, 0);
}

void PlayerPickUpAudio() {
	AEAudioPlay(PickUp, PlayerAudio, 1.0f, 1.0f, 0);
}

void PlayerThrowAudio() {
	AEAudioPlay(Throw, PlayerAudio, 1.0f, 1.0f, 0);
}

void ChargingThrowAudio(float dt) {
if (!chargingStarted) {
		AEAudioPlay(ChargingThrow, ChargingAudio, 1.0f, 1.0f, 0); 
		chargingStarted = true;
	}

if (!maxPlaying) {
		chargingTimer += dt;
		if (chargingTimer >= 1.0f) {
			AEAudioStopGroup(ChargingAudio);
			AEAudioPlay(ChargingThrowMax, ChargingAudio, 1.0f, 1.0f, -1);
			maxPlaying = true;
		}
	}
}
void StopChargingAudio(){
	chargingTimer = 0.0f;  
	maxPlaying = false;
	chargingStarted = false;
	AEAudioStopGroup(ChargingAudio);
}


// UI
void ButtonAudio() {
	AEAudioPlay(ButtonSound, UIAudio, 1.0f, 1.0f, 0);
}
void AlmanacAudio() {
	AEAudioPlay(AlmanacSound, UIAudio, 1.0f, 1.0f, 0);
}

// Mob
void HydroAttackAudio() {
	AEAudioPlay(HydroAttackSound, MobAudio, 1.0f, 1.0f, 0);
}
void FriendSuccessAudio() {
	AEAudioPlay(FriendSuccess, MobAudio, 1.0f, 1.0f, 0);
}
void RoarAudio() { 
	AEAudioPlay(Roar, MobAudio, 1.0f, 1.0f, 0);
}
void SwordAttackAudio() {
    AEAudioPlay(SwordAttackSound, MobAudio, 1.0f, 1.0f, 0);
}

void TomWhateverAudio() {
	  AEAudioPlay(Tom_Whatever, MobAudio, 1.0f, 1.0f, 0);
}

void BoogerStickAudio() {
	AEAudioPlay(BoogerStick, MobAudio, 1.0f, 1.0f, 0);
}

void ProjectileAudio() {
	AEAudioPlay(ProjectileSound, MobAudio, 1.0f, 1.0f, 0);
}

void FireballAudio() {
	AEAudioPlay(FireballSound, MobAudio, 1.0f, 1.0f, 0);
}

void RoundingProjectileAudio() {
	AEAudioPlay(RoundingProjectileSound, MobAudio, 1.0f, 1.0f, 0);
}



void StopMobAudio() {
	AEAudioStopGroup(MobAudio);
}
// Room
void RoomEnterAudio() {
	AEAudioPlay(DoorSound, RoomAudio, 0.5f, 2.0f, 0);
}

void StopAllAudio() {
	AEAudioStopGroup(PlayerAudio);
	AEAudioStopGroup(ChargingAudio);
	AEAudioStopGroup(UIAudio);
	AEAudioStopGroup(MobAudio);
	AEAudioStopGroup(RoomAudio);

	chargingTimer = 0.0f;
	maxPlaying = false;
	chargingStarted = false;
}