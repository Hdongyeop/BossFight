// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyBase.h"
#include "StatComponent.h"
#include "SwordAnim.h"
#include "BossFightGameModeBase.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AIController.h"
#include "Manager.h"
#include "PlayableCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/WidgetComponent.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyBase::AEnemyBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ~ StatComponent
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat"));

	// ~ StunText
	StunText = CreateDefaultSubobject<UWidgetComponent>(TEXT("StunText"));
	StunText->SetupAttachment(GetMesh());
	StunText->SetWidgetSpace(EWidgetSpace::Screen);
	StunText->SetRelativeLocation(FVector(0.f, 0.f, 200.f));

	static ConstructorHelpers::FClassFinder<UUserWidget> UW_StunText(TEXT("WidgetBlueprint'/Game/UI/STUN_WBP.STUN_WBP_C'"));
	if (UW_StunText.Succeeded())
	{
		StunText->SetWidgetClass(UW_StunText.Class);
	}
}

// Called when the game starts or when spawned
void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	// ~ Anim Instance
	AnimInstance = Cast<USwordAnim>(GetMesh()->GetAnimInstance());

	// ~ AIController
	AIController = Cast<AAIController>(GetController());

	// ~ Recover BP Timer
	SetRecoverBpTimer(true);

	StunText->SetVisibility(false);
}

void AEnemyBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StunText->InitWidget();

}

// Called every frame
void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AEnemyBase::TakeDamage(EAttackType AttackType, int Damage, AActor* Causer)
{
	switch (AttackType)
	{
	case EAttackType::Hit:
		{
			auto NewHp = FMath::Clamp(StatComponent->GetHp() - Damage, 0, StatComponent->GetHp() - Damage);
			StatComponent->SetHp(NewHp);

			if(NewHp <= 0)
			{
				AnimInstance->Death();

				AIController->BrainComponent->StopLogic(TEXT("Boss Dead"));

				// ~ Show Result UI
				auto GM = Cast<ABossFightGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
				GM->ClearSceneWidget();
				GM->SetSceneWidget(GM->Result_Widget);
			}
		}
		break;
	}
}

void AEnemyBase::TakeBlockDamage(AActor* Attacker, int32 Damage)
{
	auto NewBp = FMath::Clamp(StatComponent->GetBp() - Damage, 0, StatComponent->GetBp() - Damage);
	StatComponent->SetBp(NewBp);

	if(NewBp <= 0)
	{
		// ~ AI Stop
		AIController->GetBrainComponent()->PauseLogic(TEXT("BP is empty"));

		// ~ Stop auto refill BP
		SetRecoverBpTimer(false);

		StunText->SetVisibility(true);

		// ~ AI Restart after 5 seconds & Refill BP & auto refill BP
		GetWorld()->GetTimerManager().SetTimer(RecoverStun, FTimerDelegate::CreateLambda(
			[&]()
			{
				SetRecoverBpTimer(true);
				StunText->SetVisibility(false);
				StatComponent->SetBp(StatComponent->GetMaxBp());
				AIController->GetBrainComponent()->ResumeLogic(TEXT("BP is empty"));
			}
		), 5.f, false);
	}
}

void AEnemyBase::SetRecoverBpTimer(bool flag)
{
	if(flag == true)
	{
		GetWorld()->GetTimerManager().SetTimer(RecoverBp, [&]()
			{
				// UE_LOG(LogTemp, Warning, TEXT("AutoRecoverBP"));

				auto HpRatio = StatComponent->GetHp() / (float)StatComponent->GetMaxHp();
				auto NewBp = FMath::Clamp(StatComponent->GetBp() + (int)(HpRatio * 3), 0, StatComponent->GetMaxBp());
				StatComponent->SetBp(NewBp);
			}, 2.f, true);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(RecoverBp);
		// UE_LOG(LogTemp, Warning, TEXT("AutoRecoverBP Stop"));
	}
}
