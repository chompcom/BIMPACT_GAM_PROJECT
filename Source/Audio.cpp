#include "Audio.h"
#include "Loaders/DataLoader.h"

// Player
AEAudioGroup PlayerAudio;
AEAudio DmgTaken;
AEAudio Footstep_1;
AEAudio Footstep_2;
AEAudio Footstep_3;
AEAudio PickUp;
AEAudio Throw;

// Charging
AEAudioGroup ChargingAudio;
AEAudio ChargingThrow;
AEAudio ChargingThrowMax;

// UI
AEAudioGroup UIAudio;
AEAudio ButtonSound;
AEAudio NewAlmanacEntry;
AEAudio AlmanacSound;
AEAudio HoverSound;
AEAudio LoseSound;

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
AEAudio CrabAttack;
AEAudio Goblin;
AEAudio Poprocks;
AEAudio Sponge;
AEAudio Melee;

// Room
AEAudioGroup RoomAudio;
AEAudio DoorSound;
AEAudio ForestAudio;
AEAudio IceAudio;
AEAudio FireAudio;
AEAudio OceanAudio;


// BGM
AEAudioGroup BGMAudio;
AEAudio BGM1;
AEAudio BGM2;
AEAudio BGM3;
AEAudio BossBGM;
AEAudio FightMusic;

// Menu
AEAudioGroup MenuAudio;
AEAudio MenuBGM;

static float chargingTimer = 0.0f;
static bool maxPlaying = false;
static bool chargingStarted = false;

void InitAudio() {

	// Player Group
	PlayerAudio = AEAudioCreateGroup();
	DmgTaken = DataLoader::GetSound("DmgTaken");
	Footstep_1 = DataLoader::GetSound("Footstep_1");
	Footstep_2 = DataLoader::GetSound("Footstep_2");
	Footstep_3 = DataLoader::GetSound("Footstep_3");
	PickUp = DataLoader::GetSound("PickUp");
	Throw = DataLoader::GetSound("Throw");

	// Charging Group
	ChargingAudio = AEAudioCreateGroup();
	ChargingThrow = DataLoader::GetSound("ChargingThrow");
	ChargingThrowMax = DataLoader::GetSound("ChargingThrowMax");

	// UI Group
	UIAudio = AEAudioCreateGroup();
	ButtonSound = DataLoader::GetSound("ButtonSound");
	NewAlmanacEntry = DataLoader::GetSound("NewAlmanacEntry");
	AlmanacSound = DataLoader::GetSound("AlmanacSound");
	HoverSound = DataLoader::GetSound("HoverSound");
	LoseSound = DataLoader::GetSound("LoseSound");

	// Mob Group
	MobAudio = AEAudioCreateGroup();
	HydroAttackSound = DataLoader::GetSound("HydroAttackSound");
	FriendSuccess = DataLoader::GetSound("FriendSuccess");
	Roar = DataLoader::GetSound("Roar");
	SwordAttackSound = DataLoader::GetSound("SwordAttackSound");
	Tom_Whatever = DataLoader::GetSound("Tom_Whatever");
	BoogerStick = DataLoader::GetSound("BoogerStick");
	ProjectileSound = DataLoader::GetSound("ProjectileSound");
	FireballSound = DataLoader::GetSound("FireballSound");
	RoundingProjectileSound = DataLoader::GetSound("RoundingProjectileSound");
	CrabAttack = DataLoader::GetSound("CrabAttack");
	Goblin = DataLoader::GetSound("Goblin");
	Poprocks = DataLoader::GetSound("Poprocks");
	Sponge = DataLoader::GetSound("Sponge");
	Melee = DataLoader::GetSound("Melee");

	// Room Group
	RoomAudio = AEAudioCreateGroup();
	DoorSound = DataLoader::GetSound("DoorSound");
	ForestAudio = DataLoader::GetSound("ForestAudio");
	IceAudio = DataLoader::GetSound("IceAudio");
	FireAudio = DataLoader::GetSound("FireAudio");
	OceanAudio = DataLoader::GetSound("OceanAudio");

	//  BGM
	BGMAudio = AEAudioCreateGroup();
	BGM1 = DataLoader::GetSound("BGM1");
	BGM2 = DataLoader::GetSound("BGM2");
	BGM3 = DataLoader::GetSound("BGM3");
	BossBGM = DataLoader::GetSound("BossBGM");	
	FightMusic = DataLoader::GetSound("FightMusic");

	// Menu Group
	MenuAudio = AEAudioCreateGroup();
	MenuBGM = DataLoader::GetSound("MenuBGM");
}

