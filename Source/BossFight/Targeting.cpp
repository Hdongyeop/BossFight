// Fill out your copyright notice in the Description page of Project Settings.


#include "Targeting.h"
#include "Targetable.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EngineUtils.h"

// Sets default values for this component's properties
UTargeting::UTargeting()
{
	PrimaryComponentTick.bCanEverTick = true;

	TargetableActors = APawn::StaticClass();
	TargetableCollisionChannel = ECC_Pawn;
}

TArray<AActor*> UTargeting::GetAllActorsOfClass(TSubclassOf<AActor> ActorClass) const
{
	TArray<AActor*> Actors;
	for (TActorIterator<AActor> ActorIterator(GetWorld(), ActorClass); ActorIterator; ++ActorIterator)
	{
		AActor* Actor = *ActorIterator;
		const bool bIsTargetable = TargetIsTargetable(Actor);
		if (bIsTargetable)
		{
			Actors.Add(Actor);
		}
	}

	return Actors;

}

TArray<AActor*> UTargeting::FindTargetsInRange(TArray<AActor*> ActorsToLook, float RangeMin, float RangeMax) const
{
	TArray<AActor*> ActorsInRange;

	for (AActor* Actor : ActorsToLook)
	{
		const float Angle = GetAngleUsingCameraRotation(Actor);
		if (Angle > RangeMin && Angle < RangeMax)
			ActorsInRange.Add(Actor);
	}

	return ActorsInRange;
}

AActor* UTargeting::FindNearestTarget(TArray<AActor*> Actors)
{
	TArray<AActor*> ActorHit;

	for (AActor* Actor : Actors)
	{
		const bool bHit = LineTraceForActor(Actor);
		if(bHit && IsInViewport(Actor))
		{
			ActorHit.Add(Actor);
		}
	}

	if (ActorHit.Num() == 0) return nullptr;

	AActor* Target = nullptr;
	for (AActor* HitActor : ActorHit)
	{
		const float Distance = GetDistanceFromCharacter(HitActor);
		if(Distance < ClosestTargetDistance)
		{
			ClosestTargetDistance = Distance;
			Target = HitActor;
		}
	}

	return Target;
}

bool UTargeting::LineTrace(FHitResult& HitResult, const AActor* OtherActor, TArray<AActor*> ActorsToIgnore) const
{
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Init(OwnerActor, 1);
	IgnoredActors += ActorsToIgnore;

	FCollisionQueryParams Params = FCollisionQueryParams(FName("LineTraceSingle"));
	Params.AddIgnoredActors(IgnoredActors);

	return GetWorld()->LineTraceSingleByChannel(
		HitResult,
		OwnerActor->GetActorLocation(),
		OtherActor->GetActorLocation(),
		TargetableCollisionChannel,
		Params
	);
}

bool UTargeting::LineTraceForActor(AActor* OtherActor, TArray<AActor*> ActorsToIgnore) const
{
	FHitResult HitResult;
	const bool bHit = LineTrace(HitResult, OtherActor, ActorsToIgnore);
	if(bHit)
	{
		AActor* HitActor = HitResult.GetActor();
		if (HitActor == OtherActor) return true;
	}

	return false;
}

bool UTargeting::ShouldBreakLineOfSight() const
{
	if (LockedOnTargetActor == nullptr) return true;

	TArray<AActor*> ActorsToIgnore = GetAllActorsOfClass(TargetableActors);
	ActorsToIgnore.Remove(LockedOnTargetActor);


	FHitResult HitResult;
	const bool bHit = LineTrace(HitResult, LockedOnTargetActor, ActorsToIgnore);
	// Line이 Hit한게 벽일 때
	if (bHit && HitResult.GetActor() != LockedOnTargetActor)
		return true;

	return false;

}

void UTargeting::BreakLineOfSight()
{
	bIsBreakingLineOfSight = false;
	if (ShouldBreakLineOfSight())
		TargetLockOff();
}

bool UTargeting::IsInViewport(const AActor* TargetActor) const
{
	if (IsValid(OwnerPlayerController) == false) return true;

	FVector2D ScreenLocation;
	OwnerPlayerController->ProjectWorldLocationToScreen(TargetActor->GetActorLocation(), ScreenLocation);

	FVector2D ViewportSize;
	GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);

	return ScreenLocation.X > 0 && ScreenLocation.Y > 0 && ScreenLocation.X < ViewportSize.X&& ScreenLocation.Y < ViewportSize.Y;
}

float UTargeting::GetDistanceFromCharacter(const AActor* OtherActor) const
{
	return OwnerActor->GetDistanceTo(OtherActor);
}

