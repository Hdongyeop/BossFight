// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnumList.h"
#include "StatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeHp, int32, NewHp);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeBp, int32, NewBp);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOSSFIGHT_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 Hp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 MaxHp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 Bp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 MaxBp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 Damage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	int32 BlockDamage;

public:
	UPROPERTY()
	class UProgressBar* HpBar;

	UPROPERTY()
	class UProgressBar* BpBar;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Sets default values for this component's properties
	UStatComponent();

	int32 GetHp() const { return Hp; }
	int32 GetMaxHp() const { return MaxHp; }
	int32 GetBp() const { return Bp; }
	int32 GetMaxBp() const { return MaxBp; }
	int32 GetDamage() const { return Damage; }
	int32 GetBlockDamage() const { return BlockDamage; }

	UPROPERTY(VisibleAnywhere, BlueprintAssignable)
	FChangeHp OnChangeHp;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable)
	FChangeBp OnChangeBp;

	UFUNCTION(BlueprintCallable)
	void SetHp(int32 NewHp);

	UFUNCTION(BlueprintCallable)
	void SetBp(int32 NewBp);

	UFUNCTION(BlueprintCallable)
	void InitStat(EClassType ClassType);
};