void FreeAudio() { // Use before dataloader::unload pls
	AEAudioUnloadAudioGroup(PlayerAudio);
	AEAudioUnloadAudioGroup(ChargingAudio);
	AEAudioUnloadAudioGroup(UIAudio);
	AEAudioUnloadAudioGroup(MobAudio);
	AEAudioUnloadAudioGroup(RoomAudio);
	AEAudioUnloadAudioGroup(MenuAudio);
	AEAudioUnloadAudioGroup(BGMAudio);
}

// Player
void PlayerDmgAudio() {
	AEAudioPlay(DmgTaken, PlayerAudio, 1.0f, 1.0f, 0);
}

void PlayerFootstepAudio() {
	int step = rand() % 3;
	if (step == 0)      AEAudioPlay(Footstep_1, PlayerAudio, 0.4f, 1.0f, 0);
	else if (step == 1) AEAudioPlay(Footstep_2, PlayerAudio, 0.4f, 1.0f, 0);
	else                AEAudioPlay(Footstep_3, PlayerAudio, 0.4f, 1.0f, 0);
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

void StopChargingAudio() {
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
	AEAudioPlay(NewAlmanacEntry, UIAudio, 1.0f, 1.0f, 0);
}

void HoverAudio() {
	AEAudioPlay(HoverSound, UIAudio, 1.0f, 1.0f, 0);
}

void GameLoseAudio() {
	AEAudioPlay(LoseSound, UIAudio, 1.0f, 1.0f, 0);
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

void CrabAttackAudio() {
	AEAudioPlay(CrabAttack, MobAudio, 1.0f, 1.0f, 0);
}

void GoblinAudio() {
	AEAudioPlay(Goblin, MobAudio, 1.0f, 1.0f, 0);
}

void PoprocksAudio() {
	AEAudioPlay(Poprocks, MobAudio, 1.0f, 1.0f, 0);
}

void SpongeAudio() {
	AEAudioPlay(Sponge, MobAudio, 1.0f, 1.0f, 0);
}

void MeleeAudio() {
	AEAudioPlay(Melee, MobAudio, 1.0f, 1.0f, 0);
}

void StopMobAudio() {
	AEAudioStopGroup(MobAudio);
}

// Room
void RoomEnterAudio() {
	AEAudioPlay(DoorSound, RoomAudio, 0.5f, 2.0f, 0);
}

void ForestBiomeAudio() {
	AEAudioPlay(ForestAudio, RoomAudio, 0.5f, 2.0f, -1);
}

void IceBiomeAudio() {
	AEAudioPlay(IceAudio, RoomAudio, 0.5f, 2.0f, -1);
}

void FireBiomeAudio() {
	AEAudioPlay(FireAudio, RoomAudio, 0.5f, 2.0f, -1);
}

void OceanBiomeAudio() {
	AEAudioPlay(OceanAudio, RoomAudio, 0.5f, 2.0f, -1);
}

void BGM1Audio() {
	AEAudioPlay(BGM1, BGMAudio, 0.5f, 1.0f, 0);
}

void BGM2Audio() {
	AEAudioPlay(BGM2, BGMAudio, 0.5f, 1.0f, 0);
}

void BGM3Audio() {
	AEAudioPlay(BGM3, BGMAudio, 0.5f, 1.0f, 0);
}

void BossBGMAudio() {
	AEAudioPlay(BossBGM, BGMAudio, 0.5f, 1.0f, -1);
}

void FightMusicAudio() {
	AEAudioPlay(FightMusic, BGMAudio, 0.5f, 1.0f, -1);
}

static float bgmTimer = 0.0f;
static float bgmInterval = 0.0f;

void RandomBGMAudio(float dt) {
	bgmTimer += dt;
	if (bgmTimer >= bgmInterval) {
		AEAudioStopGroup(BGMAudio);

		int pick = rand() % 3;
		if (pick == 0)      AEAudioPlay(BGM1, BGMAudio, 0.5f, 1.0f, 0);
		else if (pick == 1) AEAudioPlay(BGM2, BGMAudio, 0.5f, 1.0f, 0);
		else                AEAudioPlay(BGM3, BGMAudio, 0.5f, 1.0f, 0);

		bgmTimer = 0.0f;
		bgmInterval = 10.0f + (rand() % 6);
	}
}

// Menu
void MenuBGMAudio() {
	AEAudioPlay(MenuBGM, MenuAudio, 0.5f, 1.0f, -1);
}

void StopAllAudio() {
	AEAudioStopGroup(PlayerAudio);
	AEAudioStopGroup(ChargingAudio);
	AEAudioStopGroup(UIAudio);
	AEAudioStopGroup(MobAudio);
	AEAudioStopGroup(RoomAudio);
	AEAudioStopGroup(MenuAudio);

	chargingTimer = 0.0f;
	maxPlaying = false;
	chargingStarted = false;
}