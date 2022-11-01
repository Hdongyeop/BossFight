// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayableCharacter.h"
#include "SwordAnim.h"
#include "Targeting.h"
#include "EnemyBase.h"
#include "Manager.h"
#include "StatComponent.h"
#include "EnemyBase.h"
#include "BossFightGameModeBase.h"
#include "ResultUI.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayableCharacter::APlayableCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ~ Mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("SkeletalMesh'/Game/SwordAnimationPack/Demo/Mannequin/Character/Mesh/SK_Mannequin.SK_Mannequin'"));
	if (SM.Succeeded())
		GetMesh()->SetSkeletalMesh(SM.Object);
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -88.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, 270.f, 0.f));

	// ~ CharacterMovement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;

	// ~ SpringArm
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Arm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->TargetArmLength = NormalCameraLength;
	CameraArm->bUsePawnControlRotation = true;
	CameraArm->SetRelativeLocation(FVector(0.f, 0.f, 80.f));

	// ~ Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Cam"));
	Camera->SetupAttachment(CameraArm);

	// ~ Sword
	Sword = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Sword"));
	Sword->SetupAttachment(GetMesh(), TEXT("SwordR"));
	Sword->SetCollisionProfileName(FName(TEXT("NoCollision")));

	// ~ Sword Collision
	SwordCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	SwordCollision->SetupAttachment(Sword);
	SwordCollision->GetComponentLocation() = FVector(0.f, 70.f, 0.f);
	SwordCollision->GetComponentRotation() = FRotator(90.f, 0.f, 0.f);
	SwordCollision->SetCapsuleHalfHeight(80.f);
	SwordCollision->SetCapsuleRadius(20.f);
	SwordCollision->SetCollisionProfileName(FName(TEXT("Weapon")));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_Weapon(TEXT("StaticMesh'/Game/SwordAnimationPack/Sword/Sword.Sword'"));
	if (SM_Weapon.Succeeded())
		Sword->SetStaticMesh(SM_Weapon.Object);

	// ~ Targeting
	TargetSystem = CreateDefaultSubobject<UTargeting>(TEXT("TargetSystem"));
	TargetSystem->bShouldControlRotation = true;
	TargetSystem->OnTargetLockedOn.BindDynamic(this, &APlayableCharacter::OnLockedOn);
	TargetSystem->OnTargetLockedOff.BindDynamic(this, &APlayableCharacter::OnLockedOff);

	// ~ StatComponent
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("Stat"));

	// ~ AnimInstance
	static ConstructorHelpers::FClassFinder<UAnimInstance> AMI(TEXT("AnimBlueprint'/Game/Anims/SwordAnim_ABP.SwordAnim_ABP_C'"));
	if(AMI.Succeeded())
		GetMesh()->SetAnimInstanceClass(AMI.Class);
}

// Called when the game starts or when spawned
void APlayableCharacter::BeginPlay()
{
	Super::BeginPlay();

	// ~ GameMode
	Manager = Cast<UManager>(GetGameInstance());

	// ~ PlayerController
	MyController = Cast<APlayerController>(GetController());

	/*FTimerHandle TempTimer;
	GetWorld()->GetTimerManager().SetTimer(TempTimer, [&]()
		{
			UE_LOG(LogTemp, Warning, TEXT("Horizontal : %f, Vertical : %f"), AnimInstance->MoveDirection.Y, AnimInstance->MoveDirection.X);
		}, 0.5, true);*/
}

void APlayableCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	// ~ AnimInstance Delegate
	AnimInstance = Cast<USwordAnim>(GetMesh()->GetAnimInstance());
	AnimInstance->OnMontageEnd.AddDynamic(this, &APlayableCharacter::MontageEnd);
	AnimInstance->OnAttackHit.AddDynamic(this, &APlayableCharacter::AttackHit);
}

// Called every frame
void APlayableCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ~ 달리면 카메라 멀어지게
	/*if (IsRunning && AnimInstance->GetSpeed() > NormalSpeed)
		CameraArm->TargetArmLength = 
		FMath::Lerp(CameraArm->TargetArmLength, RunningCameraLength, 2 * DeltaTime);
	else
		CameraArm->TargetArmLength =
		FMath::Lerp(CameraArm->TargetArmLength, NormalCameraLength, 2 * DeltaTime);*/
}

// Called to bind functionality to input
void APlayableCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// ~ Axis
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &APlayableCharacter::TurnRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APlayableCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &APlayableCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &APlayableCharacter::MoveRight);

	// ~ Action
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Pressed, this, &APlayableCharacter::EnterRun);
	PlayerInputComponent->BindAction(TEXT("Run"), IE_Released, this, &APlayableCharacter::ExitRun);
	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &APlayableCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("Block"), IE_Pressed, this, &APlayableCharacter::Block);
	PlayerInputComponent->BindAction(TEXT("Roll"), IE_Pressed, this, &APlayableCharacter::Roll);
	PlayerInputComponent->BindAction(TEXT("Lock"), IE_Pressed, this, &APlayableCharacter::Lock);

}

