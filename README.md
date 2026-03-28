# Unreal Core Framework

Unreal Engine **5.7+** · Requires the [AsyncFlow](https://github.com/nievesj/AsyncFlow) plugin.

A plugin for UE 5.7+ that ships reusable game systems as Subsystems, Components, and base classes. Everything async runs on `AsyncFlow` coroutines — no callbacks, no `TFuture`/`TPromise`, no `FTSTicker`.

---

## Modules

| Module | What it does |
|--------|-------------|
| `UnrealCoreFramework` | Core subsystems, UI framework, MVVM, input, camera, HTTP, asset loading, level management |
| `CoreTween` | AsyncFlow coroutine-native tweening library. Each tween is an `AsyncFlow::TTask<void>`. |
| `CoreInteraction` | Component-based interact system (`CoreInteractorComponent` + `CoreInteractionComponent`) |
| `CoreSave` | Save/load via `CoreSaveSubsystem` and `CoreSaveGame` |
| `CoreSpawning` | Object pooling: `ObjectPool`, `ObjectPoolSubsystem`, `Spawner`, `SpawnerVolume`, `SpawnerFactory` |

---

## CoreTween

A tweening library where every tween is a self-contained AsyncFlow coroutine. No centralized update loop — each tween owns its own `co_await NextTick` loop, which makes it natively awaitable, cancellable, and composable via `WhenAll`/`WhenAny`.

Replaces BUITween entirely. BUITween is deleted; `CoreRedirects` handle Blueprint asset migration.

### C++ Usage

**Basic tween — fire and forget:**

```cpp
UCoreTween::Create(MyWidget, 0.3f)
    .FromOpacity(0.0f)
    .ToOpacity(1.0f)
    .Easing(ECoreTweenEasingType::EaseOutCubic)
    .Run(this)
    .Start();
```

**Awaitable tween inside an AsyncFlow coroutine:**

```cpp
AsyncFlow::TTask<void> UMyWidget::FadeInTask()
{
    UCF_ASYNC_CONTRACT(this);

    co_await UCoreTween::Create(this, 0.4f)
        .FromOpacity(0.0f)
        .ToOpacity(1.0f)
        .FromScale(FVector2D(0.8f, 0.8f))
        .ToScale(FVector2D::UnitVector)
        .Easing(ECoreTweenEasingType::EaseOutBack)
        .Run(this);

    // Runs after the tween finishes
    UE_LOG(LogTemp, Log, TEXT("Fade-in complete"));
}
```

**Parallel tweens:**

```cpp
co_await AsyncFlow::WhenAll(
    UCoreTween::Create(WidgetA, 0.3f).ToOpacity(1.0f).Run(this),
    UCoreTween::Create(WidgetB, 0.3f).ToOpacity(1.0f).Run(this)
);
```

**Sequential chain:**

```cpp
co_await FCoreTweenSequence::Create()
    .Then(UCoreTween::Create(Widget, 0.2f).ToOpacity(1.0f))
    .Then(UCoreTween::Create(Widget, 0.3f).ToScale(FVector2D::UnitVector))
    .Run(this);
```

**Loop and ping-pong:**

```cpp
UCoreTween::Create(PulseWidget, 0.5f)
    .FromOpacity(0.3f)
    .ToOpacity(1.0f)
    .SetLoops(-1)       // -1 = infinite
    .SetPingPong(true)
    .Easing(ECoreTweenEasingType::EaseInOutSine)
    .Run(this)
    .Start();
```

**Cancel and query:**

```cpp
UCoreTween::Clear(MyWidget);                    // Cancel all tweens on a widget
bool bAnimating = UCoreTween::GetIsTweening(MyWidget);
UCoreTween::CompleteAll();                      // Jump all active tweens to end state
```

### Blueprint Usage

Wire up nodes in this order: `CreateTween` → property setters → `RunTween`.

All Blueprint nodes accept a plain `UWidget*` — no `UCoreWidget` requirement.

| Node | Purpose |
|------|---------|
| `CreateTween(Widget, Duration, Delay, bAdditive)` | Returns a param chain object |
| `ToOpacity`, `FromScale`, `SetEasing`, etc. | Configure the tween |
| `RunTween(Params, WorldContext)` | Starts the tween (fire-and-forget) |
| `ClearTweens(Widget)` | Cancel all tweens on a widget |
| `IsTweening(Widget)` | Query if any tween is active |

### Supported Properties

| Property | Type | Notes |
|----------|------|-------|
| Translation | `FVector2D` | Render transform translation |
| Scale | `FVector2D` | Render transform scale |
| Rotation | `float` | Render transform angle |
| Opacity | `float` | Render opacity |
| Color | `FLinearColor` | Works on `UUserWidget`, `UImage`, `UBorder` |
| Canvas Position | `FVector2D` | Requires `UCanvasPanelSlot` parent |
| Padding | `FMargin` | Works on `UOverlaySlot`, `UHorizontalBoxSlot`, `UVerticalBoxSlot` |
| Visibility | `ESlateVisibility` | Snaps at alpha threshold (instant, not interpolated) |
| Max Desired Height | `float` | Requires `USizeBox` |

### Easing Functions (31 types)

Linear · Quad · Cubic · Quart · Quint · Expo · Sine · Circ · Back · Elastic · Bounce · Spring

Each family has In, Out, and InOut variants. Back, Elastic, and Spring accept an optional parameter (overshoot, amplitude, damping).

### Time Sources

| Source | Behavior |
|--------|----------|
| `Unpaused` | Ticks during pause. Default for widget tweens. |
| `GameTime` | Respects pause and time dilation. |
| `RealTime` | Wall-clock time, ignores everything. |

---

## Async Conventions

Every async API in the framework returns `AsyncFlow::TTask<T>`. Old callback overloads, `TFuture`/`TPromise` helpers, and ticker-based subsystems have been removed.

### UCF_ASYNC_CONTRACT

Every AsyncFlow coroutine starts with this macro. It captures a weak pointer to the owning UObject and cancels the coroutine if the owner is destroyed mid-flight.

```cpp
AsyncFlow::TTask<void> UMySubsystem::DoSomethingAsync()
{
    UCF_ASYNC_CONTRACT(this);

    co_await AsyncFlow::Delay(this, 2.0f);
    UE_LOG(LogTemp, Log, TEXT("Two seconds later"));
}
```

### Rules

1. **Task ownership** — Store every launched `TTask` as a member variable.
2. **Cancellation** — Call `Task.Cancel()` in `Deinitialize()` (subsystems) or `EndPlay()` (components).
3. **Debug names** — Call `Task.SetDebugName(TEXT("..."))` on every framework-launched task.
4. **Public headers** — Include `AsyncFlowTask.h` (minimal). Include `AsyncFlow.h` (umbrella) only in `.cpp` files.

---

## Data Asset Manager

`UDataAssetManagerSubsystem` (Game Instance scope) handles loading, caching, and lifecycle of `UDataAsset` subclasses via the engine's `UAssetManager`.

### Sync loading

```cpp
UDataAssetManagerSubsystem* DAM = GetGameInstance()->GetSubsystem<UDataAssetManagerSubsystem>();
UDataAsset* Asset = DAM->LoadDataAssetByPrimaryAssetId(MyAssetId);
UDataAsset* Asset2 = DAM->LoadDataAssetByClass(UMyDataAsset::StaticClass(), TEXT("SwordOfFire"));
```

### Async loading (AsyncFlow coroutine)

```cpp
AsyncFlow::TTask<void> UMySystem::LoadWeaponsTask()
{
    UCF_ASYNC_CONTRACT(this);

    UDataAssetManagerSubsystem* DAM = GetGameInstance()->GetSubsystem<UDataAssetManagerSubsystem>();

    UDataAsset* Sword = co_await DAM->LoadDataAssetAsyncTask(SwordAssetId);
    TArray<UDataAsset*> AllWeapons = co_await DAM->LoadAllDataAssetsOfTypeAsyncTask(UWeaponDataAsset::StaticClass());
}
```

The `OnDataAssetLoaded` delegate still broadcasts for Blueprint consumers.

---

## Level Manager

`ULevelManagerSubsystem` (Game Instance scope) wraps level loading and unloading into AsyncFlow coroutine tasks. Supports concurrent streaming operations.

### Blueprint (fire-and-forget)

```cpp
LevelManager->LoadLevel(TEXT("/Game/Maps/Dungeon"), ELevelOperation::Stream);
LevelManager->UnloadLevel(TEXT("/Game/Maps/Dungeon"));
```

### C++ AsyncFlow coroutine

```cpp
AsyncFlow::TTask<void> UMyGameMode::TransitionTask()
{
    UCF_ASYNC_CONTRACT(this);

    ULevelManagerSubsystem* LM = GetGameInstance()->GetSubsystem<ULevelManagerSubsystem>();
    const bool bLoaded = co_await LM->LoadLevelTask(TEXT("/Game/Maps/Dungeon"), ELevelOperation::Stream);

    if (bLoaded)
    {
        co_await LM->UnloadLevelTask(TEXT("/Game/Maps/Town"));
    }
}
```

Delegates: `OnLevelLoadingStarted`, `OnLevelLoadingComplete`, `OnLevelLoadingError`.

---

## HTTP (RestRequest)

`URestRequest` wraps `FHttpModule` requests into AsyncFlow coroutine tasks.

### Fire-and-forget (Blueprint-friendly)

```cpp
RestRequest->Request(EHttpRequestType::GET, TEXT("https://api.example.com/data"));
// Bind to HTTPResponseRecievedDelegate for the result
```

### AsyncFlow coroutine

```cpp
AsyncFlow::TTask<void> UMyService::FetchDataTask()
{
    UCF_ASYNC_CONTRACT(this);

    FHttpResponsePtr Response = co_await RestRequest->RequestTask(
        EHttpRequestType::POST,
        TEXT("https://api.example.com/submit"),
        TEXT("{\"key\": \"value\"}")
    );

    if (Response.IsValid())
    {
        UE_LOG(LogTemp, Log, TEXT("Response: %s"), *Response->GetContentAsString());
    }
}
```

---

## UI System

### Widget Hierarchy

```
UCommonActivatableWidget
  └── UCoreWidget          ← Base class. Handles Show/Hide with tween or animation transitions.
        ├── UCorePage      ← Full-screen page with optional exit button and input blocking.
        ├── UCoreModal     ← Modal dialog with confirm/cancel/dismiss and awaitable result.
        └── (your widgets)
```

### UCoreWidget

Every `UCoreWidget` supports three animation modes (configured in `FCoreWidgetAnimationSettings`):

| Mode | Behavior |
|------|----------|
| `WidgetTween` | CoreTween-driven transitions (scale, translation, fade) |
| `WidgetAnimation` | UMG Widget Animations (bind `IntroAnimation` / `OutroAnimation`) |
| `None` | No animation — `OnShown`/`OnHidden` fires immediately |

**Show/Hide (fire-and-forget):**

```cpp
MyWidget->Show();   // Plays intro, fires OnShown() when done
MyWidget->Hide();   // Plays outro, fires OnHidden() when done
```

**Awaitable Show/Hide:**

```cpp
co_await MyWidget->ShowTask();
// Widget is fully visible here
co_await SomeGameLogic();
co_await MyWidget->HideTask();
// Widget is gone
```

### UCoreModal

Modal dialog with confirm/cancel/dismiss support. Optional `ConfirmButton`, `CancelButton`, `CloseButton`, `TitleText`, and `MessageText` can be bound from the Widget Blueprint.

**Awaitable — resolve inline in an AsyncFlow coroutine:**

```cpp
AsyncFlow::TTask<void> UMySystem::AskUserTask()
{
    UCF_ASYNC_CONTRACT(this);

    UCoreModal* Modal = UI->AddWidgetToStack(UMyConfirmDialog::StaticClass(), EWidgetContainerType::Modal);
    Modal->SetTitle(FText::FromString(TEXT("Delete Save?")));
    Modal->SetMessage(FText::FromString(TEXT("This cannot be undone.")));

    ECoreModalResult Result = co_await Modal->ShowAndWaitTask();

    if (Result == ECoreModalResult::Confirmed)
    {
        SaveSubsystem->DeleteSave();
    }
}
```

**Fire-and-forget — bind to the delegate in Blueprint or C++:**

```cpp
Modal->OnModalResult.AddDynamic(this, &UMyHandler::HandleModalResult);
Modal->Show();
```

| Result | Meaning |
|--------|---------|
| `Confirmed` | User pressed the confirm button |
| `Cancelled` | User pressed the cancel button |
| `Dismissed` | Modal closed without an explicit choice (close button, escape) |

### UISubsystem

`UUISubsystem` (Local Player scope) manages the widget stack:

```cpp
UUISubsystem* UI = GetLocalPlayer()->GetSubsystem<UUISubsystem>();
UCoreWidget* Page = UI->AddWidgetToStack(UMyPage::StaticClass(), EWidgetContainerType::Page);
UI->RemoveWidgetFromStack(*Page, EWidgetContainerType::Page);
```

### TweenManagerSubsystem

Orchestrates CoreTween transitions on `UCoreWidget` instances. You rarely call this directly — `UCoreWidget::Show()`/`Hide()` call it for you. Direct use:

```cpp
UTweenManagerSubsystem* TM = GetSubsystem<UTweenManagerSubsystem>();

// Fire-and-forget
TM->PlayWidgetTransitionEffect(Widget, TransitionOptions, EWidgetTransitionMode::Intro);

// Awaitable
co_await TM->PlayWidgetTransitionEffectTask(Widget, TransitionOptions, EWidgetTransitionMode::Intro);
```

---

## MVVM (ViewModel System)

Built on top of Epic's `UMVVMViewModelBase`.

### UCoreViewModel

Base class with lifecycle hooks: `Initialize()`, `DeInitialize()`, `Refresh()`.

```cpp
UCLASS()
class UPlayerStatsViewModel : public UCoreViewModel
{
    GENERATED_BODY()

protected:
    virtual void OnInitialized() override
    {
        AMyCharacter* Character = GetSourceAs<AMyCharacter>();
        Health = Character->GetHealth();
    }

    virtual void OnRefreshed() override
    {
        AMyCharacter* Character = GetSourceAs<AMyCharacter>();
        Health = Character->GetHealth();
        UE_MVVM_SET_PROPERTY_VALUE(Health, Health);
    }

    UPROPERTY(BlueprintReadOnly, FieldNotify)
    float Health = 0.0f;
};
```

### ViewModelManagerSubsystem

World-scoped subsystem that manages ViewModel instances keyed by tracked object:

```cpp
UViewModelManagerSubsystem* VMMS = GetWorld()->GetSubsystem<UViewModelManagerSubsystem>();

// Type-safe creation
UPlayerStatsViewModel* VM = VMMS->GetOrCreateViewModel<UPlayerStatsViewModel>(MyPawn);

// Blueprint-friendly creation
UCoreViewModel* VM = VMMS->GetOrCreateViewModel(UPlayerStatsViewModel::StaticClass(), MyPawn);

// Cleanup
VMMS->RemoveViewModel(MyPawn);
```

`UCoreWidget` auto-discovers its ViewModel on activation if you set `ViewModelClass` in the editor. The widget calls `OnViewModelBound()` once the ViewModel is resolved.

---

## Subsystem Base Classes

Thin wrappers over Epic's subsystem types. Exist so the framework can add shared behavior later without touching every subsystem individually.

| Base Class | Engine Parent | Scope |
|-----------|--------------|-------|
| `UCoreGameInstanceSubsystem` | `UGameInstanceSubsystem` | Game instance lifetime |
| `UCoreWorldSubsystem` | `UWorldSubsystem` | Per-world lifetime |
| `UCoreLocalPlayerSubsystem` | `ULocalPlayerSubsystem` | Per local player |
| `UCoreEngineSubsystem` | `UEngineSubsystem` | Engine lifetime |
| `UCoreTickableWorldSubsystem` | `UTickableWorldSubsystem` | Per-world, ticked |

---

## Other Systems

### Input
- `UCoreEnhancedInputComponent` — Extended Enhanced Input component.
- `UCoreEnhancedPlayerInput` — Extended player input class.

### Camera
- `UCoreCameraComponent` — Extended camera component.
- `UCoreCameraManagerSubsystem` — Camera management subsystem.

### Health
- `UCoreHealthComponent` — Actor component for health tracking.

### Interaction (CoreInteraction module)
- `UCoreInteractorComponent` — Attach to the actor that initiates interactions.
- `UCoreInteractionComponent` — Attach to the actor that receives interactions.
- `IInteractable` — Interface for interactable objects.

### Save (CoreSave module)
- `UCoreSaveSubsystem` — Save/load operations.
- `UCoreSaveGame` — Base save game class.

### Spawning (CoreSpawning module)
- `UObjectPool` / `UObjectPoolSubsystem` — Object pooling.
- `ASpawner` — Spawner actor.
- `ASpawnerVolume` — Volume-based spawning.
- `USpawnerFactory` — Spawner creation factory.
- `IPoolable` — Interface for poolable actors.

---

## Project Structure

```
UnrealCoreFramework/
├── Source/
│   ├── UnrealCoreFramework/     Main module
│   │   ├── Public/
│   │   │   ├── Async/           CoreAsyncTypes.h (conventions + UCF_ASYNC_CONTRACT)
│   │   │   ├── Camera/          CoreCameraComponent, CoreCameraManagerSubsystem
│   │   │   ├── Components/      CoreHealthComponent
│   │   │   ├── Data/            CoreDataAsset, CorePrimaryDataAsset
│   │   │   ├── Debug/           CoreCheatManager
│   │   │   ├── Game/            GameFlow classes
│   │   │   ├── Http/            RestRequest
│   │   │   ├── Input/           CoreEnhancedInputComponent, CoreEnhancedPlayerInput
│   │   │   ├── Settings/        UnrealCoreFrameworkSettings
│   │   │   ├── SubSystems/      All subsystem headers
│   │   │   ├── Tools/           SubsystemHelper, HelperStatics
│   │   │   ├── Tween/           TweenManagerSubsystem
│   │   │   ├── UI/              CoreWidget, CorePage, CoreHUD, AnimatableWidgetInterface
│   │   │   └── ViewModels/      CoreViewModel, ViewModelManagerSubsystem
│   │   └── Private/             Implementations
│   ├── CoreTween/               Tweening library module
│   │   ├── Public/
│   │   │   ├── CoreTween.h              Static factory + Blueprint library
│   │   │   ├── CoreTweenBuilder.h       Fluent builder + FCoreTweenState
│   │   │   ├── CoreTweenEasing.h        31 easing functions
│   │   │   ├── CoreTweenProperty.h      TCoreTweenProp<T>, TCoreTweenInstantProp<T>
│   │   │   ├── CoreTweenSequence.h      Sequential chain
│   │   │   ├── CoreTweenTypes.h         Delegates + ECoreTweenTimeSource
│   │   │   └── Targets/
│   │   │       ├── CoreTweenTarget.h    ICoreTweenTarget interface
│   │   │       └── WidgetTweenTarget.h  UWidget implementation
│   │   └── Private/             Implementations
│   ├── CoreInteraction/         Interaction system module
│   ├── CoreSave/                Save system module
│   └── CoreSpawning/            Object pool + spawner module
└── UnrealCoreFramework.uplugin
```

---

## Dependencies

**Required plugin:** `AsyncFlow` (must be enabled in `.uproject`).

**Engine modules used:** `Core`, `CoreUObject`, `Engine`, `UMG`, `Slate`, `SlateCore`, `HTTP`, `EnhancedInput`, `CommonUI`, `ModelViewViewModel`, `FieldNotification`, `Json`, `JsonUtilities`, `OnlineSubsystemUtils`.

---

## Status

Early development. API is not stable.
