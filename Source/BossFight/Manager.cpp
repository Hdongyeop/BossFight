// Fill out your copyright notice in the Description page of Project Settings.


#include "Manager.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

UManager::UManager()
{
	// ~ Data
	static ConstructorHelpers::FObjectFinder<UDataTable> DT_Player(TEXT("DataTable'/Game/Data/PlayerStat_DT.PlayerStat_DT'"));
	if (DT_Player.Succeeded())
		PlayerStat = DT_Player.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> DT_Boss(TEXT("DataTable'/Game/Data/EnemyStat_DT.EnemyStat_DT'"));
	if (DT_Boss.Succeeded())
		BossStat = DT_Boss.Object;

	static ConstructorHelpers::FObjectFinder<UDataTable> DT_Attack(TEXT("DataTable'/Game/Data/Attack_DT.Attack_DT'"));
	if (DT_Attack.Succeeded())
		AttackList = DT_Attack.Object;

	// ~ Sound
	static ConstructorHelpers::FObjectFinder<USoundBase> SB_Block(TEXT("SoundCue'/Game/Sounds/Block_Cue.Block_Cue'"));
	if (SB_Block.Succeeded())
		BlockSound = SB_Block.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> SB_Attack_0(TEXT("SoundCue'/Game/Sounds/Attack_0_Cue.Attack_0_Cue'"));
	if (SB_Attack_0.Succeeded())
		AttackSound.Add(SB_Attack_0.Object);

	static ConstructorHelpers::FObjectFinder<USoundBase> SB_Attack_1(TEXT("SoundCue'/Game/Sounds/Attack_1_Cue.Attack_1_Cue'"));
	if (SB_Attack_1.Succeeded())
		AttackSound.Add(SB_Attack_1.Object);

	static ConstructorHelpers::FObjectFinder<USoundBase> SB_Attack_2(TEXT("SoundCue'/Game/Sounds/Attack_2_Cue.Attack_2_Cue'"));
	if (SB_Attack_2.Succeeded())
		AttackSound.Add(SB_Attack_2.Object);
}

void UManager::Init()
{
	Super::Init();
	UE_LOG(LogTemp, Warning, TEXT("GameInstance Init"));
}

FPlayerData* UManager::GetPlayerStat(FString State)
{
	return PlayerStat->FindRow<FPlayerData>(FName(State), TEXT(""));
}

FEnemyData* UManager::GetBossStat(int Phase)
{
	return BossStat->FindRow<FEnemyData>(FName(*FString::FromInt(Phase)), TEXT(""));
}

FAttackData* UManager::GetAttackData(FString AttackName)
{
	return AttackList->FindRow<FAttackData>(FName(AttackName), TEXT(""));
}

void UManager::PlaySound(UObject* object, USoundBase* Sound)
{
	UGameplayStatics::PlaySound2D(object, Sound);
}
