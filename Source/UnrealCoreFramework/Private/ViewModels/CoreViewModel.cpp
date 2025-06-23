// Fill out your copyright notice in the Description page of Project Settings.

#include "ViewModels/CoreViewModel.h"

#include "Kismet/GameplayStatics.h"
#include "Tools/SubsystemHelper.h"

DEFINE_LOG_CATEGORY(LogCoreViewModel);

void UCoreViewModel::Initialize()
{
	ViewModelManagerSubsystem = USubsystemHelper::GetSubsystem<UViewModelManagerSubsystem>(this);
	OnInitialized();
}

void UCoreViewModel::DeInitialize()
{
	ViewModelManagerSubsystem = nullptr;
}

void UCoreViewModel::Refresh()
{
	OnRefreshed();
	OnRefreshed_BP();
}

APlayerController* UCoreViewModel::GetPlayerController() const
{
	return UGameplayStatics::GetPlayerController(this, 0);
}

ACharacter* UCoreViewModel::GetPlayerCharacter() const
{
	return UGameplayStatics::GetPlayerCharacter(this, 0);
}