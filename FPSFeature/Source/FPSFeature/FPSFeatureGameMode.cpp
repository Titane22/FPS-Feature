// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "FPSFeatureGameMode.h"
#include "FPSFeatureHUD.h"
#include "Character/FPSFeatureCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFPSFeatureGameMode::AFPSFeatureGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FPCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSFeatureHUD::StaticClass();
}