FRotator UTargeting::GetControlRotationOnTarget(const AActor* OtherActor) const
{
	const FRotator ControlRotation = OwnerPlayerController->GetControlRotation();

	const FVector CharacterLocation = OwnerActor->GetActorLocation();
	const FVector OtherActorLocation = OtherActor->GetActorLocation();

	const FRotator LookRotation = FRotationMatrix::MakeFromX(OtherActorLocation - CharacterLocation).Rotator();
	float Pitch = LookRotation.Pitch;

	FRotator TargetRotation;
	if(bAdjustPitchBasedOnDistanceToTarget)
	{
		const float DistanceToTarget = GetDistanceFromCharacter(OtherActor);
		const float PitchInRange = (DistanceToTarget * PitchDistanceCoefficient + PitchDistanceOffset) * -1.f;
		const float PitchOffset = FMath::Clamp(PitchInRange, PitchMin, PitchMax);

		Pitch = Pitch + PitchOffset;
		TargetRotation = FRotator(Pitch, LookRotation.Yaw, ControlRotation.Roll);
	}
	else
	{
		if (bIgnoreLookInput)
			TargetRotation = FRotator(Pitch, LookRotation.Yaw, ControlRotation.Roll);
		else
			TargetRotation = FRotator(ControlRotation.Pitch, LookRotation.Yaw, ControlRotation.Roll);
	}

	return FMath::RInterpTo(ControlRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), 9.f);
}

void UTargeting::SetControlRotationOnTarget(AActor* TargetActor) const
{
	if (IsValid(OwnerPlayerController) == false) return;

	const FRotator ControlRotation = GetControlRotationOnTarget(TargetActor);

	if (OnTargetSetRotation.IsBound())
		OnTargetSetRotation.Execute(TargetActor, ControlRotation);
	else
		OwnerPlayerController->SetControlRotation(ControlRotation);
}

void UTargeting::ControlRotation(bool ShouldControlRotation) const
{
	if (IsValid(OwnerPawn) == false) return;

	OwnerPawn->bUseControllerRotationYaw = ShouldControlRotation;

	UCharacterMovementComponent* CharacterMovementComponent = OwnerPawn->FindComponentByClass<UCharacterMovementComponent>();
	if(CharacterMovementComponent != nullptr)
	{
		CharacterMovementComponent->bOrientRotationToMovement = !ShouldControlRotation;
	}
}

float UTargeting::GetAngleUsingCameraRotation(const AActor* ActorToLook) const
{
	UCameraComponent* CameraComponent = OwnerActor->FindComponentByClass<UCameraComponent>();
	if (CameraComponent == nullptr)
		return GetAngleUsingCharacterRotation(ActorToLook);

	const FRotator CameraWorldRotation = CameraComponent->GetComponentRotation();
	const FRotator LookAtRotation = FindLookAtRotation(CameraComponent->GetComponentLocation(), ActorToLook->GetActorLocation());

	float YawAngle = CameraWorldRotation.Yaw - LookAtRotation.Yaw;
	if(YawAngle < 0)
		YawAngle += 360.f;

	return YawAngle;
}

float UTargeting::GetAngleUsingCharacterRotation(const AActor* ActorToLook) const
{
	const FRotator CharacterRotation = OwnerActor->GetActorRotation();
	const FRotator LookAtRotation = FindLookAtRotation(OwnerActor->GetActorLocation(), ActorToLook->GetActorLocation());

	float YawAngle = CharacterRotation.Yaw - LookAtRotation.Yaw;
	if (YawAngle < 0)
		YawAngle += 360.f;

	return YawAngle;
}

FRotator UTargeting::FindLookAtRotation(const FVector Start, const FVector Target)
{
	return FRotationMatrix::MakeFromX(Target - Start).Rotator();
}

void UTargeting::TargetLockOn(AActor* TargetToLockOn)
{
	if (TargetToLockOn == nullptr) return;

	// Recast PlayerController in case it wasn't already setup on Begin Play (local split screen)
	SetupLocalPlayerController();

	bTargetLocked = true;

	if (bShouldControlRotation)
		ControlRotation(true);

	if(bAdjustPitchBasedOnDistanceToTarget || bIgnoreLookInput)
	{
		if(IsValid(OwnerPlayerController))
		{
			OwnerPlayerController->SetIgnoreLookInput(true);
		}
	}

	if (OnTargetLockedOn.IsBound())
		OnTargetLockedOn.Execute(TargetToLockOn);
}

void UTargeting::ResetIsSwitchingTarget()
{
	bIsSwitchingTarget = false;
	bDesireToSwitch = false;
}

bool UTargeting::ShouldSwitchTargetActor(float AxisValue) const
{
	return FMath::Abs(AxisValue) > StartRotatingThreshold;
}

bool UTargeting::TargetIsTargetable(const AActor* Actor)
{
	const bool bIsImplemented = Actor->GetClass()->ImplementsInterface(UTargetable::StaticClass());
	if (bIsImplemented)
		return ITargetable::Execute_IsTargetable(Actor);

	return true;
}

void UTargeting::SetupLocalPlayerController()
{
	if (IsValid(OwnerPawn) == false) return;

	OwnerPlayerController = Cast<APlayerController>(OwnerPawn->GetController());
}

