// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SwordAnim.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMotageEnd);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackHit);

/**
 * 
 */
UCLASS()
class BOSSFIGHT_API USwordAnim : public UAnimInstance
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Speed = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float Rotation = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 AttackComboIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bComboAble = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bDeath = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class APlayableCharacter* OwningCharacter;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector2D MoveDirection = FVector2D(0.f, 0.f);

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FMotageEnd OnMontageEnd;

	UPROPERTY(BlueprintAssignable, VisibleAnywhere, BlueprintCallable, Category = "Event")
	FAttackHit OnAttackHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsLocked = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = PlayerAnim)
	bool IsMontagePlaying = false;

	UPROPERTY(VisibleAnywhere, Category = PlayerAnim)
	UAnimMontage* AttackAnim;

	UPROPERTY(VisibleAnywhere, Category = PlayerAnim)
	UAnimMontage* BlockAnim;

	UPROPERTY(VisibleAnywhere, Category = PlayerAnim)
	TArray<UAnimMontage*> RollAnim;

	UPROPERTY(VisibleAnywhere, Category = PlayerAnim)
	UAnimMontage* DeathAnim;

public:
	USwordAnim();

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	float GetSpeed() const { return Speed; }
	int32 GetAttackComboIndex() const { return AttackComboIndex; }

	void Attack();
	void Block();
	void Roll();
	void Death();

	UFUNCTION()
	void AnimNotify_MontageEnd();

	UFUNCTION()
	void AnimNotify_AttackHit();

	UFUNCTION()
	void AnimNotify_ComboAble();

	UFUNCTION()
	void AnimNotify_ComboDisable();

	UFUNCTION()
	void AnimNotify_Dodgeable();

	UFUNCTION()
	void AnimNotify_DodgeableEnd();

	UFUNCTION()
	void AnimNotify_Blockable();

	UFUNCTION()
	void AnimNotify_BlockableEnd();
};
