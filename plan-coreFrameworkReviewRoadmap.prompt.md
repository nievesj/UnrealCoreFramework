# Plan: Unreal Core Framework — Full Review Roadmap Implementation

This plan addresses every item in the CoreFramework_Review.md across all three phases: bug fixes, planned feature completion, new rapid-prototyping systems, and polish/extension features. All async code uses `AsyncFlow::TTask<T>` with `UCF_ASYNC_CONTRACT` and `CO_CONTRACT` conventions established in `CoreAsyncTypes.h`.

---

## Module Architecture

### Current State

```
Source/
  CoreTween/               → Core, UMG, AsyncFlow
  UnrealCoreFramework/     → Core, CoreUObject, Engine, ..., AsyncFlow, CoreTween
```

CoreTween is standalone — no dependency on UnrealCoreFramework. The main module depends on CoreTween and provides the `UTweenManagerSubsystem` integration layer. This pattern is the template for new modules.

### Target State — Three New Modules

```
Source/
  CoreTween/               → Core, UMG, AsyncFlow                    (existing, unchanged)
  CoreSpawning/            → Core, CoreUObject, Engine               (new)
  CoreInteraction/         → Core, CoreUObject, Engine               (new)
  CoreSave/                → Core, CoreUObject, Engine, AsyncFlow    (new)
  UnrealCoreFramework/     → ..., CoreTween, CoreSpawning, CoreInteraction, CoreSave
```

Each new module is **standalone** — no dependency on UnrealCoreFramework or each other. They use **UE native base classes only** (`UWorldSubsystem`, `UGameInstanceSubsystem`, `UActorComponent`, `USaveGame`) — never the UCF base classes (`UCoreWorldSubsystem`, `UCoreGameInstanceSubsystem`, etc.). UnrealCoreFramework adds them as public dependencies and provides optional integration wrappers (subsystem helpers, data-asset hookups, Blueprint libraries) exactly like it does for CoreTween today.

This means:
- Any module can be used in isolation outside this plugin
- UnrealCoreFramework acts as the integration hub
- Compile-time impact is contained per module
- The `.uplugin` registers all five modules

### Module Details

#### CoreSpawning

| Property | Value |
|---|---|
| Directory | `Source/CoreSpawning/` |
| API macro | `CORESPAWNING_API` |
| Dependencies (Public) | `Core`, `CoreUObject`, `Engine` |
| Dependencies (Private) | — |
| Contains | `FObjectPoolBase`, `TObjectPool<T>`, `UObjectPoolSubsystem` (extends **`UWorldSubsystem`**), `IPoolable`, `APoolableActor`, `ASpawner`, `ASpawnerVolume`, `USpawnerFactory`, `USpawnerConfigDataAsset` |

No AsyncFlow dependency — the pool/spawner system is synchronous. Spawn cadence uses `FTimerHandle`, not coroutines. If async prewarm is needed later, `AsyncFlow` can be added as an optional private dependency.

**Type Erasure Design:** `FObjectPoolBase` is a non-template base class with virtual `ReleaseUntyped(AActor*)`, `AcquireUntyped() → AActor*`, `ReleaseAll()`, and `GetActiveCount()/GetInactiveCount()`. `TObjectPool<T>` inherits from `FObjectPoolBase` and adds type-safe `Acquire() → T*` and `Release(T*)`. The subsystem stores `TMap<FName, TUniquePtr<FObjectPoolBase>>`. The template helper `AcquireFromPool<T>(FName)` does `static_cast<T*>` internally after calling `AcquireUntyped()`. Blueprint callers use the `FName`-keyed untyped API (`AcquireActorFromPool`, `ReleaseActorToPool`).

#### CoreInteraction

| Property | Value |
|---|---|
| Directory | `Source/CoreInteraction/` |
| API macro | `COREINTERACTION_API` |
| Dependencies (Public) | `Core`, `CoreUObject`, `Engine` |
| Dependencies (Private) | — |
| Contains | `IInteractable`, `UCoreInteractionComponent` (extends **`UActorComponent`**), `UCoreInteractorComponent` (extends **`UActorComponent`**) |

Pure gameplay system with no async requirements. Detection uses sphere overlap / line trace. All synchronous.

#### CoreSave

| Property | Value |
|---|---|
| Directory | `Source/CoreSave/` |
| API macro | `CORESAVE_API` |
| Dependencies (Public) | `Core`, `CoreUObject`, `Engine`, `AsyncFlow` |
| Dependencies (Private) | — |
| Contains | `UCoreSaveGame` (extends **`USaveGame`**), `UCoreSaveSubsystem` (extends **`UGameInstanceSubsystem`**) |

