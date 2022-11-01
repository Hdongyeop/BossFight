// Copyright Epic Games, Inc. All Rights Reserved.


#include "BossFightGameModeBase.h"
#include "PlayableCharacter.h"
#include "WidgetManager.h"
#include "MainUI.h"
#include "ResultUI.h"

ABossFightGameModeBase::ABossFightGameModeBase()
{
	// ~ Set Default Pawn
	static ConstructorHelpers::FClassFinder<APlayableCharacter> PC(TEXT("Blueprint'/Game/Blueprints/PlayableCharacter_BP.PlayableCharacter_BP_C'"));
	if (PC.Succeeded())
		DefaultPawnClass = PC.Class;

	// ~ Set Default Player Controller
	static ConstructorHelpers::FClassFinder<APlayerController> PController(TEXT("Blueprint'/Game/Blueprints/PlayerController_BP.PlayerController_BP_C'"));
	if (PController.Succeeded())
		PlayerControllerClass = PController.Class;

	// ~~ HUD UI
	static ConstructorHelpers::FClassFinder<UMainUI> HUD_UI(TEXT("WidgetBlueprint'/Game/UI/HUD_WBP.HUD_WBP_C'"));
	if (HUD_UI.Succeeded())
		HUD_Class = HUD_UI.Class;

	// ~~ Title UI
	static ConstructorHelpers::FClassFinder<UUserWidget> Title_UI(TEXT("WidgetBlueprint'/Game/UI/MainMenu_WBP.MainMenu_WBP_C'"));
	if (Title_UI.Succeeded())
		Title_Class = Title_UI.Class;

	// ~ Options UI
	static ConstructorHelpers::FClassFinder<UUserWidget> Option_UI(TEXT("WidgetBlueprint'/Game/UI/Options_WBP.Options_WBP_C'"));
	if (Option_UI.Succeeded())
		Option_Class = Option_UI.Class;

	// ~~ Result UI
	static ConstructorHelpers::FClassFinder<UResultUI> Result_UI(TEXT("WidgetBlueprint'/Game/UI/Result_WBP.Result_WBP_C'"));
	if (Result_UI.Succeeded())
		Result_Class = Result_UI.Class;
}

void ABossFightGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("GameMode BeginPlay"));

	HUD_Widget = CreateWidget(GetWorld(), HUD_Class);
	Result_Widget = CreateWidget(GetWorld(), Result_Class);
	Title_Widget = CreateWidget(GetWorld(), Title_Class);
	Option_Widget = CreateWidget(GetWorld(), Option_Class);

	SetSceneWidget(Title_Widget);
}

void ABossFightGameModeBase::SetSceneWidget(UUserWidget* CurrentSceneWidget)
{
	CurrentScene_Widget = CurrentSceneWidget;
	CurrentScene_Widget->AddToViewport();
}

void ABossFightGameModeBase::ClearSceneWidget() const
{
	CurrentScene_Widget->RemoveFromViewport();
}
