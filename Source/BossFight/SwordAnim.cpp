// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordAnim.h"
#include "PlayableCharacter.h"
#include "Manager.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

USwordAnim::USwordAnim()
{
	// Anim
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM_Attack(TEXT("AnimMontage'/Game/Anims/AttackCombo.AttackCombo'"));
	if (AM_Attack.Succeeded())
		AttackAnim = AM_Attack.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM_Block(TEXT("AnimMontage'/Game/Anims/Block.Block'"));
	if (AM_Block.Succeeded())
		BlockAnim = AM_Block.Object;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM_Roll_F(TEXT("AnimMontage'/Game/SwordAnimationPack/Animations/Roll/Roll_F_Montage.Roll_F_Montage'"));
	if (AM_Roll_F.Succeeded())
		RollAnim.Add(AM_Roll_F.Object);
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM_Roll_B(TEXT("AnimMontage'/Game/SwordAnimationPack/Animations/Roll/RollB_Montage.RollB_Montage'"));
	if (AM_Roll_B.Succeeded())
		RollAnim.Add(AM_Roll_B.Object);
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM_Roll_L(TEXT("AnimMontage'/Game/SwordAnimationPack/Animations/Roll/RollL_Montage.RollL_Montage'"));
	if (AM_Roll_L.Succeeded())
		RollAnim.Add(AM_Roll_L.Object);
	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM_Roll_R(TEXT("AnimMontage'/Game/SwordAnimationPack/Animations/Roll/RollR_Montage.RollR_Montage'"));
	if (AM_Roll_R.Succeeded())
		RollAnim.Add(AM_Roll_R.Object);

	static ConstructorHelpers::FObjectFinder<UAnimMontage> AM_Death(TEXT("AnimMontage'/Game/Anims/Death_Montage.Death_Montage'"));
	if (AM_Death.Succeeded())
		DeathAnim = AM_Death.Object;
}

void USwordAnim::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	// Character
	auto Actor = GetOwningActor();
	if(IsValid(Actor))
		OwningCharacter = Cast<APlayableCharacter>(Actor);
}

void USwordAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	if(IsValid(Pawn))
	{
		Speed = Pawn->GetVelocity().Size();

		Rotation = Pawn->GetActorRotation().Yaw;
	}
}

void USwordAnim::Attack()
{
	if (bComboAble == false && IsMontagePlaying == true) return;

	IsMontagePlaying = true;
	bComboAble = false;

	Montage_Play(AttackAnim, 1.0f);

	FName NextComboName = *FString::Printf(TEXT("Combo%d"), AttackComboIndex);
	Montage_JumpToSection(NextComboName, AttackAnim);
}

void USwordAnim::Block()
{
	if (IsMontagePlaying == true) return;

	IsMontagePlaying = true;
	Montage_Play(BlockAnim);
}

void USwordAnim::Roll()
{
	if (IsMontagePlaying == true) return;

	IsMontagePlaying = true;

	if (IsLocked)
	{
		float Horizontal = MoveDirection.Y;
		float Vertical = MoveDirection.X;

		if (Horizontal == 0.f && Vertical == 0.f)
			Montage_Play(RollAnim[1]); // Back
		else if (Horizontal > 0.f)
			Montage_Play(RollAnim[3]); // Right
		else if (Horizontal < 0.f)
			Montage_Play(RollAnim[2]); // Left
		else if (Vertical > 0.f)
			Montage_Play(RollAnim[0]); // Forward
		else
			Montage_Play(RollAnim[1]);
	}
	else
	{
		Montage_Play(RollAnim[0]);
	}
}

void USwordAnim::Death()
{
	Montage_Stop(0.1f, nullptr);
	
	bDeath = true;
}

void USwordAnim::AnimNotify_MontageEnd()
{
	OnMontageEnd.Broadcast();
}

void USwordAnim::AnimNotify_AttackHit()
{
	OnAttackHit.Broadcast();
}

void USwordAnim::AnimNotify_ComboAble()
{
	bComboAble = true;
	AttackComboIndex = (AttackComboIndex + 1) % 3;
}

void USwordAnim::AnimNotify_ComboDisable()
{
	bComboAble = false;
	AttackComboIndex = 0;
}

void USwordAnim::AnimNotify_Dodgeable()
{
	OwningCharacter->GetCapsuleComponent()->SetCollisionProfileName("Dodge");
}

void USwordAnim::AnimNotify_DodgeableEnd()
{

	OwningCharacter->GetCapsuleComponent()->SetCollisionProfileName("Pawn");
}

void USwordAnim::AnimNotify_Blockable()
{
	OwningCharacter->SetBlockable(true);

}

void USwordAnim::AnimNotify_BlockableEnd()
{
	OwningCharacter->SetBlockable(false);
}