Depends on AsyncFlow for `co_await AsyncFlow::AsyncSaveGame()` and `co_await AsyncFlow::AsyncLoadGame()` from `AsyncFlowSaveGameAwaiters.h`. These awaiters are confirmed fully implemented:
- `AsyncSaveGame(USaveGame*, FString, int32)` → `co_await` yields `bool`
- `AsyncLoadGame(FString, int32)` → `co_await` yields `USaveGame*`
- Both use `TSharedPtr<bool> AliveFlag` pattern for lifetime safety

### .uplugin Changes

Register all five modules (2 existing + 3 new):

```json
"Modules": [
    { "Name": "UnrealCoreFramework", "Type": "Runtime", "LoadingPhase": "Default" },
    { "Name": "CoreTween",           "Type": "Runtime", "LoadingPhase": "Default" },
    { "Name": "CoreSpawning",        "Type": "Runtime", "LoadingPhase": "Default" },
    { "Name": "CoreInteraction",     "Type": "Runtime", "LoadingPhase": "Default" },
    { "Name": "CoreSave",            "Type": "Runtime", "LoadingPhase": "Default" }
]
```

**Note:** The `Plugins` array already declares `AsyncFlow` as a plugin dependency. Since `CoreSave` depends on `AsyncFlow` at the module level and lives within this plugin, the plugin-level dependency is already satisfied. If `CoreSpawning` or `CoreInteraction` are ever extracted to separate plugins in the future, they would need their own `.uplugin` with explicit plugin dependency declarations.

### UnrealCoreFramework.Build.cs Changes

Add new modules to `PublicDependencyModuleNames`:

```csharp
"CoreSpawning",
"CoreInteraction",
"CoreSave"
```

Add `"GameplayTags"` to `PublicDependencyModuleNames` — needed for the Audio Manager (`PlaySound(FGameplayTag)`).

### Per-Module File Structure (following CoreTween pattern)

Each new module follows this layout:

```
Source/CoreXxx/
  CoreXxx.Build.cs
  Public/
    CoreXxx.h                  // Module API header (minimal, includes key types)
    [class headers]
  Private/
    CoreXxxModule.h            // IModuleInterface subclass
    CoreXxxModule.cpp          // IMPLEMENT_MODULE macro
    [class implementations]
```

### Error Handling Conventions

All `AsyncFlow::TTask<T>` methods across the framework follow these conventions:

1. **Failable operations return `TTask<bool>` or a nullable type** (`TTask<USaveGame*>` where `nullptr` = failure). Void tasks (`TTask<void>`) only for operations that cannot meaningfully fail (animation playback, delays).
2. **Log at Warning level on failure.** Every failed operation logs via the owning subsystem's log category with context (slot name, asset ID, state name, etc.).
3. **Fire an error delegate when one exists.** Subsystems that expose `OnError`-style delegates broadcast on failure so Blueprint users get notified.
4. **Never retry automatically.** The caller decides retry policy. The framework reports failure and returns control.
5. **`co_return` a failure value — never throw.** Coroutines that detect an error condition `co_return false` (or `co_return nullptr`) after logging. No C++ exceptions.

---

## Phase 1 — Bug Fixes & Planned Feature Completion

> **Execution notes:** Steps 1, 2, 3 are independent bug fixes — execute in any order. Step 4 is standalone. Steps 5 → 6 → 7 → 8 are serial (each depends on the previous).

### Step 1: Fix `UCoreHealthComponent::Heal()` specifier
Change `BlueprintPure` to `BlueprintCallable` on line 59 of `CoreHealthComponent.h`. `Heal()` mutates `CurrentHealth` — `BlueprintPure` is incorrect.

### Step 2: Fix `GENERATED_BODY()` placement in `UCorePage`
In `CorePage.h`, move `GENERATED_BODY()` from line 21 (inside `private:`) to immediately after the opening brace of the class body (before any access specifier), which is the UE convention.

### Step 3: Remove `DisablePlayerControllerInput` from `IPageableWidgetInterface`
Delete the raw `bool DisablePlayerControllerInput` member on line 25 of `PageableWidgetInterface.h`. `UCorePage` already owns this concern via its `DisablePlayerInput` UPROPERTY.

### Step 4: Uncomment and wire UMG WidgetAnimation path

**Constraint:** `UWidgetAnimation*` cannot be a UPROPERTY inside a `USTRUCT`. The animation references must live on the `UCoreWidget` class directly, using `meta=(BindWidgetAnimOptional)`. This means intro/outro animations are **per-Widget-Blueprint-class** (resolved by name in the WBP editor), not per-instance configurable. This is the standard UMG pattern and is appropriate for a framework.

Implementation:

1. In `UiCoreFrameworkTypes.h`, **remove** the four commented-out `WidgetAnimation*` / `FWidgetAnimationOptions` lines from `FCoreWidgetAnimationSettings` (lines 159–169). They will never live here.
2. In `CoreWidget.h`, add four class-level UPROPERTYs:
   ```cpp
   UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
   TObjectPtr<UWidgetAnimation> IntroAnimation;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoreWidget|Animation")
   FWidgetAnimationOptions IntroAnimationOptions;

   UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
   TObjectPtr<UWidgetAnimation> OutroAnimation;

   UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CoreWidget|Animation")
   FWidgetAnimationOptions OutroAnimationOptions;
   ```
