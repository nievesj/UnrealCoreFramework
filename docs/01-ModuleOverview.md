# Module Overview

Unreal Core Framework is structured as a single UE5 plugin containing five runtime modules. Each module is self-contained with its own `Build.cs`, Public/Private source directories, and API macro.

---

## Module Dependency Graph

```
CoreTween ──────────────────────────────┐
    depends on: Core, UMG, AsyncFlow    │
                                        │
CoreInteraction                         │
    depends on: Core, CoreUObject,      │
                Engine                  │
                                        │
CoreSpawning                            │
    depends on: Core, CoreUObject,      │
                Engine                  │
                                        │
CoreSave ───────────────────────────────┤
    depends on: Core, CoreUObject,      │
                Engine, AsyncFlow,      │
                UnrealCoreFramework     │
                                        │
UnrealCoreFramework ◄───────────────────┘
    depends on: Core, CoreUObject,
                ApplicationCore, Engine,
                HTTP, InputCore, Slate,
                UMG, CommonUI, Json,
                JsonUtilities,
                EnhancedInput,
                ModelViewViewModel,
                FieldNotification,
                AsyncFlow, CoreTween
```

## UnrealCoreFramework (Main Module)

**API Macro:** `UNREALCOREFRAMEWORK_API`

The central module providing:
- Gameplay base classes (Actor, Character, GameMode, GameState, PlayerController, PlayerState, Pawn, GameInstance, GameSession, WorldSettings, SpectatorPawn, ViewportClient)
- CommonUI-based UI stack (Widget, Page, Modal, HUD, MainUiContainer, StackWidgetContainer, InputActionButton)
- Subsystem base classes and concrete subsystems (Level Manager, Data Asset Manager, Game Flow, UI, Input, Audio, Notification, Tween Manager, ViewModel Manager)
- Health component with ViewModel integration
- Enhanced Input wrappers
- Async HTTP requests via `URestRequest`
- Data asset base classes and damage type system
- MVVM ViewModel infrastructure
- Developer Settings for project-wide configuration
- Utility function libraries (`UHelperStatics`, `USubsystemHelper`)
- Third-person character with spring-arm camera setup
- AsyncFlow conventions and macros (`UCF_ASYNC_CONTRACT`)
- Blueprint async actions (Delay, Widget Transition)

## CoreTween

**API Macro:** `CORETWEEN_API`

Standalone tweening engine for UMG widgets:
- `UCoreTween` — Static entry point: `Create()`, `Clear()`, `GetIsTweening()`, `CompleteAll()`
- `FCoreTweenBuilder` — Fluent builder with chained property setters, calls `Run()` to launch a `TTask<void>`
- `FCoreTweenSequence` — Sequential tween chains
- `FCoreTweenEasing` — 30+ easing functions (Quad, Cubic, Quart, Quint, Expo, Sine, Circ, Back, Elastic, Bounce, Spring)
- `TCoreTweenProp<T>` / `TCoreTweenInstantProp<T>` — Property interpolation templates
- `UCoreTweenWorldSubsystem` — Per-world tween state isolation (PIE-safe)
- `ICoreTweenTarget` — Abstraction for target types (Widget, Actor, SceneComponent)
- Full Blueprint support via `UCoreTweenBlueprintFunctionLibrary` and `UCoreTweenParamChain`

## CoreInteraction

**API Macro:** `COREINTERACTION_API`

Component-based interaction system:
- `IInteractable` — Blueprint-native interface with `Interact()`, `CanInteract()`, `GetInteractionPrompt()`
- `UCoreInteractionComponent` — Placed on interactable actors; configurable prompt, range, cooldown, enabled state
- `UCoreInteractorComponent` — Placed on the player; sphere overlap detection, focus management, `TryInteract()`

## CoreSpawning

**API Macro:** `CORESPAWNING_API`

Object pooling and data-driven spawner system:
- `IPoolable` — Interface for pool lifecycle (`OnAcquired`, `OnReleased`, `ResetToPool`)
- `APoolableActor` — Default implementation that hides/disables on release
- `FObjectPoolBase` / `TObjectPool<T>` — Type-safe actor pools with pre-warming
- `UObjectPoolSubsystem` — World subsystem managing named pools
- `ASpawner` — Timer-based spawner driven by `USpawnerConfigDataAsset`
- `ASpawnerVolume` — Spawner that picks random points within a box volume
- `USpawnerFactory` — Centralizes pool creation and config resolution

## CoreSave

**API Macro:** `CORESAVE_API`

Save/load system:
- `UCoreFrameworkSaveGame` — Generic save object with string key-value and binary data maps
- `UCoreSaveSubsystem` — Game instance subsystem managing slot-based save/load/delete operations

---

## Loading Phase

All modules load at `Default` phase. The plugin descriptor specifies `"LoadingPhase": "Default"` for every module.

## Build Configuration

All modules use `PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs`.

