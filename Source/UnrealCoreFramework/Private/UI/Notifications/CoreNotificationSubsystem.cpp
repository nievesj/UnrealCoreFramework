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

#include "UI/Notifications/CoreNotificationSubsystem.h"

#include "Engine/World.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY(LogCoreNotification);

void UCoreNotificationSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogCoreNotification, Log, TEXT("CoreNotificationSubsystem initialized"));
}

void UCoreNotificationSubsystem::Deinitialize()
{
	ClearAllNotifications();
	Super::Deinitialize();
}

void UCoreNotificationSubsystem::ShowNotification(const FCoreNotification& Notification)
{
	NotificationQueue.Add(Notification);

	if (!bIsDisplaying)
	{
		ProcessQueue();
	}
}

void UCoreNotificationSubsystem::DismissCurrentNotification()
{
	if (!bIsDisplaying)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(DismissTimerHandle);
	}

	bIsDisplaying = false;
	OnNotificationDismissed.Broadcast(CurrentNotification);

	ProcessQueue();
}

void UCoreNotificationSubsystem::ClearAllNotifications()
{
	NotificationQueue.Empty();

	if (bIsDisplaying)
	{
		const UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().ClearTimer(DismissTimerHandle);
		}

		bIsDisplaying = false;
		OnNotificationDismissed.Broadcast(CurrentNotification);
	}
}

void UCoreNotificationSubsystem::ProcessQueue()
{
	if (NotificationQueue.Num() == 0)
	{
		return;
	}

	CurrentNotification = NotificationQueue[0];
	NotificationQueue.RemoveAt(0);
	bIsDisplaying = true;

	OnNotificationShown.Broadcast(CurrentNotification);

	if (CurrentNotification.DisplayDuration > 0.0f)
	{
		const UWorld* World = GetWorld();
		if (World)
		{
			World->GetTimerManager().SetTimer(
				DismissTimerHandle,
				this,
				&UCoreNotificationSubsystem::HandleDismissTimer,
				CurrentNotification.DisplayDuration,
				false);
		}
	}
}

void UCoreNotificationSubsystem::HandleDismissTimer()
{
	DismissCurrentNotification();
}