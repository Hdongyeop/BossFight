// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Engine/GameInstance.h"
#include "EnumList.h"
#include "Engine/DataTable.h"
#include "Manager.generated.h"

USTRUCT()
struct FPlayerData : public FTableRowBase
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxBp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BaseBlockDamage;
};

USTRUCT()
struct FEnemyData : public FTableRowBase
{
	GENERATED_BODY()
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxHp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxBp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BaseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 BaseBlockDamage;
};

USTRUCT()
struct FAttackData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttackType AttackType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Damage;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameStarted);

/**
 * 
 */
UCLASS()
class BOSSFIGHT_API UManager : public UGameInstance
{
	GENERATED_BODY()

private:
	// ~ Data
	UPROPERTY()
	UDataTable* PlayerStat;

	UPROPERTY()
	UDataTable* BossStat;

	UPROPERTY()
	UDataTable* AttackList;

public:
	UPROPERTY()
	USoundBase* BlockSound;

	UPROPERTY()
	TArray<USoundBase*> AttackSound;

	UPROPERTY(VisibleAnywhere, BlueprintCallable, BlueprintAssignable)
	FGameStarted OnGameStarted;

public:
	UManager();

	virtual void Init() override;

	FPlayerData* GetPlayerStat(FString State = TEXT("Default"));

	FEnemyData* GetBossStat(int Phase);

	FAttackData* GetAttackData(FString AttackName);

	void PlaySound(UObject* object, USoundBase* Sound);
};
