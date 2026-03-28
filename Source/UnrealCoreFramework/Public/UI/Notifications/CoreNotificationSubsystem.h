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