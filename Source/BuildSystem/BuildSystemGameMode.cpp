// Copyright Epic Games, Inc. All Rights Reserved.

#include "BuildSystemGameMode.h"
#include "BuildSystemCharacter.h"
#include "UObject/ConstructorHelpers.h"

ABuildSystemGameMode::ABuildSystemGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
