#pragma once

#include "UObject/Object.h"

#include "CoreGameFlowState.generated.h"

/**
 * Represents a named state in the game flow (MainMenu, Loading, InGame, Paused, GameOver).
 * Subclass and override OnEnter/OnExit for state-specific logic.
 */
UCLASS(Blueprintable, Abstract)
class UNREALCOREFRAMEWORK_API UCoreGameFlowState : public UObject
{
	GENERATED_BODY()

public:
	/** Called when entering this state. */
	UFUNCTION(BlueprintNativeEvent, Category = "GameFlow")
	void OnEnter();
	virtual void OnEnter_Implementation() {}

	/** Called when exiting this state. */
	UFUNCTION(BlueprintNativeEvent, Category = "GameFlow")
	void OnExit();
	virtual void OnExit_Implementation() {}

	/** Called each frame while this state is active. Override for per-frame logic. */
	UFUNCTION(BlueprintNativeEvent, Category = "GameFlow")
	void OnTick(float DeltaTime);
	virtual void OnTick_Implementation(float DeltaTime) {}

	/** Display name for this state. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameFlow")
	FName StateName;
};

