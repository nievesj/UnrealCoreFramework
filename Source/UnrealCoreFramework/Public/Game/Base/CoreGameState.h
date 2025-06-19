// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"

#include "CoreGameState.generated.h"

class UServiceLocator;
/**
 *
 */
UCLASS()
class UNREALCOREFRAMEWORK_API ACoreGameState : public AGameStateBase
{
	GENERATED_UCLASS_BODY()
public:
	virtual void ReceivedGameModeClass() override;

protected:
	virtual void PostInitializeComponents() override;

	virtual void OnComponentsInitialized();
};