void APlayableCharacter::TurnRight(float Value)
{
	AddControllerYawInput(Value);

	TargetSystem->TargetActorWithAxisInput(Value);
}

void APlayableCharacter::LookUp(float Value)
{
	AddControllerPitchInput(-Value);
}

void APlayableCharacter::MoveForward(float Value)
{
	// 컨트롤러가 바라보는 방향으로 움직이자
	FRotator controlYaw = FRotator(0.f, GetControlRotation().Yaw, 0.f);
	FVector Direction = UKismetMathLibrary::GetForwardVector(controlYaw);

	AnimInstance->MoveDirection.X = Value;
	AddMovementInput(Direction, Value);
}

void APlayableCharacter::MoveRight(float Value)
{
	// 컨트롤러가 바라보는 방향의 오른쪽으로 움직이자
	FRotator controlYaw = FRotator(0.f, GetControlRotation().Yaw, 0.f);
	FVector Direction = UKismetMathLibrary::GetRightVector(controlYaw);

	AnimInstance->MoveDirection.Y = Value;
	AddMovementInput(Direction, Value);
}

void APlayableCharacter::EnterRun()
{
	IsRunning = true;
	GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
}

void APlayableCharacter::ExitRun()
{
	IsRunning = false;
	GetCharacterMovement()->MaxWalkSpeed = NormalSpeed;
}

void APlayableCharacter::Attack()
{
	if (bGameStarted == false) return;

	AnimInstance->Attack();
}

void APlayableCharacter::AttackHit()
{
	// ~ Sound
	auto AttackSound = Manager->AttackSound[AnimInstance->GetAttackComboIndex()];
	Manager->PlaySound(this, AttackSound);

	// ~ Overlap Event
	TArray<AActor*> OverlappedActors;
	SwordCollision->GetOverlappingActors(OverlappedActors, AEnemyBase::StaticClass());
	for (AActor* Actor : OverlappedActors)
	{
		// UE_LOG(LogTemp, Warning, TEXT("Hit : %s"), *Actor->GetName());
		// ~ 데미지 처리는 받는쪽에서
		auto Enemy = Cast<AEnemyBase>(Actor);
		if (IsValid(Enemy) == false) return;
		if (Enemy->StatComponent->GetHp() <= 0) return;

		auto AttackData = Manager->GetAttackData(TEXT("NormalAttack"));
		if (AttackData == nullptr) return;

		auto TotalDamage = AttackData->Damage + StatComponent->GetDamage();

		Enemy->TakeDamage(AttackData->AttackType, TotalDamage, this);
	}
}

void APlayableCharacter::Block()
{
	if (bGameStarted == false) return;

	AnimInstance->Block();
}

void APlayableCharacter::Roll()
{
	if (bGameStarted == false) return;

	AnimInstance->Roll();
}

void APlayableCharacter::Lock()
{
	// UE_LOG(LogTemp, Warning, TEXT("PRESS TAB"));
	TargetSystem->TargetActor();
}

void APlayableCharacter::TakeDamage(EAttackType AttackType, int Damage, AActor* Causer)
{
	switch (AttackType)
	{
	case EAttackType::Hit:
	{
		if(bBlockable)
		{
			// 막는 사운드 & 체간 게이지 오르는 것등

			// ~ Sound
			auto BlockSound = Manager->BlockSound;

			Manager->PlaySound(this, BlockSound);

			auto Enemy = Cast<AEnemyBase>(Causer);
			Enemy->TakeBlockDamage(this, StatComponent->GetBlockDamage());
		}
		else
		{
			// ~ Set Hp
			auto NewHp = FMath::Clamp(StatComponent->GetHp() - Damage, 0, StatComponent->GetHp() - Damage);
			StatComponent->SetHp(NewHp);

			// ~ Death Anim & No input & Show ResultUI
			if(NewHp <= 0)
			{
				TargetSystem->TargetLockOff();

				AnimInstance->Death();
				
				DisableInput(UGameplayStatics::GetPlayerController(this, 0));

				if(IsValid(Manager))
				{
					// UE_LOG(LogTemp, Warning, TEXT("MyGameMode Check"));
					auto GM = Cast<ABossFightGameModeBase>(UGameplayStatics::GetGameMode(GetWorld()));
					GM->ClearSceneWidget();
					GM->SetSceneWidget(GM->Result_Widget);
				}
			}
		}
	}
	break;
	}

}

void APlayableCharacter::MontageEnd()
{
	AnimInstance->IsMontagePlaying = false;
}

void APlayableCharacter::OnLockedOn(AActor* TargetActor)
{
	Target = TargetActor;
	AnimInstance->IsLocked = true;
}

void APlayableCharacter::OnLockedOff(AActor* TargetActor)
{
	Target = nullptr;
	AnimInstance->IsLocked = false;
}