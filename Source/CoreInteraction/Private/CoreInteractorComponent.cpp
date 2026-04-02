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
	UCoreInteractionComponent* ClosestInteractionComp = nullptr;
	float ClosestDistance = MAX_FLT;

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* OtherActor = Overlap.GetActor();
		if (!OtherActor || OtherActor == Owner)
		{
			continue;
		}

		UCoreInteractionComponent* InteractionComp = OtherActor->FindComponentByClass<UCoreInteractionComponent>();
		if (!InteractionComp)
		{
			continue;
		}

		if (!IInteractable::Execute_CanInteract(InteractionComp, const_cast<AActor*>(Owner)))
		{
			continue;
		}

		const float Distance = FVector::Dist(Location, OtherActor->GetActorLocation());
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestActor = OtherActor;
			ClosestInteractionComp = InteractionComp;
		}
	}

	if (ClosestActor != FocusedActor)
	{
		if (FocusedActor)
		{
			OnInteractableLost.Broadcast(FocusedActor);
		}

		FocusedActor = ClosestActor;
		FocusedInteractionComp = ClosestInteractionComp;

		if (FocusedActor)
		{
			OnInteractableFound.Broadcast(FocusedActor);
		}
	}
}

void UCoreInteractorComponent::TryInteract()
{
	if (!FocusedActor || !FocusedInteractionComp)
	{
		return;
	}

	if (!IInteractable::Execute_CanInteract(FocusedInteractionComp, GetOwner()))
	{
		return;
	}

	IInteractable::Execute_Interact(FocusedInteractionComp, GetOwner());
	OnInteracted.Broadcast(FocusedActor);
}
