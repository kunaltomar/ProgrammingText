// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "starbreezeGameMode.h"
#include "starbreezeHUD.h"
#include "starbreezeCharacter.h"
#include "Enemy.h"
#include "Engine.h"
#include "starbreezeCharacter.h"
#include "Runtime/Engine/Classes/Engine/TargetPoint.h"
#include "UObject/ConstructorHelpers.h"

AstarbreezeGameMode::AstarbreezeGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AstarbreezeHUD::StaticClass();
}

