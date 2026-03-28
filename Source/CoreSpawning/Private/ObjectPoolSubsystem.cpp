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

