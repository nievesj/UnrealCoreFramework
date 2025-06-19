// Fill out your copyright notice in the Description page of Project Settings.

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
	// Tell each PlayerController that the Game class is here
	for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
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
