// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EWeaponEquipped : uint8
{
	EKnife,
	EPrimary,
	ESecondary,
	EAbility
};


class RADIANT_API WeaponEnum
{
public:
	WeaponEnum();
	~WeaponEnum();
};
