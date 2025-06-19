// Unreal Core Framework | Copyright (c) 2024 José M. Nieves - MIT License.

#include "Input/CoreEnhancedInputComponent.h"

#include "Engine/Engine.h"

// Sets default values for this component's properties
UCoreEnhancedInputComponent::UCoreEnhancedInputComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UCoreEnhancedInputComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

// Called every frame
void UCoreEnhancedInputComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