3. In `UCoreWidget::Show()`/`Hide()`, change the `EWidgetAnimationType::WidgetAnimation` case to call `PlayWidgetAnimation()` with the appropriate animation and options, then fire `InternalShown()`/`InternalHidden()` via the `OnAnimationFinished` delegate.
4. In `UCoreWidget::ShowTask()`/`HideTask()`, add a branch for `EWidgetAnimationType::WidgetAnimation`. Use `PlayAnimation()` + bind `FWidgetAnimationDynamicEvent` to a lambda that completes a `TSharedPtr<AsyncFlow::FAsyncFlowState>` or use a one-shot delegate awaiter (`AsyncFlow::FDelegateAwaiter`). The coroutine `co_await`s until the animation finishes, then calls `InternalShown()`/`InternalHidden()`.

### Step 5: Create `CoreSpawning` module and implement Object Pooling System

#### 5a: Scaffold the module
Create `Source/CoreSpawning/` with `CoreSpawning.Build.cs`, module boilerplate (`CoreSpawningModule.h/.cpp`), and `Public/`/`Private/` directories.

#### 5b: Implement core pooling classes

All classes use **UE native base classes** — no UCF dependencies.

| New File | Class | Description |
|---|---|---|
| `Public/IPoolable.h` | `IPoolable` | UInterface with `OnAcquired()`, `OnReleased()`, `ResetToPool()` virtuals. |
| `Public/PoolableActor.h` | `APoolableActor` | `AActor` + `IPoolable`. Default implementations: disable actor, stop movement, hide on release; enable, show on acquire. |
| `Private/PoolableActor.cpp` | — | Implementation. |
| `Public/ObjectPool.h` | `FObjectPoolBase` + `TObjectPool<T>` | `FObjectPoolBase`: non-template base with `AcquireUntyped() → AActor*`, `ReleaseUntyped(AActor*)`, `PreWarm(int32, UWorld*)`, `ReleaseAll()`, `GetActiveCount()`, `GetInactiveCount()`. `TObjectPool<T>`: template subclass adding type-safe `Acquire() → T*`, `Release(T*)`. Internal storage: `TArray<AActor*>` for inactive, `TSet<AActor*>` for active. Configurable max pool size. |
| `Public/ObjectPoolSubsystem.h` | `UObjectPoolSubsystem` | Extends **`UWorldSubsystem`** (not UCoreWorldSubsystem). Named pool registry: `CreatePool<T>(FName, int32 PreWarmCount)`, `AcquireFromPool<T>(FName) → T*`, `ReleaseToPool(FName, AActor*)`. Blueprint API via `FName` keys: `CreateActorPool(FName, TSubclassOf<AActor>, int32)`, `AcquireActorFromPool(FName) → AActor*`, `ReleaseActorToPool(FName, AActor*)`. Internal storage: `TMap<FName, TUniquePtr<FObjectPoolBase>>`. Cleanup in `Deinitialize()`. |
| `Private/ObjectPoolSubsystem.cpp` | — | Implementation. |

#### 5c: Migrate existing spawning stubs
Move `SpawningSubsystem.h/.cpp` and `Spawner.h/.cpp` out of `UnrealCoreFramework/Public/SubSystems/World/Spawning/` and into `CoreSpawning/`. These get rewritten in Steps 6–8. Remove the old files and their `Private/` counterparts from UnrealCoreFramework.

### Step 6: Implement configurable `ASpawner`
Rewrite the stub `Spawner.h/.cpp` (now in `CoreSpawning`) with:
- `TSubclassOf<AActor> SpawnClass`, spawn rate (timer-driven, not Tick), max alive count, spawn transform rules.
- Pool integration: optionally acquire from `UObjectPoolSubsystem` instead of `SpawnActor`.
- Data-asset-driven config via a new `USpawnerConfigDataAsset`.
- Disable Tick (currently enabled). Use `FTimerHandle` for spawn cadence.

### Step 7: Implement `ASpawnerVolume`
New `ASpawnerVolume` actor with a `UBoxComponent` or `USphereComponent` defining the spawn region. Picks random points within the volume (using `FMath::RandPointInBox` / `UKismetMathLibrary::RandomPointInBoundingBox`). Inherits or composes `ASpawner` logic for spawn rate, pool integration, and data-asset config.

### Step 8: Implement `USpawnerFactory`
New `USpawnerFactory` UObject that resolves spawn parameters from `USpawnerConfigDataAsset` — actor class, pool name, spawn interval, max count, volume extents. Used by both `ASpawner` and `ASpawnerVolume` to centralize config resolution.

---

## Phase 2 — High-Value Rapid Prototyping Systems

