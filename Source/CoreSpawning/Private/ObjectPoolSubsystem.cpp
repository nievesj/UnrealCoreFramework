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

#include "ObjectPoolSubsystem.h"

DEFINE_LOG_CATEGORY(LogObjectPoolSubsystem);

void UObjectPoolSubsystem::Deinitialize()
{
	for (auto& Pair : Pools)
	{
		if (Pair.Value)
		{
			Pair.Value->DestroyAll(GetWorld());
		}
	}
	Pools.Empty();

	Super::Deinitialize();
}

void UObjectPoolSubsystem::CreateActorPool(const FName PoolName, TSubclassOf<AActor> ActorClass, const int32 PreWarmCount, const int32 MaxPoolSize)
{
	if (Pools.Contains(PoolName))
	{
		UE_LOG(LogObjectPoolSubsystem, Warning, TEXT("Pool [%s] already exists."), *PoolName.ToString());
		return;
	}

	TUniquePtr<FObjectPoolBase> Pool = MakeUnique<FObjectPoolBase>(ActorClass, MaxPoolSize);
	if (PreWarmCount > 0)
	{
		Pool->PreWarm(PreWarmCount, GetWorld());
	}
	Pools.Add(PoolName, MoveTemp(Pool));
}

AActor* UObjectPoolSubsystem::AcquireActorFromPool(const FName PoolName)
{
	TUniquePtr<FObjectPoolBase>* Found = Pools.Find(PoolName);
	if (!Found || !Found->IsValid())
	{
		UE_LOG(LogObjectPoolSubsystem, Warning, TEXT("Pool [%s] does not exist."), *PoolName.ToString());
		return nullptr;
	}

	return (*Found)->AcquireUntyped(GetWorld());
}

void UObjectPoolSubsystem::ReleaseActorToPool(const FName PoolName, AActor* Actor)
{
	TUniquePtr<FObjectPoolBase>* Found = Pools.Find(PoolName);
	if (!Found || !Found->IsValid())
	{
		UE_LOG(LogObjectPoolSubsystem, Warning, TEXT("Pool [%s] does not exist."), *PoolName.ToString());
		return;
	}

	(*Found)->ReleaseUntyped(Actor);
}

void UObjectPoolSubsystem::ReleaseAllInPool(const FName PoolName)
{
	TUniquePtr<FObjectPoolBase>* Found = Pools.Find(PoolName);
	if (!Found || !Found->IsValid())
	{
		return;
	}

	(*Found)->ReleaseAll();
}

bool UObjectPoolSubsystem::DoesPoolExist(const FName PoolName) const
{
	return Pools.Contains(PoolName);
}
