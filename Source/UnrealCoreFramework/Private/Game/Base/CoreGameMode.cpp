#include "Game/Base/CoreGameMode.h"

#include "Kismet/GameplayStatics.h"

void ACoreGameMode::InitGameState()
{
	// Set Default Pawn
	if (IsValid(PlayerPawnClass))
	{
		if (UClass* PawnClass = PlayerPawnClass.Get())
		{
			DefaultPawnClass = PawnClass;
		}
	}

	Super::InitGameState();
	OnGameStateInitialized();
}

void ACoreGameMode::OnGameStateInitialized() {}