### Step 9: Game Flow State Machine
New subsystem folder in UnrealCoreFramework: `Public/SubSystems/Game/GameFlow/`.

Stays in the main module because it integrates with UI (show/hide menus), Audio Manager, and Level Manager — all of which live in UnrealCoreFramework.

| Class | Base | Role |
|---|---|---|
| `UCoreGameFlowState` | `UObject` | Represents a named state (MainMenu, Loading, InGame, Paused, GameOver). Has `OnEnter()`, `OnExit()`, `OnTick()` virtuals. |
| `UCoreGameFlowTransition` | `UObject` | Defines valid from→to state pairs with optional `bool CanTransition()` condition. |
| `UCoreGameFlowConfigDataAsset` | `UDataAsset` | Data asset that defines the full transition table: array of state definitions (FName + TSubclassOf<UCoreGameFlowState>) and array of valid transitions (from FName → to FName + optional condition class). Loaded and registered by the subsystem at Initialize(). |
| `UCoreGameFlowSubsystem` | `UCoreGameInstanceSubsystem` | Manages current state, transition table, `RequestTransition(FName)`. Fires delegates `OnStateEntered` / `OnStateExited`. Async transitions via `AsyncFlow::TTask<bool> TransitionTask()`. |

#### Design Decisions

**Transition table ownership:** The transition table is defined in a `UCoreGameFlowConfigDataAsset` data asset. The subsystem loads this asset (configured via `UUnrealCoreFrameworkSettings`) during `Initialize()` and builds the internal transition graph. This keeps the state machine data-driven and avoids hardcoding transition logic in C++.

**State behavior registration:** State-specific behaviors (show HUD, crossfade music, load level) are wired via **delegates**, not hardcoded in the state UObject. `UCoreGameFlowSubsystem` exposes:
- `OnStateEntered(FName StateName)` — broadcast after entering a state
- `OnStateExited(FName StateName)` — broadcast before exiting a state
- `OnTransitionStarted(FName From, FName To)` — broadcast when a transition begins
- `OnTransitionCompleted(FName From, FName To)` — broadcast when a transition completes

Game-specific code (GameMode, PlayerController, or project-specific subsystems) binds to these delegates and calls `UISubsystem->ShowHUD()`, `AudioSubsystem->CrossfadeMusic()`, etc. The state machine itself has **zero direct coupling** to UI, audio, or level systems. It only fires events.

Individual `UCoreGameFlowState` subclasses can override `OnEnter()`/`OnExit()` for state-specific logic that is inherent to the state itself (e.g., pausing the game timer on Paused enter). Cross-system orchestration goes through delegates.

**World-scoped subsystem unavailability during level transitions:** The `UCoreGameFlowSubsystem` is a `UCoreGameInstanceSubsystem` and persists across level loads. During transitions:
1. The subsystem sets an internal `bIsTransitioning` flag.
2. `OnStateExited` fires before the old world tears down — listeners on world subsystems must complete their work synchronously or guard against the world going away.
3. `OnStateEntered` fires after the new world is ready (post `OnWorldBeginPlay`). The subsystem defers the enter-state delegate broadcast via `GetWorld()->OnWorldBeginPlay` binding, or via a one-frame delay (`FTimerHandle`), to ensure world subsystems are initialized.
4. If `RequestTransition()` is called while `bIsTransitioning` is true, it queues the request and processes it after the current transition completes.

### Step 10: Create `CoreInteraction` module and implement Interaction System

#### 10a: Scaffold the module
Create `Source/CoreInteraction/` with `CoreInteraction.Build.cs`, module boilerplate, and `Public/`/`Private/` directories.

#### 10b: Implement interaction classes

All classes use **UE native base classes** — no UCF dependencies.

| New File | Class | Role |
|---|---|---|
| `Public/IInteractable.h` | `IInteractable` | UInterface: `Execute_Interact()`, `GetInteractionPrompt()`, `CanInteract()`. |
| `Public/CoreInteractionComponent.h` | `UCoreInteractionComponent` | Extends **`UActorComponent`**. Placed on interactable objects. Implements `IInteractable`. Exposes type, prompt text, range, cooldown, enabled state. |
| `Private/CoreInteractionComponent.cpp` | — | Implementation. |
| `Public/CoreInteractorComponent.h` | `UCoreInteractorComponent` | Extends **`UActorComponent`**. Placed on player. Sphere overlap or line-trace detection for nearby `IInteractable` actors. Manages focus/highlight (outline or material param), triggers `Interact()` on input. Blueprint events: `OnInteractableFound`, `OnInteractableLost`, `OnInteracted`. |
| `Private/CoreInteractorComponent.cpp` | — | Implementation. |

### Step 11: Create `CoreSave` module and implement Save/Load System

#### 11a: Scaffold the module
Create `Source/CoreSave/` with `CoreSave.Build.cs`, module boilerplate, and `Public/`/`Private/` directories.

