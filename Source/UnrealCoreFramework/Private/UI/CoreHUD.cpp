// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/CoreHUD.h"

void ACoreHUD::ShowMainHUD()
{
	UE_LOG(LogTemp, Log, TEXT("ShowMainHUD"));

	// Make widget owned by our PlayerController
	MainPage = CreateWidget<UCoreWidget>(GetWorld(), MainPageClass);
	MainPage->AddToViewport();
}

void ACoreHUD::HideMainHUD()
{
	if (MainPage)
	{
		MainPage->RemoveFromParent();
		MainPage = nullptr;
	}
}
