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

#pragma once

#include "SubSystems/Base/CoreGameInstanceSubsystem.h"

#include "CoreNotificationSubsystem.generated.h"

class UCoreWidget;

DECLARE_LOG_CATEGORY_EXTERN(LogCoreNotification, Log, All);

USTRUCT(BlueprintType)
struct FCoreNotification
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Notification)
	FText Title;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Notification)
	FText Message;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Notification)
	float DisplayDuration = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Notification)
	FName Tag;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotificationShown, const FCoreNotification&, Notification);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNotificationDismissed, const FCoreNotification&, Notification);

/**
 * Subsystem for queueing and displaying UI notifications.
 * Consumers bind to OnNotificationShown/OnNotificationDismissed to drive their widget display.
 */
UCLASS()
class UNREALCOREFRAMEWORK_API UCoreNotificationSubsystem : public UCoreGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** Queue a notification for display. */
	UFUNCTION(BlueprintCallable, Category = Notification)
	void ShowNotification(const FCoreNotification& Notification);

	/** Dismiss the currently displayed notification. */
	UFUNCTION(BlueprintCallable, Category = Notification)
	void DismissCurrentNotification();

	/** Dismiss all queued and active notifications. */
	UFUNCTION(BlueprintCallable, Category = Notification)
	void ClearAllNotifications();

	/** Fired when a notification is shown. */
	UPROPERTY(BlueprintAssignable, Category = Notification)
	FOnNotificationShown OnNotificationShown;

	/** Fired when a notification is dismissed. */
	UPROPERTY(BlueprintAssignable, Category = Notification)
	FOnNotificationDismissed OnNotificationDismissed;

private:
	void ProcessQueue();
	void HandleDismissTimer();

	TArray<FCoreNotification> NotificationQueue;
	FCoreNotification CurrentNotification;
	bool bIsDisplaying = false;
	FTimerHandle DismissTimerHandle;
};