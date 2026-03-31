# Unreal Core Framework

A modular Unreal Engine 5 plugin providing basic game systems to build upon: gameplay framework base classes, a coroutine-driven tweening engine, a CommonUI-based UI stack, MVVM view models, object pooling, an interaction system, save/load management, and async-first APIs powered by [AsyncFlow](https://github.com/nievesj/AsyncFlow).

**Author:** José M. Nieves  
**License:** MIT  
**Engine:** Unreal Engine 5.3+  
**Status:** Beta / Experimental

---

## Modules

| Module | Description |
|---|---|
| **UnrealCoreFramework** | Core module — gameplay base classes, UI stack, subsystems, input, HTTP, data assets, health, MVVM, tweening integration, settings, and utilities. |
| **CoreTween** | Coroutine-based tweening engine for UMG widgets. Fluent builder API, 30+ easing functions, per-world subsystem isolation, sequences, Blueprint support. |
| **CoreInteraction** | Component-based interaction system with `IInteractable` interface, proximity detection, cooldowns, and Blueprint events. |
| **CoreSpawning** | Object pooling and data-driven spawner system with volume-based random spawning and pool pre-warming. |
| **CoreSave** | Save/load subsystem with generic key-value storage, binary serialization, and slot management. |

## Plugin Dependencies

- **EnhancedInput** — Data-driven input binding
- **CommonUI** — Activatable widget stack and viewport client
- **AsyncFlow** — C++20 coroutine task system (`TTask<T>`, `co_await`)
- **ModelViewViewModel** — MVVM base classes for UI data binding
- **OnlineSubsystemUtils** — Online session utilities

---

## Quick Start

### 1. Enable the Plugin

Add to your `.uproject`:

```json
{
  "Name": "UnrealCoreFramework",
  "Enabled": true
}
```

### 2. Add Module Dependencies

In your game module's `.Build.cs`:

```csharp
PublicDependencyModuleNames.AddRange(new string[]
{
    "UnrealCoreFramework",
    "CoreTween",
    "CoreInteraction",
    "CoreSpawning",
    "CoreSave"
});
```

### 3. Set Up the Game Mode

Subclass the Core Framework base classes:

```cpp
// MyGameMode.h
#pragma once
#include "Game/Base/CoreGameMode.h"
#include "MyGameMode.generated.h"

UCLASS()
class AMyGameMode : public ACoreGameMode
{
    GENERATED_BODY()
};
```

### 4. Configure Project Settings

Open **Project Settings → Unreal Core Framework** to assign:
- Main Menu Page class
- Main HUD Page class
- Pause Menu Page class
- Main UI Container class
- Prompt widget class

---

## Usage Examples

### Health Component

```cpp
// Add UCoreHealthComponent to your character in the constructor
HealthComp = CreateDefaultSubobject<UCoreHealthComponent>(TEXT("HealthComp"));

// Bind to events
HealthComp->OnHealthChanged.AddDynamic(this, &AMyCharacter::HandleHealthChanged);
HealthComp->OnDeath.AddDynamic(this, &AMyCharacter::HandleDeath);
```

### CoreTween (C++)

```cpp
// Fade a widget in over 0.5s with ease-out
co_await UCoreTween::Create(MyWidget, 0.5f)
    .FromOpacity(0.0f)
    .ToOpacity(1.0f)
    .Easing(ECoreTweenEasingType::EaseOutCubic)
    .Run(this);

// Chain a sequence
co_await FCoreTweenSequence::Create()
    .Then(UCoreTween::Create(Widget, 0.3f).ToOpacity(1.0f))
    .Then(UCoreTween::Create(Widget, 0.2f).ToScale(FVector2D(1.0f)))
    .Run(this);
```

### Interaction System

```cpp
// On the interactable actor:
InteractionComp = CreateDefaultSubobject<UCoreInteractionComponent>(TEXT("Interaction"));
InteractionComp->PromptText = FText::FromString(TEXT("Open Door"));
InteractionComp->OnInteracted.AddDynamic(this, &ADoor::HandleInteracted);

// On the player:
InteractorComp = CreateDefaultSubobject<UCoreInteractorComponent>(TEXT("Interactor"));
InteractorComp->DetectionRadius = 250.0f;
// Call InteractorComp->TryInteract() from input binding
```

### Object Pool

```cpp
// Create and pre-warm a pool
UObjectPoolSubsystem* PoolSub = GetWorld()->GetSubsystem<UObjectPoolSubsystem>();
PoolSub->CreatePool<AProjectile>(TEXT("Projectiles"), 20, 64);

// Acquire and release
AProjectile* Proj = PoolSub->AcquireFromPool<AProjectile>(TEXT("Projectiles"));
// ... use projectile ...
PoolSub->ReleaseActorToPool(TEXT("Projectiles"), Proj);
```

### Save System

```cpp
UCoreSaveSubsystem* SaveSub = GetGameInstance()->GetSubsystem<UCoreSaveSubsystem>();

// Save
UCoreFrameworkSaveGame* Save = SaveSub->GetOrCreateSaveGame(TEXT("Slot1"));
Save->SetStringValue(TEXT("PlayerName"), TEXT("Hero"));
SaveSub->SaveGame(TEXT("Slot1"));

// Load
SaveSub->LoadGame(TEXT("Slot1"));
FString Name = SaveSub->GetCurrentSaveGame()->GetStringValue(TEXT("PlayerName"));
```

### Level Management

```cpp
ULevelManagerSubsystem* LevelMgr = GetGameInstance()->GetSubsystem<ULevelManagerSubsystem>();

// Async level load via coroutine
bool bSuccess = co_await LevelMgr->LoadLevelTask(TEXT("/Game/Maps/Arena"), ELevelOperation::Open);

// Blueprint fire-and-forget
LevelMgr->LoadLevel(TEXT("/Game/Maps/Arena"));
```

### Modal Dialogs

```cpp
UCoreModal* Modal = Cast<UCoreModal>(UISub->AddWidgetToStack(ModalClass, EWidgetContainerType::Modal));
Modal->SetTitle(FText::FromString(TEXT("Quit Game?")));
Modal->SetMessage(FText::FromString(TEXT("Are you sure?")));

ECoreModalResult Result = co_await Modal->ShowAndWaitTask();
if (Result == ECoreModalResult::Confirmed)
{
    // quit
}
```

### Game Flow

```cpp
UCoreGameFlowSubsystem* Flow = GetGameInstance()->GetSubsystem<UCoreGameFlowSubsystem>();
Flow->SetConfig(MyGameFlowConfig); // data asset with states and transitions
Flow->RequestTransition(TEXT("InGame"));

// Or co_await
bool bOk = co_await Flow->TransitionTask(TEXT("Paused"));
```

---

## Architecture Overview

```
UnrealCoreFramework (Plugin)
├── UnrealCoreFramework (Module)  — Core gameplay, UI, subsystems, tools
│   ├── Game/Base/                — ACoreActor, ACoreCharacter, ACoreGameMode, etc.
│   ├── UI/                      — UCoreWidget, UCorePage, UCoreModal, UMainUiContainer
│   ├── SubSystems/              — Base subsystem classes + Level, Audio, Input, UI, GameFlow, etc.
│   ├── Components/              — UCoreHealthComponent
│   ├── Input/                   — Enhanced Input integration
│   ├── Data/                    — UCoreDataAsset, UCorePrimaryDataAsset, UDamageTypeDataAsset
│   ├── Http/                    — URestRequest (async HTTP)
│   ├── ViewModels/              — UCoreViewModel, UPlayerHealthViewModel
│   ├── Tween/                   — UTweenManagerSubsystem
│   ├── Async/                   — Async actions, conventions, UCF_ASYNC_CONTRACT macro
│   ├── Settings/                — UUnrealCoreFrameworkSettings
│   ├── Tools/                   — UHelperStatics, USubsystemHelper
│   └── ThirdPersonController/   — ACoreThirdPersonCharacter
│
├── CoreTween (Module)           — Tweening engine
│   ├── UCoreTween               — Static factory API
│   ├── FCoreTweenBuilder        — Fluent builder → Run() → TTask<void>
│   ├── FCoreTweenSequence       — Sequential chains
│   ├── FCoreTweenEasing         — 30+ easing functions
│   └── UCoreTweenWorldSubsystem — Per-world state isolation
│
├── CoreInteraction (Module)     — Interaction system
│   ├── IInteractable            — Interface
│   ├── UCoreInteractionComponent — On interactable actors
│   └── UCoreInteractorComponent  — On the player
│
├── CoreSpawning (Module)        — Object pooling and spawners
│   ├── IPoolable / APoolableActor
│   ├── FObjectPoolBase / TObjectPool<T>
│   ├── UObjectPoolSubsystem
│   ├── ASpawner / ASpawnerVolume
│   └── USpawnerConfigDataAsset / USpawnerFactory
│
└── CoreSave (Module)            — Save/load system
    ├── UCoreFrameworkSaveGame
    └── UCoreSaveSubsystem
```

---

## Documentation

See the [docs/](docs/) directory for detailed documentation:

- [Module Overview](docs/01-ModuleOverview.md)
- [Gameplay Framework](docs/02-GameplayFramework.md)
- [UI System](docs/03-UISystem.md)
- [CoreTween Engine](docs/04-CoreTween.md)
- [Subsystems](docs/05-Subsystems.md)
- [Interaction System](docs/06-InteractionSystem.md)
- [Spawning & Object Pools](docs/07-SpawningAndPools.md)
- [Save System](docs/08-SaveSystem.md)
- [Data Assets](docs/09-DataAssets.md)
- [Input System](docs/10-InputSystem.md)
- [HTTP / REST](docs/11-HTTP.md)
- [MVVM / ViewModels](docs/12-MVVM.md)
- [Async Conventions](docs/13-AsyncConventions.md)
- [Utilities](docs/14-Utilities.md)
- [Settings & Configuration](docs/15-Settings.md)

---

## License

MIT License — see [LICENSE](LICENSE) for details.

