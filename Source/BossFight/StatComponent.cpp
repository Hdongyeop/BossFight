// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"
#include "BossFightGameModeBase.h"
#include "MainUI.h"
#include "Manager.h"
#include "Kismet/GameplayStatics.h"
#include "PlayableCharacter.h"
#include "Components/ProgressBar.h"

UStatComponent::UStatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStatComponent::SetHp(int32 NewHp)
{
	Hp = NewHp;
	OnChangeHp.Broadcast(NewHp);
}

void UStatComponent::SetBp(int32 NewBp)
{
	Bp = NewBp;
	OnChangeBp.Broadcast(NewBp);
}

void UStatComponent::InitStat(EClassType ClassType)
{
	auto Manager = Cast<UManager>(UGameplayStatics::GetGameInstance(GetWorld()));
	
	if (ClassType == EClassType::Player)
	{
		// Player
		auto PlayerStat = Manager->GetPlayerStat();
		if(PlayerStat == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't find PlayerStat"));
		}
		Hp = PlayerStat->MaxHp;
		MaxHp = PlayerStat->MaxHp;
		Bp = PlayerStat->MaxBp;
		MaxBp = PlayerStat->MaxBp;
		Damage = PlayerStat->BaseDamage;
		BlockDamage = PlayerStat->BaseBlockDamage;
	}
	else
	{
		// Boss
		auto BossStat = Manager->GetBossStat(1);
		if (BossStat == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Can't find BossStat"));
		}
		Hp = BossStat->MaxHp;
		MaxHp = BossStat->MaxHp;
		Bp = BossStat->MaxBp;
		MaxBp = BossStat->MaxBp;
		Damage = BossStat->BaseDamage;
		BlockDamage = BossStat->BaseBlockDamage;
	}
}


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();
}
