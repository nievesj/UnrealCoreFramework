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

#pragma once

#include "Components/ActorComponent.h"

#include "CoreInteractorComponent.generated.h"

class UCoreInteractionComponent;

DECLARE_LOG_CATEGORY_EXTERN(LogCoreInteractor, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableFound, AActor*, InteractableActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractableLost, AActor*, InteractableActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractedWith, AActor*, InteractableActor);

/**
 * Placed on the player. Detects nearby IInteractable actors via sphere overlap,
 * manages focus/highlight, and triggers Interact() on input.
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COREINTERACTION_API UCoreInteractorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCoreInteractorComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** Attempt to interact with the currently focused interactable. */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void TryInteract();

	/** Get the currently focused interactable actor. */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	AActor* GetFocusedInteractable() const { return FocusedActor; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
	float DetectionRadius = 300.0f;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractableFound OnInteractableFound;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractableLost OnInteractableLost;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractedWith OnInteracted;

private:
	void ScanForInteractables();

	UPROPERTY(Transient)
	TObjectPtr<AActor> FocusedActor;

	UPROPERTY(Transient)
	TObjectPtr<UCoreInteractionComponent> FocusedInteractionComp;
};

