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
			Actor = World->SpawnActor<AActor>(Config->SpawnClass, SpawnTransform, Params);
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

