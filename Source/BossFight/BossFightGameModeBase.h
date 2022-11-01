// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BossFightGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class BOSSFIGHT_API ABossFightGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TSubclassOf<UUserWidget> HUD_Class;

	UPROPERTY()
	TSubclassOf<UUserWidget> Result_Class;

	UPROPERTY()
	TSubclassOf<UUserWidget> Title_Class;

	UPROPERTY()
	TSubclassOf<UUserWidget> Option_Class;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UUserWidget* HUD_Widget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UUserWidget* Result_Widget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UUserWidget* Title_Widget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UUserWidget* Option_Widget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UUserWidget* CurrentScene_Widget;

public:
	ABossFightGameModeBase();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void SetSceneWidget(UUserWidget* CurrentSceneWidget);

	UFUNCTION(BlueprintCallable)
	void ClearSceneWidget() const;
};
