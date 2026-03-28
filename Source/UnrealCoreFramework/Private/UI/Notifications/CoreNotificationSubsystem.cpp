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