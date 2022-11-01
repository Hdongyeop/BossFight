// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnumList.h"
#include "GameFramework/Character.h"
#include "PlayableCharacter.generated.h"

UCLASS()
class BOSSFIGHT_API APlayableCharacter : public ACharacter
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bGameStarted = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsRunning = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float NormalSpeed = 400.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningSpeed = 600.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float NormalCameraLength = 400.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningCameraLength = 600.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bBlockable = false;

public:
	// Sets default values for this character's properties
	APlayableCharacter();

	UPROPERTY()
	class UManager* Manager;

	UPROPERTY()
	class APlayerController* MyController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* Sword;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* SwordCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UTargeting* TargetSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UStatComponent* StatComponent;

	UPROPERTY()
	class USwordAnim* AnimInstance;

	UPROPERTY(VisibleAnywhere, Category = TargetSystem)
	AActor* Target;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	bool GetBlockable() { return bBlockable; }
	void SetBlockable(bool flag) { bBlockable = flag; }

	void TurnRight(float Value);
	void LookUp(float Value);
	void MoveForward(float Value);
	void MoveRight(float Value);

	void EnterRun();
	void ExitRun();

	void Attack();
	void Block();
	void Roll();
	void Lock();
	UFUNCTION(BlueprintCallable)
	void TakeDamage(EAttackType AttackType, int Damage, AActor* Causer);

	UFUNCTION()
	void MontageEnd();
	UFUNCTION()
	void OnLockedOn(AActor* TargetActor);
	UFUNCTION()
	void OnLockedOff(AActor* TargetActor);
	UFUNCTION()
	void AttackHit();
};
