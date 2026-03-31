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

