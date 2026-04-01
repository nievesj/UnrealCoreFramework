# Subsystems

The framework provides abstract base classes for every Unreal Engine subsystem scope, plus concrete subsystems for level management, data assets, game flow, audio, input, UI, notifications, tweening, and view model management.

---

## Base Subsystem Classes

All base classes are `UCLASS(Abstract)` and provide standardized lifecycle hooks.

### UCoreEngineSubsystem

**Scope:** Engine lifetime (singleton, survives everything)

| Method | Description |
|---|---|
| `ShouldCreateSubsystem()` | Returns `true` by default |
| `Initialize()` | Called when the subsystem collection initializes |
| `Deinitialize()` | Called on shutdown |
| `GetFunctionCallspace()` | Overridden for network context checking |

### UCoreGameInstanceSubsystem

**Scope:** Game instance lifetime (survives level transitions)

| Method | Description |
|---|---|
| `PreInitialize()` | Called before `Initialize()` |
| `Initialize()` | Standard initialization |
| `PostInitialize()` | Called after `Initialize()` |
| `Deinitialize()` | Cleanup |
| `GetFunctionCallspace()` | Network context |

### UCoreWorldSubsystem

**Scope:** World lifetime (created/destroyed with each level)

| Method | Description |
|---|---|
| `PreInitialize()` | Called before `Initialize()` |
| `Initialize()` | Standard initialization |
| `PostInitialize()` | Called after all world subsystems initialize |
| `OnWorldBeginPlay()` | Called when gameplay starts |
| `OnWorldComponentsUpdated()` | Called after world components update |
| `UpdateStreamingState()` | For streaming level management |
| `Deinitialize()` | Cleanup |

### UCoreLocalPlayerSubsystem

**Scope:** Local player lifetime (per-player, local only)

| Method | Description |
|---|---|
| `PreInitialize()` | Before init |
| `Initialize()` | Standard init |
| `PostInitialize()` | After init |
| `Deinitialize()` | Cleanup |
| `GetFunctionCallspace()` | Network context |

### UCoreTickableWorldSubsystem

**Scope:** World lifetime, with per-frame ticking

| Method | Description |
|---|---|
| `PreInitialize()` / `Initialize()` / `PostInitialize()` | Lifecycle |
| `Tick(DeltaTime)` | Called every frame |
| `Deinitialize()` | Cleanup |

### UCoreAudioEngineSubSystem

**Scope:** Audio device lifetime

| Method | Description |
|---|---|
| `ShouldCreateSubsystem()` | Returns `true` by default |
| `Initialize()` / `Deinitialize()` | Lifecycle |
| `GetFunctionCallspace()` | Network context |

---

## Concrete Subsystems

### ULevelManagerSubsystem

**Scope:** Game Instance

Manages level loading and unloading. Both Blueprint (fire-and-forget) and C++ (coroutine) APIs.

**Blueprint API:**

```cpp
UFUNCTION(BlueprintCallable)
void LoadLevel(const FString& LevelPath,
    ELevelOperation Operation = ELevelOperation::Open,
    ELevelLoadMethod LoadMethod = ELevelLoadMethod::Async);

UFUNCTION(BlueprintCallable)
void LoadLevelSoftObject(const TSoftObjectPtr<UWorld>& Level, ...);

UFUNCTION(BlueprintCallable)
void UnloadLevel(const FString& LevelPath);

UFUNCTION(BlueprintPure)
bool IsLevelLoaded(const FString& LevelPath) const;

UFUNCTION(BlueprintPure)
bool IsLevelLoading(const FString& LevelPath) const;
```

**Coroutine API:**

```cpp
AsyncFlow::TTask<bool> LoadLevelTask(const FString& LevelPath, ELevelOperation Operation);
AsyncFlow::TTask<bool> LoadLevelTask(const TSoftObjectPtr<UWorld>& Level, ELevelOperation Operation);
AsyncFlow::TTask<bool> UnloadLevelTask(const FString& LevelPath);
```

**Delegates:**
- `OnLevelLoadingStarted(FString LevelName)`
- `OnLevelLoadingComplete(FString LevelName)`
- `OnLevelLoadingError(FString LevelName, FString ErrorMessage)`

**Enums:**
- `ELevelLoadMethod`: `Synchronous`, `Async`
- `ELevelOperation`: `Open` (full map travel), `Stream` (additive)

### UDataAssetManagerSubsystem

**Scope:** Game Instance (via `UCoreGameInstanceSubsystem`)

Centralized manager for Primary Data Assets with caching.

