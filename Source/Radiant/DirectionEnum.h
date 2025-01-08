// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EMovementDirection : uint8
{
	EStationary,
	EForward,
	EBackward,
	ERight,
	ELeft
};

class RADIANT_API DirectionEnum
{
public:
	DirectionEnum();
	~DirectionEnum();
};
