#include "PoolableActor.h"

#include "Components/PrimitiveComponent.h"

APoolableActor::APoolableActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APoolableActor::OnAcquired_Implementation()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
}

void APoolableActor::OnReleased_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	Execute_ResetToPool(this);
}

void APoolableActor::ResetToPool_Implementation()
{
	SetActorLocation(FVector::ZeroVector);
	SetActorRotation(FRotator::ZeroRotator);
}

