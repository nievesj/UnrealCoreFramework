// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameModeBase.h"

#include "CoreGameMode.generated.h"

UCLASS()
class UNREALCOREFRAMEWORK_API ACoreGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void InitGameState() override;
	virtual void OnGameStateInitialized();

	UPROPERTY(EditAnywhere, Category = UnrealCoreFrameworkSettings)
	TSubclassOf<APawn> PlayerPawnClass;
};
