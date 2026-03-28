#include "CoreInteractorComponent.h"

#include "IInteractable.h"
#include "CoreInteractionComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY(LogCoreInteractor);

UCoreInteractorComponent::UCoreInteractorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UCoreInteractorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCoreInteractorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	ScanForInteractables();
}

void UCoreInteractorComponent::ScanForInteractables()
{
	const AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	const FVector Location = Owner->GetActorLocation();
	TArray<FOverlapResult> Overlaps;
	FCollisionShape Sphere = FCollisionShape::MakeSphere(DetectionRadius);

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	World->OverlapMultiByChannel(
		Overlaps,
		Location,
		FQuat::Identity,
		ECC_Visibility,
		Sphere);

	AActor* ClosestActor = nullptr;
	float ClosestDistance = MAX_FLT;

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* OtherActor = Overlap.GetActor();
		if (!OtherActor || OtherActor == Owner)
		{
			continue;
		}

		if (!OtherActor->FindComponentByClass<UCoreInteractionComponent>())
		{
			continue;
		}

		UCoreInteractionComponent* InteractionComp = OtherActor->FindComponentByClass<UCoreInteractionComponent>();
		if (!IInteractable::Execute_CanInteract(InteractionComp, const_cast<AActor*>(Owner)))
		{
			continue;
		}

		const float Distance = FVector::Dist(Location, OtherActor->GetActorLocation());
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestActor = OtherActor;
		}
	}

	if (ClosestActor != FocusedActor)
	{
		if (FocusedActor)
		{
			OnInteractableLost.Broadcast(FocusedActor);
		}

		FocusedActor = ClosestActor;

		if (FocusedActor)
		{
			OnInteractableFound.Broadcast(FocusedActor);
		}
	}
}

void UCoreInteractorComponent::TryInteract()
{
	if (!FocusedActor)
	{
		return;
	}

	UCoreInteractionComponent* InteractionComp = FocusedActor->FindComponentByClass<UCoreInteractionComponent>();
	if (!InteractionComp)
	{
		return;
	}

	if (!IInteractable::Execute_CanInteract(InteractionComp, GetOwner()))
	{
		return;
	}

	IInteractable::Execute_Interact(InteractionComp, GetOwner());
	OnInteracted.Broadcast(FocusedActor);
}

