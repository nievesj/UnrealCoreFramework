#pragma once

#include "Components/ActorComponent.h"
#include "IInteractable.h"

#include "CoreInteractionComponent.generated.h"

/**
 * Placed on interactable actors. Implements IInteractable with configurable
 * prompt text, interaction range, cooldown, and enabled state.
 */
UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class COREINTERACTION_API UCoreInteractionComponent : public UActorComponent, public IInteractable
{
	GENERATED_BODY()

public:
	UCoreInteractionComponent();

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractionPrompt_Implementation() const override;
	virtual bool CanInteract_Implementation(AActor* Interactor) const override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	FText PromptText = FText::FromString(TEXT("Interact"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
	float InteractionRange = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0"))
	float CooldownDuration = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool bEnabled = true;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteracted, AActor*, Interactor);

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteracted OnInteracted;

private:
	float LastInteractionTime = -999.0f;
};