#### 11b: Implement save/load classes

All classes use **UE native base classes** — no UCF dependencies.

| New File | Class | Role |
|---|---|---|
| `Public/CoreSaveGame.h` | `UCoreSaveGame` | Extends **`USaveGame`**. Adds version int, slot name, timestamp, `TMap<FString, FString>` generic key-value store, and `SerializeActor(AActor*)` / `DeserializeActor()` helpers. |
| `Private/CoreSaveGame.cpp` | — | Implementation. |
| `Public/CoreSaveSubsystem.h` | `UCoreSaveSubsystem` | Extends **`UGameInstanceSubsystem`** (not UCoreGameInstanceSubsystem). `SaveToSlot(FString)` / `LoadFromSlot(FString)` with sync and async (`AsyncFlow::TTask<bool>`) paths. Slot enumeration, delete, auto-save timer. On failure: logs at Warning, fires `OnSaveError`/`OnLoadError` delegate, `co_return false`. |
| `Private/CoreSaveSubsystem.cpp` | — | Implementation. Uses `co_await AsyncFlow::AsyncSaveGame()` and `co_await AsyncFlow::AsyncLoadGame()` from `AsyncFlowSaveGameAwaiters.h`. |

**AsyncFlow dependency confirmed.** `AsyncFlowSaveGameAwaiters.h` provides both required awaiters:
- `AsyncFlow::AsyncSaveGame(USaveGame*, FString, int32)` → `co_await` yields `bool` (success/failure)
- `AsyncFlow::AsyncLoadGame(FString, int32)` → `co_await` yields `USaveGame*` (nullptr on failure)
- Both use `TSharedPtr<bool> AliveFlag` + `TWeakPtr` guard for safe lifetime management
- Wraps `UGameplayStatics::AsyncSaveGameToSlot` / `AsyncLoadGameFromSlot` internally

### Step 12: `UCoreModal` dialog subclass
Stays in UnrealCoreFramework (depends on UI system, CoreTween, CommonUI).

New `CoreModal.h/.cpp` extending `UCorePage`:
- Background dimmer overlay (semi-transparent `UImage` filling viewport).
- Focus trap (override `NativeOnActivated` to set focus to first button).
- Standardized accept/cancel button bindings via `UCoreInputActionButton`.
- `AsyncFlow::TTask<bool> PromptTask()` that resolves `true` for accept, `false` for cancel — replaces manual delegate wiring.
- Register in `UnrealCoreFrameworkSettings` alongside the existing `YesNoPrompt`.

**Task ownership:** `PromptTask()` is a coroutine that lives as long as the modal is active. The **caller** (e.g., a page, subsystem, or GameFlowState) must store the returned `TTask<bool>` to keep the coroutine alive. The modal signals completion (sets the result to true/false) **before** triggering its own deactivation/destruction. The coroutine `co_return`s the result, and the caller's `co_await` resumes on the caller's side. The modal is then safe to destroy because the coroutine frame has already completed.

### Step 13: Audio Manager — flesh out `UAudioSubsystem`
Stays in UnrealCoreFramework (integrates with CoreTween for crossfade, uses framework settings/data-assets).

Rewrite the stub `AudioSubsystem.h/.cpp`:
- Change base to `UCoreGameInstanceSubsystem` (extends `UGameInstanceSubsystem`). Audio state persists across level loads. World access for `UAudioComponent` spawning is resolved via `GetGameInstance()->GetWorld()` with a validity check — if no world is available (during level transition), audio operations are queued and flushed when the next world is ready (bind to `FWorldDelegates::OnPostWorldInitialization`).
- `PlaySound(FGameplayTag SoundTag)` — lookup from a `TMap<FGameplayTag, USoundBase*>` populated from a `UAudioRegistryDataAsset`.
- `CrossfadeMusic(USoundBase* NewTrack, float Duration)` — uses CoreTween (`FCoreTweenBuilder`) to fade volume of active `UAudioComponent`, then swap track. Returns `AsyncFlow::TTask<void>`.
- `SetMixPreset(FName Preset)` — applies `USoundMix` from a data-asset registry.
- Internal `UAudioComponent` pool (reuse instead of spawning per call). Pre-warm configurable count.

**Requires** `"GameplayTags"` added to `UnrealCoreFramework.Build.cs` public dependencies.

### Step 14: Migrate `ACoreThirdPersonCharacter` to Enhanced Input
Replace the legacy axis bindings (`MoveForward`, `TurnAtRate`, `LookUpAtRate`, etc.) in `CoreThirdPersonCharacter.h/.cpp` with `UInputAction` / `UInputMappingContext` bindings using `UCoreEnhancedInputComponent`. Remove `TurnRateGamepad` and legacy touch handlers.