| Method | Description |
|---|---|
| `LoadDataAssetByPrimaryAssetId()` | Sync load by asset ID |
| `LoadDataAssetByClass()` | Sync load by class + name |
| `LoadDataAssetAsyncTask()` | Async load by asset ID → `TTask<UDataAsset*>` |
| `LoadDataAssetByClassAsyncTask()` | Async load by class + name → `TTask<UDataAsset*>` |
| `LoadAllDataAssetsOfType()` | Sync load all of a type |
| `LoadAllDataAssetsOfTypeAsyncTask()` | Async load all → `TTask<TArray<UDataAsset*>>` |
| `GetAllAssetIds()` | Get all registered asset IDs for a class |
| `IsAssetLoaded()` | Check if cached |
| `PreloadAssets()` | Batch preload |
| `UnloadAsset()` | Remove from cache |
| `ClearCache()` | Clear all cached assets |

**Delegate:** `OnDataAssetLoaded(UDataAsset*, bool bSuccess)`

### UCoreGameFlowSubsystem

**Scope:** Game Instance

Data-driven game flow state machine. See [Game Flow](#game-flow-system) section below.

### UUISubsystem

**Scope:** Local Player

Per-player UI management. See [UI System](03-UISystem.md).

### UCoreInputSubsystem

**Scope:** Local Player

Detects hardware input device changes and broadcasts `OnCoreInputDeviceChanged`:

```cpp
UENUM(BlueprintType)
enum class ECoreInputDeviceType : uint8
{
    None,
    KeyboardMouse,
    Gamepad,
    XboxGamePad,
    PS5GamePad
};
```

### UInputSubsystem

**Scope:** Local Player

Input configuration and mapping context management.

### UAudioSubsystem

**Scope:** Audio Engine

Audio device lifecycle management. Extends `UCoreAudioEngineSubSystem`.

### UCoreNotificationSubsystem

**Scope:** Game Instance

Queued notification display system:

```cpp
USTRUCT(BlueprintType)
struct FCoreNotification
{
    FText Title;
    FText Message;
    float DisplayDuration = 5.0f;
    FName Tag;
};
```

| Method | Description |
|---|---|
| `ShowNotification()` | Queue a notification |
| `DismissCurrentNotification()` | Dismiss the active one |
| `ClearAllNotifications()` | Clear queue and active |

**Delegates:** `OnNotificationShown`, `OnNotificationDismissed`

### UTweenManagerSubsystem

**Scope:** Game Instance

Bridges CoreTween to the UI animation system:

| Method | Description |
|---|---|
| `PlayWidgetTransitionEffect()` | Fire-and-forget widget transition |
| `PlayWidgetTransitionEffectTask()` | Coroutine variant → `TTask<void>` |
| `PlayPresetAnimation()` | Play a named animation preset |
| `RegisterAnimationPresets()` | Register named presets |
| `ShouldPlayAnimations()` | Performance-based check |

Internally dispatches to fade, scale, or translation animation coroutines using CoreTween.

### UViewModelManagerSubsystem

**Scope:** World

Manages ViewModel instances. See [MVVM / ViewModels](12-MVVM.md).

---

## Game Flow System

The game flow system manages high-level state transitions (MainMenu → Loading → InGame → Paused → GameOver).

### Components

1. **`UCoreGameFlowConfigDataAsset`** — Data asset defining:
   - `InitialState` (FName)
   - `States` (array of `FCoreGameFlowStateDefinition` — name + class)
   - `Transitions` (array of `FCoreGameFlowTransitionDefinition` — from + to)

2. **`UCoreGameFlowState`** — Abstract, Blueprintable UObject:
   - `OnEnter()` — BlueprintNativeEvent
   - `OnExit()` — BlueprintNativeEvent
   - `OnTick(DeltaTime)` — BlueprintNativeEvent
   - `StateName` — Display name

3. **`UCoreGameFlowSubsystem`** — The state machine:
   - `SetConfig()` — Load a config data asset and rebuild the transition graph
   - `RequestTransition(TargetState)` — Request a state change (validates against graph)
   - `TransitionTask(TargetState)` — `co_await` variant
   - `GetCurrentStateName()` / `GetCurrentState()` — Query
   - `IsTransitionValid()` — Check without executing

**Delegates:**
- `OnStateEntered(FName StateName)`
- `OnStateExited(FName StateName)`
- `OnTransitionStarted(FName FromState, FName ToState)`
- `OnTransitionCompleted(FName FromState, FName ToState)`

```cpp
// Setup
UCoreGameFlowSubsystem* Flow = GetGameInstance()->GetSubsystem<UCoreGameFlowSubsystem>();
Flow->SetConfig(MyConfig);

// Transition
bool bOk = Flow->RequestTransition(TEXT("InGame"));

// Async transition
bool bOk = co_await Flow->TransitionTask(TEXT("Paused"));
```

