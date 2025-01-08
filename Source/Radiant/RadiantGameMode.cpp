// Copyright Epic Games, Inc. All Rights Reserved.

#include "RadiantGameMode.h"
#include "RadiantCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARadiantGameMode::ARadiantGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/Jett/BP_Jett"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
