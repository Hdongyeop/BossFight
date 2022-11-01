// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnumList.h"
#include "EnemyBase.generated.h"

UCLASS()
class BOSSFIGHT_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bGameStarted = false;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStatComponent* StatComponent;

	UPROPERTY()
	class USwordAnim* AnimInstance;

	UPROPERTY()
	class AAIController* AIController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTimerHandle RecoverBp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FTimerHandle RecoverStun;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWidgetComponent* StunText;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

public:
	// Sets default values for this character's properties
	AEnemyBase();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void TakeDamage(EAttackType AttackType, int Damage, AActor* Causer);
	
	void TakeBlockDamage(AActor* Attacker, int32 Damage);

	void SetRecoverBpTimer(bool flag);
};
