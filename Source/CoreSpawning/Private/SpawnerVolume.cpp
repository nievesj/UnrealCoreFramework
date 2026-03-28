#include "SpawnerVolume.h"

#include "Components/BoxComponent.h"

ASpawnerVolume::ASpawnerVolume()
{
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawnVolume"));
	SpawnVolume->SetBoxExtent(FVector(200.0f, 200.0f, 100.0f));
	SpawnVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SpawnVolume->SetHiddenInGame(true);
	SetRootComponent(SpawnVolume);
}

FTransform ASpawnerVolume::GetSpawnTransform_Implementation()
{
	if (!SpawnVolume)
	{
		return GetActorTransform();
	}

	const FVector Origin = SpawnVolume->GetComponentLocation();
	const FVector Extent = SpawnVolume->GetScaledBoxExtent();

	const FVector RandomPoint = FMath::RandPointInBox(FBox(Origin - Extent, Origin + Extent));

	return FTransform(GetActorRotation(), RandomPoint, FVector::OneVector);
}

