#pragma once

#include "UObject/Interface.h"

#include "IInteractable.generated.h"

/**
 * Interface for actors that can be interacted with.
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

class COREINTERACTION_API IInteractable
{
	GENERATED_BODY()

public:
	/** Execute the interaction. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor);

	/** Get display prompt text for this interactable. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionPrompt() const;

	/** Check if interaction is currently available. */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	bool CanInteract(AActor* Interactor) const;
};