**Input Action/Mapping Context assets — programmatic creation:** Instead of relying on binary `.uasset` files, create the default `UInputAction` (`IA_Move`, `IA_Look`, `IA_Jump`) and `UInputMappingContext` (`IMC_ThirdPerson`) objects programmatically via `NewObject<UInputAction>()` and `NewObject<UInputMappingContext>()` in the character's constructor or a static initializer. Configure their `ValueType` (Axis2D for Move/Look, Digital for Jump) and key mappings in C++. Expose them as `UPROPERTY(EditDefaultsOnly)` so they can be overridden with editor-created assets in subclasses.

Fallback: if programmatic creation proves unreliable (some input trigger/modifier combinations require serialized asset data), note this in implementation and create the assets manually in-editor as a backup path.

---

## Phase 3 — Polish & Extended Features

### Step 15: Camera Management System
Stays in UnrealCoreFramework (integrates with CoreTween for FOV/offset interpolation).

New folder `Public/Camera/`:
- `UCoreCameraComponent` — extends `UCameraComponent`. Built-in `ShakeCamera(float Duration, float Magnitude)`, FOV tween via CoreTween `AsyncFlow::TTask<void>`, offset interpolation.
- `UCoreCameraModifier` — base `UCameraModifier` subclass for screen shake, DOF transitions, post-process blending.
- `UCoreCameraManagerSubsystem` — `UCoreLocalPlayerSubsystem`. Manages named camera modes (ThirdPerson, Isometric, Cinematic) with `AsyncFlow::TTask<void> TransitionToMode(FName)` blending between configurations.

### Step 16: Debug/Cheat Console System
Stays in UnrealCoreFramework (integrates with Game Flow, UI system).

New folder `Public/Debug/`:
- `UCoreCheatManager` — extends `UCheatManager`. Auto-registers framework cheats (god mode, free camera, spawn actor, set game flow state).
- `UCoreDebugSubsystem` — `UCoreGameInstanceSubsystem`. Cheat command registration via `RegisterCheat(FName, FDelegate)`. Toggle debug overlay.
- `UCoreDebugWidget` — on-screen overlay showing FPS, draw calls, memory, and custom game stats via `STAT` integration. Built on `UCoreWidget` with CoreTween fade-in.

### Step 17: Toast/Notification System
Stays in UnrealCoreFramework (depends on UI system, CoreTween).

New folder `Public/UI/Notifications/`:
- `UCoreNotificationSubsystem` — `UCoreLocalPlayerSubsystem`. `ShowToast(FText Message, float Duration)` queues notifications. Max visible count, auto-dismiss timer.
- `UCoreNotificationWidget` — animated toast using CoreTween (slide in from edge, fade out). Positioned via `UCanvasPanel` slot manipulation.

### Step 18: CoreTween Actor/SceneComponent targets
Changes to the `CoreTween` module:

