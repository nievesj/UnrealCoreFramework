#include "SpawnerFactory.h"

#include "ObjectPoolSubsystem.h"
#include "Engine/World.h"

void USpawnerFactory::InitializeFromConfig(UObjectPoolSubsystem* PoolSubsystem, const USpawnerConfigDataAsset* Config)
{
	if (!Config || !Config->bUsePool || !PoolSubsystem)
	{
		return;
	}

	if (PoolSubsystem->DoesPoolExist(Config->PoolName))
	{
		return;
	}

	PoolSubsystem->CreateActorPool(Config->PoolName, Config->SpawnClass, Config->PoolPreWarmCount, Config->MaxPoolSize);
}

AActor* USpawnerFactory::SpawnFromConfig(UObject* WorldContext, UObjectPoolSubsystem* PoolSubsystem, const USpawnerConfigDataAsset* Config, const FTransform& SpawnTransform)
{
	if (!Config || !Config->SpawnClass)
	{
		return nullptr;
	}

	AActor* Actor = nullptr;

	if (Config->bUsePool && PoolSubsystem)
	{
		Actor = PoolSubsystem->AcquireActorFromPool(Config->PoolName);
	}
	else if (WorldContext)
	{
		UWorld* World = WorldContext->GetWorld();
		if (World)
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			Actor = World->SpawnActor<AActor>(Config->SpawnClass, &SpawnTransform, Params);
		}
	}

	if (Actor)
	{
		Actor->SetActorTransform(SpawnTransform);
	}

	return Actor;
}

void USpawnerFactory::ReleaseFromConfig(UObjectPoolSubsystem* PoolSubsystem, const USpawnerConfigDataAsset* Config, AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	if (Config && Config->bUsePool && PoolSubsystem)
	{
		PoolSubsystem->ReleaseActorToPool(Config->PoolName, Actor);
	}
	else
	{
		Actor->Destroy();
	}
}

