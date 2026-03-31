// MIT License
//
// Copyright (c) 2026 José M. Nieves
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include "Game/Base/CoreGameState.h"

#include "Engine/World.h"
#include "Game/Base/CoreGameInstance.h"
#include "Kismet/GameplayStatics.h"

ACoreGameState::ACoreGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bAlwaysRelevant = true;
	SetReplicatingMovement(false);

	// Note: this is very important to set to false. Though all replication infos are spawned at run time, during seamless travel
	// they are held on to and brought over into the new world. In ULevel::InitializeNetworkActors, these PlayerStates may be treated as map/startup actors
	// and given static NetGUIDs. This also causes their deletions to be recorded and sent to new clients, which if unlucky due to name conflicts,
	// may end up deleting the new PlayerStates they had just spawned.
	bNetLoadOnClient = false;

	// Default to every few seconds.
	ServerWorldTimeSecondsUpdateFrequency = 0.1f;

	SumServerWorldTimeSecondsDelta = 0.0;
	NumServerWorldTimeSecondsDeltas = 0;
}

void ACoreGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	OnComponentsInitialized();
}

void ACoreGameState::ReceivedGameModeClass()
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* const PlayerController = Iterator->Get();
		if (PlayerController)
		{
			PlayerController->ReceivedGameModeClass(GameModeClass);
		}
	}
}

void ACoreGameState::OnComponentsInitialized()
{
	if (UGameInstance* pGameInstance = UGameplayStatics::GetGameInstance(GetWorld()))
	{
		UCoreGameInstance* pCoreGameInstance = Cast<UCoreGameInstance>(pGameInstance);
	}
}
