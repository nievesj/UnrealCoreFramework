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