1. Add `FSceneComponentTweenTarget` implementing `ICoreTweenTarget` in a new `Targets/SceneComponentTweenTarget.h`. Maps Location↔Translation, ActorRotation↔Rotation, ActorScale↔Scale. The `ICoreTweenTarget` interface needs extension: add `GetCurrentLocation() → FVector`, `GetCurrentActorRotation() → FRotator`, `ApplyLocation(FVector)`, `ApplyActorRotation(FRotator)` (stubs already noted in comments on lines 46–51 of `CoreTweenTarget.h`). Public header uses **forward declarations only** for `USceneComponent` and `AActor` — no `#include` of Engine headers.
2. Add `FActorTweenTarget` as a convenience wrapper that delegates to the root component's `FSceneComponentTweenTarget`.
3. Update `FCoreTweenBuilder` to accept `USceneComponent*` or `AActor*` in addition to `UWidget*`. Add `FromLocation/ToLocation`, `FromActorRotation/ToActorRotation`, `FromActorScale/ToActorScale` fluent setters.
4. Update `CoreTween.Build.cs` to add `"Engine"` to **private** dependencies (not public — keeps CoreTween's public dependency footprint minimal; consumers only need forward declarations in headers, full includes happen in `.cpp` files).
5. Update `CoreTween.h` (module API header) to include `Targets/SceneComponentTweenTarget.h` and `Targets/ActorTweenTarget.h` so consumers who include the umbrella header get access to the new target types.

### Step 19: CoreTween Sequence builder
Add `FCoreTweenSequence` class to the `CoreTween` module:
- Fluent API: `FCoreTweenSequence::Create().Then(Builder1).Then(Builder2).Then(Builder3).Run(WorldContext) → AsyncFlow::TTask<void>`.
- Internally chains `co_await` calls sequentially.
- Add to Blueprint function library for non-coroutine users.

### Step 20: Blueprint Async Action Classes
Stays in UnrealCoreFramework (bridges all modules).

New folder `Public/Async/Actions/` containing individual `UBlueprintAsyncActionBase` subclasses:

| Class | Wraps |
|---|---|
| `UCoreAsyncAction_LoadDataAsset` | `UDataAssetManagerSubsystem::LoadDataAssetAsyncTask` |
| `UCoreAsyncAction_LoadLevel` | `ULevelManagerSubsystem::LoadLevelTask` |
| `UCoreAsyncAction_HttpRequest` | `URestRequest` coroutine API |
| `UCoreAsyncAction_PlayWidgetTransition` | `UCoreWidget::ShowTask`/`HideTask` |
| `UCoreAsyncAction_Delay` | `AsyncFlow::Delay` awaiters (game-time, real-time, unpaused variants) |

Each action follows the standard pattern: `Activate()` launches an `AsyncFlow::TTask`, completion fires output exec pins via `DECLARE_DYNAMIC_MULTICAST_DELEGATE`. Each class is a separate `.h/.cpp` file pair.

### Step 21: ViewModel auto-discovery for widgets
1. Add `TSubclassOf<UCoreViewModel> ViewModelClass` UPROPERTY to `UCoreWidget`.
2. In **`NativeOnActivated()` only** (not `NativePreConstruct()` — that runs in the editor where subsystems don't exist), if `ViewModelClass` is set, resolve from `UViewModelManagerSubsystem` using the owning player's pawn as the tracked object. Guard with a null check on the subsystem. Store as a `UPROPERTY` and call `OnViewModelBound()`.
3. On `NativeOnDeactivated()` / destruction, release the ViewModel reference. If it's the last consumer (ref-count or check), call `RemoveViewModel()`.

### Step 22: Page history/back-navigation in `UISubsystem`
Add a `TArray<TSubclassOf<UCorePage>> PageHistory` stack to `UUISubsystem`. Push on page open, pop on `GoBack()`. Expose `GoBack()` as `BlueprintCallable`. Pages call `GoBack()` from their exit button handler instead of hardcoding a target page.

### Step 23: Disable Tick on `UCoreEnhancedInputComponent`
Remove the `TickComponent` override from `CoreEnhancedInputComponent.h` and its `.cpp`. Set `PrimaryComponentTick.bCanEverTick = false` in the constructor.

### Step 24: Clean up empty stub base classes
Audit all classes in `Game/Base/`: `ACoreActor`, `ACorePawn`, `ACoreCharacter`, `ACoreActorComponent`, `ACoreSceneComponent`, `ACorePlayerController`, `ACorePlayerState`, `ACoreGameSession`, `ACoreWorldSettings`, `ACoreSpectatorPawn`.

Concrete actions for each:
1. **Remove all empty `Tick` overrides** (`Tick(float DeltaTime)`) and set `PrimaryActorTick.bCanEverTick = false` in the constructor. Same for `TickComponent` on component stubs.
2. **Remove empty `BeginPlay` overrides** that only call `Super::BeginPlay()`.
3. **Remove empty `SetupPlayerInputComponent` overrides** that only call `Super`.
4. **Add `UCLASS(Abstract)`** to all base classes that are not meant to be instantiated directly (all of them — they are framework extension points).
5. **Add a doc comment** to each class: `/** Framework extension point. Subclass for project-specific logic. */`

---

## Intentionally Deferred Items

The following items from `CoreFramework_Review.md` are explicitly out of scope for this plan. They may be addressed in a future roadmap:

- **Health Component shield/armor layer and invulnerability window** (§4.4) — Project-specific combat features beyond framework scope.
- **`UCoreTooltip` widget** (§4.1) — Low priority relative to core systems. Can be added later as a Phase 4 item.
- **Data Asset Manager bulk preload by type and dependency tracking** (§4.5) — `LoadAllDataAssetsOfType` already exists. Dependency tracking is an optimization that can be deferred until asset graphs become complex enough to warrant it.

---

## Resolved Considerations

1. ~~**Module split for new systems?**~~ **Resolved — splitting into three standalone modules:** `CoreSpawning`, `CoreInteraction`, `CoreSave`. Each uses UE native base classes (no dependency on UnrealCoreFramework), following the CoreTween pattern. UnrealCoreFramework adds all three as public dependencies and provides integration wrappers.

2. **GameplayTags dependency.** The Audio Manager (`PlaySound(FGameplayTag)`) needs `"GameplayTags"` added to `UnrealCoreFramework.Build.cs` public dependencies. Tracked in Step 13.

3. ~~**AsyncFlow SaveGame awaiters.**~~ **Resolved — fully implemented.** `AsyncFlowSaveGameAwaiters.h` provides `AsyncFlow::AsyncSaveGame()` → `bool` and `AsyncFlow::AsyncLoadGame()` → `USaveGame*`. Both are `co_await`-able with proper `AliveFlag` lifetime safety. No additional work needed in AsyncFlow.

4. ~~**Audio subsystem base class.**~~ **Resolved — `UCoreGameInstanceSubsystem`.** Audio state persists across level loads. World resolution for `UAudioComponent` spawning handled via `GetGameInstance()->GetWorld()` with queuing during level transitions.

5. ~~**Game Flow transition table.**~~ **Resolved — Data Asset.** `UCoreGameFlowConfigDataAsset` defines states and valid transitions. Registered in `UUnrealCoreFrameworkSettings`. Subsystem loads and builds the graph at `Initialize()`.

6. ~~**Object Pool type erasure.**~~ **Resolved — `FObjectPoolBase` → `TObjectPool<T>`.** Subsystem stores `TMap<FName, TUniquePtr<FObjectPoolBase>>`. Template helpers do `static_cast` internally.

---

## Validation Strategy

Each step should be validated as follows:

| Validation Method | Applies To |
|---|---|
| **`get_errors` (compile check)** | Every step — run after every file edit |
| **Full project build** | After completing each Phase (1, 2, 3) |
| **Runtime smoke test** | Steps requiring runtime behavior: Object Pool (5–8), Game Flow (9), Interaction (10), Save/Load (11), Modal (12), Audio (13), Enhanced Input (14), Camera (15), Notifications (17), CoreTween targets (18–19) |

### Smoke Test Checklist (post-Phase 2)
- [ ] Create a pool, prewarm 5 actors, acquire 3, release 2, acquire 2 more — verify counts
- [ ] Save game to slot, load from slot, verify round-trip data integrity
- [ ] Transition Game Flow: MainMenu → Loading → InGame → Paused → InGame → GameOver — verify delegates fire
- [ ] Place interactable actor, approach with interactor, verify prompt and interaction
- [ ] Open modal, accept — verify `PromptTask()` returns true. Open modal, cancel — verify false.
- [ ] Play sound by tag, crossfade music — verify audio output
- [ ] Third person character: move, look, jump — verify Enhanced Input bindings

---

## Tracking

```
Phase 1 — Bug Fixes & Planned Feature Completion
- [ ] Step 1: Fix UCoreHealthComponent::Heal() → BlueprintCallable
- [ ] Step 2: Fix GENERATED_BODY() placement in UCorePage
- [ ] Step 3: Remove DisablePlayerControllerInput from IPageableWidgetInterface
- [ ] Step 4: Uncomment and wire UMG WidgetAnimation path (including ShowTask/HideTask async paths)
- [ ] Step 5: Create CoreSpawning module + implement Object Pool (FObjectPoolBase, TObjectPool<T>, UObjectPoolSubsystem, IPoolable, APoolableActor)
- [ ] Step 6: Implement ASpawner — configurable spawn logic with pool integration (in CoreSpawning)
- [ ] Step 7: Implement ASpawnerVolume — volume-based random spawn locations (in CoreSpawning)
- [ ] Step 8: Implement USpawnerFactory — data-asset-driven spawn configuration (in CoreSpawning)

Phase 2 — High-Value Rapid Prototyping Systems
- [ ] Step 9: Implement Game Flow State Machine (UCoreGameFlowSubsystem + UCoreGameFlowConfigDataAsset) (in UnrealCoreFramework)
- [ ] Step 10: Create CoreInteraction module + implement Interaction System (IInteractable, UCoreInteractionComponent, UCoreInteractorComponent)
- [ ] Step 11: Create CoreSave module + implement Save/Load System (UCoreSaveGame, UCoreSaveSubsystem)
- [ ] Step 12: Implement UCoreModal dialog subclass (in UnrealCoreFramework)
- [ ] Step 13: Implement Audio Manager — flesh out UAudioSubsystem as UCoreGameInstanceSubsystem + add GameplayTags dependency (in UnrealCoreFramework)
- [ ] Step 14: Migrate ACoreThirdPersonCharacter to Enhanced Input with programmatic input asset creation (in UnrealCoreFramework)

Phase 3 — Polish & Extended Features
- [ ] Step 15: Implement Camera Management System (in UnrealCoreFramework)
- [ ] Step 16: Implement Debug/Cheat Console System (in UnrealCoreFramework)
- [ ] Step 17: Implement Toast/Notification System (in UnrealCoreFramework)
- [ ] Step 18: Implement CoreTween Actor/SceneComponent targets — Engine as private dep (in CoreTween)
- [ ] Step 19: Implement CoreTween Sequence builder (in CoreTween)
- [ ] Step 20: Implement Blueprint Async Action classes (individual UCoreAsyncAction_* classes) (in UnrealCoreFramework)
- [ ] Step 21: Implement ViewModel auto-discovery for widgets — NativeOnActivated only (in UnrealCoreFramework)
- [ ] Step 22: Implement Page history/back-navigation in UISubsystem (in UnrealCoreFramework)
- [ ] Step 23: Disable Tick on UCoreEnhancedInputComponent (in UnrealCoreFramework)
- [ ] Step 24: Clean up empty stub base classes — remove Tick, add Abstract, add doc comments (in UnrealCoreFramework)
```
