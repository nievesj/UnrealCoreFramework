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

protected:
	virtual void BeginPlay() override;

private:
	void ScanForInteractables();

	UPROPERTY(Transient)
	TObjectPtr<AActor> FocusedActor;
};