// Called when the game starts
void UTargeting::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = GetOwner();
	if (IsValid(OwnerActor) == false) return;

	OwnerPawn = Cast<APawn>(OwnerActor);
	if (IsValid(OwnerPawn) == false) return;

	SetupLocalPlayerController();
}


// Called every frame
void UTargeting::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!bTargetLocked || IsValid(LockedOnTargetActor) == false)
	{
		return;
	}

	if(!TargetIsTargetable(LockedOnTargetActor))
	{
		TargetLockOff();
		return;
	}

	SetControlRotationOnTarget(LockedOnTargetActor);

	// 거리가 멀어지면 록오프
	if (GetDistanceFromCharacter(LockedOnTargetActor) > MinimumDistanceToEnable)
		TargetLockOff();

	if(ShouldBreakLineOfSight() && bIsBreakingLineOfSight == false)
	{
		if(BreakLineOfSightDelay <= 0)
		{
			TargetLockOff();
		}
		else
		{
			bIsBreakingLineOfSight = true;
			GetWorld()->GetTimerManager().SetTimer(
				LineOfSightBreakTimerHandle,
				this,
				&UTargeting::BreakLineOfSight,
				BreakLineOfSightDelay
			);
		}
	}
}

void UTargeting::TargetActor()
{
	ClosestTargetDistance = MinimumDistanceToEnable;

	if(bTargetLocked)
	{
		TargetLockOff();
	}
	else
	{
		const TArray<AActor*> Actors = GetAllActorsOfClass(TargetableActors);
		LockedOnTargetActor = FindNearestTarget(Actors);
		TargetLockOn(LockedOnTargetActor);
	}
}

void UTargeting::TargetLockOff()
{
	// Recast PlayerController in case it wasn't already setup on Begin Play (local split screen)
	SetupLocalPlayerController();

	bTargetLocked = false;

	if(LockedOnTargetActor)
	{
		if (bShouldControlRotation)
			ControlRotation(false);

		if (IsValid(OwnerPlayerController))
			OwnerPlayerController->ResetIgnoreLookInput();

		if (OnTargetLockedOff.IsBound())
			OnTargetLockedOff.Execute(LockedOnTargetActor);

		LockedOnTargetActor = nullptr;
	}
}

void UTargeting::TargetActorWithAxisInput(float AxisValue)
{
	if (bTargetLocked == false) return;

	if (LockedOnTargetActor == nullptr) return;

	if (ShouldSwitchTargetActor(AxisValue) == false) return;

	if (bIsSwitchingTarget) return;

	AActor* CurrentTarget = LockedOnTargetActor;

	const float RangeMin = AxisValue < 0 ? 0 : 180;
	const float RangeMax = AxisValue < 0 ? 180 : 360;

	ClosestTargetDistance = MinimumDistanceToEnable;

	TArray<AActor*> Actors = GetAllActorsOfClass(TargetableActors);
	TArray<AActor*> ActorsToLook;
	TArray<AActor*> ActorsToIgnore;

	ActorsToIgnore.Add(CurrentTarget);
	for (AActor* Actor : Actors)
	{
		const bool bHit = LineTraceForActor(Actor, ActorsToIgnore);
		if (bHit && IsInViewport(Actor))
			ActorsToLook.Add(Actor);
	}

	TArray<AActor*> TargetsInRange = FindTargetsInRange(ActorsToLook, RangeMin, RangeMax);

	AActor* ActorToTarget = nullptr;
	for (AActor* Actor : TargetsInRange)
	{
		const float Distance = GetDistanceFromCharacter(Actor);
		if(Distance < MinimumDistanceToEnable)
		{
			// 현재 타겟과 가장 가까운 녀석 찾기
			const float RelativeActorsDistance = CurrentTarget->GetDistanceTo(Actor);
			if(RelativeActorsDistance < ClosestTargetDistance)
			{
				ClosestTargetDistance = RelativeActorsDistance;
				ActorToTarget = Actor;
			}
		}
	}

	if(ActorToTarget)
	{
		if (SwitchingTargetTimerHandle.IsValid())
			SwitchingTargetTimerHandle.Invalidate();

		TargetLockOff();
		LockedOnTargetActor = ActorToTarget;
		TargetLockOn(ActorToTarget);

		GetWorld()->GetTimerManager().SetTimer(
			SwitchingTargetTimerHandle,
			this,
			&UTargeting::ResetIsSwitchingTarget,
			bIsSwitchingTarget ? 0.25f : 0.5f
		);

		bIsSwitchingTarget = true;
	}
}

bool UTargeting::GetTargetLockedStatus()
{
	return bTargetLocked;
}

AActor* UTargeting::GetLockedOnTargetActor() const
{
	return LockedOnTargetActor;
}

bool UTargeting::IsLocked() const
{
	return bTargetLocked && LockedOnTargetActor;
}

