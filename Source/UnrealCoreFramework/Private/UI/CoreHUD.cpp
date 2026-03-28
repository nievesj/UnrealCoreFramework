// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/CoreHUD.h"

void ACoreHUD::BeginPlay()
{
	Super::BeginPlay();
	ShowMainHUD();
}

void ACoreHUD::ShowMainHUD()
{
	if (!MainPageClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CoreHUD] MainPageClass is NOT set — aborting."));
		return;
	}

	APlayerController* PC = GetOwningPlayerController();
	if (!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CoreHUD] No owning PlayerController — aborting."));
		return;
	}

	MainPage = CreateWidget<UCoreWidget>(PC, MainPageClass);
	if (!MainPage)
	{
		UE_LOG(LogTemp, Warning, TEXT("[CoreHUD] CreateWidget returned null — aborting."));
		return;
	}

	MainPage->AddToViewport();

	if (!MainPage->IsActivated())
	{
		MainPage->ActivateWidget();
	}
}

void ACoreHUD::HideMainHUD()
{
	if (MainPage)
	{
		MainPage->RemoveFromParent();
		MainPage = nullptr;
	}
}
