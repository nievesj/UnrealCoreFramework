#pragma once

#include "Engine/DataAsset.h"

#include "CoreGameFlowConfigDataAsset.generated.h"

class UCoreGameFlowState;

/** Defines a single state in the flow. */
USTRUCT(BlueprintType)
struct FCoreGameFlowStateDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameFlow")
	FName StateName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameFlow")
	TSubclassOf<UCoreGameFlowState> StateClass;
};

/** Defines a valid transition between two states. */
USTRUCT(BlueprintType)
struct FCoreGameFlowTransitionDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameFlow")
	FName FromState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameFlow")
	FName ToState;
};

/**
 * Data asset defining the complete game flow graph:
 * states, valid transitions, and the initial state.
 */
UCLASS(BlueprintType)
class UNREALCOREFRAMEWORK_API UCoreGameFlowConfigDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameFlow")
	FName InitialState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameFlow")
	TArray<FCoreGameFlowStateDefinition> States;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameFlow")
	TArray<FCoreGameFlowTransitionDefinition> Transitions;
};

