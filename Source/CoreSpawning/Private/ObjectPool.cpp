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

#include "ObjectPool.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"

DEFINE_LOG_CATEGORY(LogObjectPool);

FObjectPoolBase::FObjectPoolBase(TSubclassOf<AActor> InActorClass, const int32 InMaxPoolSize)
	: ActorClass(InActorClass)
	, MaxPoolSize(InMaxPoolSize)
{
}

FObjectPoolBase::~FObjectPoolBase()
{
	InactiveActors.Empty();
	ActiveActors.Empty();
}

void FObjectPoolBase::PreWarm(const int32 InCount, UWorld* World)
{
	if (!World || !ActorClass)
	{
		return;
	}

	const int32 ActorsToSpawn = FMath::Min(InCount, MaxPoolSize - InactiveActors.Num());
	for (int32 Idx = 0; Idx < ActorsToSpawn; ++Idx)
	{
		if (AActor* Actor = SpawnPooledActor(World))
		{
			DeactivateActor(Actor);
			InactiveActors.Add(Actor);
		}
	}
}

AActor* FObjectPoolBase::AcquireUntyped(UWorld* World)
{
	AActor* Actor = nullptr;

	// Prune stale entries from the inactive pool
	while (InactiveActors.Num() > 0)
	{
		TWeakObjectPtr<AActor> WeakActor = InactiveActors.Pop(EAllowShrinking::No);
		if (WeakActor.IsValid())
		{
			Actor = WeakActor.Get();
			break;
		}
	}

	if (!Actor && World && ActorClass)
	{
		Actor = SpawnPooledActor(World);
	}

	if (Actor)
	{
		ActiveActors.Add(Actor);
		ActivateActor(Actor);
	}

	return Actor;
}

void FObjectPoolBase::ReleaseUntyped(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	if (!ActiveActors.Contains(Actor))
	{
		UE_LOG(LogObjectPool, Warning, TEXT("Tried to release actor [%s] not owned by this pool."), *Actor->GetName());
		return;
	}

	ActiveActors.Remove(Actor);

	if (InactiveActors.Num() < MaxPoolSize)
	{
		DeactivateActor(Actor);
		InactiveActors.Add(Actor);
	}
	else
	{
		Actor->Destroy();
	}
}

void FObjectPoolBase::ReleaseAll()
{
	TArray<TWeakObjectPtr<AActor>> ToRelease = ActiveActors.Array();
	for (const TWeakObjectPtr<AActor>& WeakActor : ToRelease)
	{
		if (WeakActor.IsValid())
		{
			ReleaseUntyped(WeakActor.Get());
		}
	}
	ActiveActors.Empty();
}

void FObjectPoolBase::DestroyAll(UWorld* World)
{
	for (const TWeakObjectPtr<AActor>& WeakActor : ActiveActors)
	{
		if (WeakActor.IsValid())
		{
			WeakActor->Destroy();
		}
	}
	ActiveActors.Empty();

	for (const TWeakObjectPtr<AActor>& WeakActor : InactiveActors)
	{
		if (WeakActor.IsValid())
		{
			WeakActor->Destroy();
		}
	}
	InactiveActors.Empty();
}

AActor* FObjectPoolBase::SpawnPooledActor(UWorld* World)
{
	if (!World || !ActorClass)
	{
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	return World->SpawnActor<AActor>(ActorClass, FTransform::Identity, Params);
}

void FObjectPoolBase::DeactivateActor(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	if (Actor->GetClass()->ImplementsInterface(UPoolable::StaticClass()))
	{
		IPoolable::Execute_OnReleased(Actor);
	}
	else
	{
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorEnableCollision(false);
		Actor->SetActorTickEnabled(false);
	}
}

void FObjectPoolBase::ActivateActor(AActor* Actor)
{
	if (!Actor)
	{
		return;
	}

	if (Actor->GetClass()->ImplementsInterface(UPoolable::StaticClass()))
	{
		IPoolable::Execute_OnAcquired(Actor);
	}
	else
	{
		Actor->SetActorHiddenInGame(false);
		Actor->SetActorEnableCollision(true);
		Actor->SetActorTickEnabled(true);
	}
}
