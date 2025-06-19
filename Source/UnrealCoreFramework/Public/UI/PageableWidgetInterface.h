// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/Interface.h"

#include "PageableWidgetInterface.generated.h"

/**
 *
 */
UINTERFACE(MinimalAPI)
class UPageableWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class UNREALCOREFRAMEWORK_API IPageableWidgetInterface
{
	GENERATED_BODY()

public:
	virtual void Open();
	virtual void Close();
	bool		 DisablePlayerControllerInput;
};