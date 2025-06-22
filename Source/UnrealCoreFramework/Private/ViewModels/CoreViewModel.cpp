// Fill out your copyright notice in the Description page of Project Settings.

#include "ViewModels/CoreViewModel.h"

#include "Tools/SubsystemHelper.h"

void UCoreViewModel::Initialize()
{
	ViewModelManagerSubsystem = USubsystemHelper::GetSubsystem<UViewModelManagerSubsystem>(this);
	OnInitialized();
}

void UCoreViewModel::Refresh()
{
	OnRefreshed();
}