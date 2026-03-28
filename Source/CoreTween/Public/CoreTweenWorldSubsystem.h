#pragma once

#include "CoreTweenBuilder.h"
#include "Subsystems/WorldSubsystem.h"
#include "Templates/SharedPointer.h"

#include "CoreTweenWorldSubsystem.generated.h"

class UWidget;
struct FCoreTweenState;

/**
 * Per-world storage for active tween states.
 * Replaces the former static TArray on UCoreTween so that PIE worlds
 * each get their own isolated set of tweens.
 */
UCLASS()
class CORETWEEN_API UCoreTweenWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Register a new tween state for tracking. Called by FCoreTweenBuilder::Run(). */
	void RegisterTweenState(TSharedPtr<FCoreTweenState> State);

	/** Cancel all active tweens on the given widget. Returns number cancelled. */
	int32 Clear(UWidget* Widget);

	/** @return true if any tween is actively running on this widget. */
	bool GetIsTweening(UWidget* Widget);

	/** Force-complete all active tweens. Each jumps to end state, fires OnComplete, and exits. */
	void CompleteAll();

	/** Accessor for the active tween states array. */
	TArray<TSharedPtr<FCoreTweenState>>& GetActiveTweenStates() { return ActiveTweenStates; }

private:
	/** Prune finished entries from the tracking array. */
	void PruneFinished();

	TArray<TSharedPtr<FCoreTweenState>> ActiveTweenStates;
};
