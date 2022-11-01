// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "EnumList.generated.h"

UENUM(BlueprintType)
enum class EClassType : uint8
{
	Player,
	Boss,
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Hit,
	Spell,
	Dot
};

/**
 * 
 */
UCLASS()
class BOSSFIGHT_API UEnumList : public UUserDefinedEnum
{
	GENERATED_BODY()
	
};
