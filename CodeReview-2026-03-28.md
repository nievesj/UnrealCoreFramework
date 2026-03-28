# Unreal Core Framework — Code Review

**Date:** 2026-03-28  
**Plugin Version:** 1.0 (Beta / Experimental)  
**Modules Reviewed:** UnrealCoreFramework, CoreTween, CoreInteraction, CoreSpawning, CoreSave  
**Files Reviewed:** ~130 (.h, .cpp, .Build.cs)

---

## Executive Summary

The Unreal Core Framework is an ambitious plugin providing base game classes, UI framework, input, subsystems, camera management, data assets, async actions, MVVM viewmodels, HTTP utilities, a tween engine, interaction system, object pooling/spawning, and save system infrastructure. The codebase shows strong architectural intent with good use of modern C++20 coroutines (via AsyncFlow), UE subsystem patterns, and MVVM.

However, the review uncovered **5 critical**, **7 high**, and **12 medium** severity issues. Several files have syntax corruption that prevents compilation, multiple null-check patterns log errors but don't return (causing guaranteed crashes), and the CoreTween's Blueprint coroutine lifetime is fundamentally broken. Three modules (CoreInteraction, CoreSpawning, CoreSave) are not registered in the `.uplugin` file.

---

## Issue Summary Table

| # | Severity | Module | File | Issue |
|---|----------|--------|------|-------|
| 1 | 🔴 Critical | UnrealCoreFramework | CoreWidget.cpp | File is catastrophically corrupted — scrambled code |
| 2 | 🔴 Critical | UnrealCoreFramework | CorePage.cpp | Multiple syntax errors — missing braces, orphaned line |
| 3 | 🔴 Critical | UnrealCoreFramework | UiCoreFrameworkTypes.h | Missing enum declarations (EWidgetAnimationType, etc.) |
| 4 | 🔴 Critical | UnrealCoreFramework | ViewModelManagerSubsystem.cpp | ViewModel never stored in map — `TMap::Add` missing value |
| 5 | 🔴 Critical | CoreTween | CoreTween.cpp | Blueprint `RunTween()` — coroutine destroyed before completion |
| 6 | 🟠 High | UnrealCoreFramework | PlayerHealthViewModel.cpp | Missing `return` after null check → null dereference crash |
| 7 | 🟠 High | UnrealCoreFramework | UISubsystem.cpp | `AddWidgetToStack`/`RemoveWidgetFromStack` crash on null MainUiContainer |
| 8 | 🟠 High | UnrealCoreFramework | DataAssetManagerSubSystem.cpp | Dereferences potentially null `AssetManager` |
| 9 | 🟠 High | CoreTween | CoreTweenSequence.cpp | Use-after-free — `this->Steps` accessed across coroutine suspension |
| 10 | 🟠 High | CoreTween | CoreTweenBuilder.cpp / CoreTween.cpp | Use-after-free on dangling `FlowState` via non-owning TSharedPtr |
| 11 | 🟠 High | CoreTween | CoreTween.cpp | Permanent GC root leak if `CreateTween` called without `RunTween` |
| 12 | 🟠 High | CoreSpawning | Spawner.cpp | Spawner never releases actors back to pool — pool integration broken |
| 13 | 🟠 High | CoreInteraction | CoreInteractorComponent.cpp | Scanner checks actor for IInteractable but only a component implements it |
| 14 | 🟡 Medium | UnrealCoreFramework | UISubsystem.cpp | `Initialize` doesn't call `Super::Initialize` (also AudioSubsystem, InputSubsystem) |
| 15 | 🟡 Medium | UnrealCoreFramework | DamageTypeDataAsset.cpp | Fallback sets `UDamageType::StaticClass()` into `TSubclassOf<UCoreDamageType>` |
| 16 | 🟡 Medium | UnrealCoreFramework | CorePage.cpp | `InternalShown` accumulates duplicate delegate bindings |
| 17 | 🟡 Medium | UnrealCoreFramework | CorePage.cpp | References undeclared `DisablePlayerControllerInput` (should be `DisablePlayerInput`) |
| 18 | 🟡 Medium | UnrealCoreFramework | UISubsystem.h | Missing declarations for `CreateMainUIContainer()` and `GetTopPage()` |
| 19 | 🟡 Medium | CoreTween | CoreTweenProperty.h | Setting only "From" silently tweens to zero |
| 20 | 🟡 Medium | CoreInteraction | CoreInteractorComponent.cpp | `TryInteract` broadcasts OnInteracted even when interaction fails |
| 21 | 🟡 Medium | CoreInteraction | CoreInteractionComponent.cpp | `CanInteract_Implementation` called directly, bypassing BP overrides |
| 22 | 🟡 Medium | CoreSpawning | PoolableActor.cpp | `ResetToPool_Implementation` called directly, bypassing BP overrides |
| 23 | 🟡 Medium | CoreSpawning | SpawnerVolume.cpp | Random spawn point ignores box rotation (axis-aligned only) |
| 24 | 🟡 Medium | CoreSpawning | ObjectPool.h | Raw `AActor*` pointers outside GC graph — dangling pointer risk |
| 25 | 🟡 Medium | Plugin Config | .uplugin | CoreInteraction, CoreSpawning, CoreSave not registered as modules |

---

## Detailed Findings

### 🔴 CRITICAL ISSUES

---

#### Issue 1: CoreWidget.cpp — File Is Catastrophically Corrupted

**File:** `Source/UnrealCoreFramework/Private/UI/CoreWidget.cpp`  
**Impact:** Entire UI widget system won't compile

The file is scrambled beyond repair. `#include` directives appear inside function bodies, functions are interleaved with code from other functions, there are orphaned braces, duplicate function definitions, and switch-case labels floating outside any switch statement.

Notable symptoms:
- Line 1: Orphaned closing brace `}` with no matching open
- Lines 6, 9-10: `#include` directives inside an `if (!Anim)` block
- Lines 26-33: `#include` and `DEFINE_LOG_CATEGORY` appear inside `InternalHidden()`
- Lines 46-51: `case` labels appear inside `NativePreConstruct()`
- Lines 68-74: Duplicate `case EWidgetAnimationType::CommonUiDefault:` labels
- Lines 84-115, 117-147: `ShowTask()` and `HideTask()` have interleaved coroutine code
- Lines 148-154: Duplicate `InternalShown()` and `InternalHidden()` definitions
- Lines 169-191: Second duplicate `PlayWidgetAnimation()` definition
- Missing implementations: `GetSequencePlayer`, `OnAnimationStarted`, `OnAnimationCompleted`

**Suggested fix:** Reconstruct from git history: `git show HEAD~N:Source/UnrealCoreFramework/Private/UI/CoreWidget.cpp` — or rewrite based on the clean header declarations.

---

#### Issue 2: CorePage.cpp — Multiple Syntax Errors

**File:** `Source/UnrealCoreFramework/Private/UI/CorePage.cpp`  
**Impact:** Won't compile

```cpp
// Line 1: Orphaned statement before #include — not inside any function
DisablePlayerControllerInput = false;
#include "UI/CorePage.h"
```

```cpp
// Line 49: Missing opening brace
void UCorePage::NativeOnDeactivated()
    Super::NativeOnDeactivated();   // should be inside { }
    DisablePlayerControllerInput = false;
}
```

```cpp
// Line 63: Missing opening brace
void UCorePage::Open()
    IPageableWidgetInterface::Open();    // should be inside { }
    DisablePlayerControllerInput = DisablePlayerInput;
    DisablePlayerControllerInput = DisablePlayerInput;  // duplicate line
    Show();
}
```

Also, `DisablePlayerControllerInput` is referenced throughout but never declared. The header declares `DisablePlayerInput` instead.

**Suggested fix:** Add missing braces, replace `DisablePlayerControllerInput` with `DisablePlayerInput`, remove orphaned line 1, remove duplicate assignment on line 66.

---

#### Issue 3: UiCoreFrameworkTypes.h — Missing Enum Declarations

**File:** `Source/UnrealCoreFramework/Public/UI/UiCoreFrameworkTypes.h:39-42`  
**Impact:** Won't compile — multiple enums used across the codebase are undefined

Lines 39-42 contain orphaned enum values with no `UENUM`/`enum class` declaration:
```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnModalResultDelegate, ECoreModalResult, Result);

    CommonUiDefault,    // ← orphaned, no enum declaration
    WidgetTween,
    WidgetAnimation,
};
```

The following enum types are used throughout the codebase but **never defined anywhere**:
- `EWidgetAnimationType` — used in CoreWidget, CoreStackWidgetContainer, UiCoreFrameworkTypes
- `EWidgetTransitionMode` — used in CoreWidget, AnimatableWidgetInterface, TweenManagerSubsystem
- `EWidgetTransitionType` — used in UiCoreFrameworkTypes (`FWidgetTweenTransitionOptions::TransitionType`)
- `EWidgetTranslationType` — used in UiCoreFrameworkTypes (`FWidgetTweenTransitionOptions::WidgetTranslationType`)

**Suggested fix:** Add the missing `UENUM(BlueprintType)` declarations before line 39:
```cpp
UENUM(BlueprintType)
enum class EWidgetAnimationType : uint8
{
    None,
    CommonUiDefault,
    WidgetTween,
    WidgetAnimation,
};

UENUM(BlueprintType)
enum class EWidgetTransitionMode : uint8
{
    Intro,
    Outtro,
};

UENUM(BlueprintType)
enum class EWidgetTransitionType : uint8
{
    Fade,
    Scale,
    Translation,
};

UENUM(BlueprintType)
enum class EWidgetTranslationType : uint8
{
    FromLeft,
    FromRight,
    FromTop,
    FromBottom,
};
```

---

#### Issue 4: ViewModelManagerSubsystem — ViewModel Never Stored in Map

**File:** `Source/UnrealCoreFramework/Private/ViewModels/ViewModelManagerSubsystem.cpp:34`  
**Impact:** MVVM system is non-functional — every request creates orphaned ViewModels

```cpp
UCoreViewModel* NewViewModel = NewObject<UCoreViewModel>(this, ViewModelClass);
if (IsValid(NewViewModel))
{
    NewViewModel->SetSource(TrackedObject);
    ViewModels.Add(TrackedObject->GetUniqueID());   // ← MISSING VALUE!
    return NewViewModel;
}
```

`TMap::Add(Key)` with a single argument creates a **default-constructed** value (nullptr for `TObjectPtr`). The `NewViewModel` pointer is never inserted into the map.

Consequences:
1. `GetModel()` will never find any existing ViewModel
2. Every `GetOrCreateViewModel()` call creates a new ViewModel that is immediately orphaned
3. Duplicate event subscriptions accumulate (e.g., PlayerHealthViewModel binds OnHealthChanged multiple times)
4. The map fills with null entries

**Suggested fix:**
```cpp
ViewModels.Add(TrackedObject->GetUniqueID(), NewViewModel);
```

---

#### Issue 5: Blueprint `RunTween()` — Coroutine Destroyed Before Completion

**File:** `Source/CoreTween/Private/CoreTween.cpp:99-109`  
**Impact:** All Blueprint tweens silently do nothing — coroutine is immediately killed

```cpp
void UCoreTweenBlueprintFunctionLibrary::RunTween(UCoreTweenParamChain* Params, UObject* WorldContext)
{
    AsyncFlow::TTask<void> Task = Params->Builder.Run(WorldContext);
    Task.Start();       // coroutine runs until first co_await, then suspends
    Params->ClearFlags(RF_MarkAsRootSet);
    Params->ConditionalBeginDestroy();
}   // Task destroyed here → Handle.destroy() → coroutine killed
```

`TTask<void>` is stored as a local variable. When `RunTween` returns, the destructor calls `Handle.destroy()`, tearing down the coroutine frame. The tween applies its initial alpha=0 state and then dies at the first `co_await`. The `TweenState` persists in `ActiveTweenStates` as a zombie with `bFinished=false`.

**Suggested fix:** The `TTask` must survive until the coroutine completes. Options:
- Store active tasks in a `TArray<TTask<void>>` on `UCoreTween` or a subsystem
- Store the `TTask` inside `FCoreTweenState` so the tracking system keeps it alive
- Use `Task.OnComplete()` with a self-capturing shared_ptr to prevent premature destruction

---

### 🟠 HIGH SEVERITY ISSUES

---

#### Issue 6: PlayerHealthViewModel::Initialize Crashes on Null HealthComponent

**File:** `Source/UnrealCoreFramework/Private/ViewModels/PlayerHealthViewModel.cpp:8-19`

```cpp
void UPlayerHealthViewModel::Initialize()
{
    UCoreHealthComponent* HealthComp = GetSourceAs<UCoreHealthComponent>();
    if (!IsValid(HealthComp))
        UE_LOG(LogCoreViewModel, Error, TEXT("...")); // logs but does NOT return

    HealthComponent = HealthComp;        // stores nullptr
    HealthComp->OnHealthChanged.AddDynamic(...);  // CRASH: nullptr dereference
```

The `if` only guards the `UE_LOG`. When `HealthComp` is null, execution falls through to `AddDynamic` which dereferences null.

**Suggested fix:** Add `return;` after the UE_LOG, or wrap both lines in braces with return.

---

#### Issue 7: UISubsystem Crashes on Null MainUiContainer

**File:** `Source/UnrealCoreFramework/Private/SubSystems/LocalPlayer/UISubsystem.cpp:75-94`

Both `AddWidgetToStack` and `RemoveWidgetFromStack` log when `MainUiContainer` is null but **do not return**:
```cpp
if (!MainUiContainer)
{
    UE_VLOG_UELOG(..., TEXT("MainUiContainer is invalid"));
    // ← no return!
}
UCoreWidget* Widget = MainUiContainer->AddWidgetToStack(...);  // CRASH
```

**Suggested fix:** Add `return nullptr;` and `return;` respectively after each null-check log.

---

#### Issue 8: DataAssetManagerSubSystem Dereferences Potentially Null AssetManager

**File:** `Source/UnrealCoreFramework/Private/SubSystems/Game/DataAssetManagerSubSystem.cpp:56`

`LoadDataAssetByPrimaryAssetId` dereferences `AssetManager` without null check:
```cpp
FSoftObjectPath AssetPath = AssetManager->GetPrimaryAssetPath(AssetId);
```

`AssetManager` is set via `UAssetManager::GetIfInitialized()` which returns null if not ready. The sibling `LoadAllDataAssetsOfType` correctly checks `if (!IsValid(AssetManager))` but this method does not.

Additionally, `PreloadAssets` (line 221) and `GetAllAssetIds` (line 209) also dereference `AssetManager` without null checks.

**Suggested fix:** Add `if (!IsValid(AssetManager)) { return nullptr; }` before line 56, and similar guards in `PreloadAssets`/`GetAllAssetIds`.

---

#### Issue 9: CoreTweenSequence::Run() — Use-After-Free on `this->Steps`

**File:** `Source/CoreTween/Private/CoreTweenSequence.cpp:14-20`

```cpp
AsyncFlow::TTask<void> FCoreTweenSequence::Run(UObject* WorldContext)
{
    for (FCoreTweenBuilder& Step : Steps)     // Steps is a member field
    {
        co_await Step.Run(WorldContext);       // suspends — `this` may be destroyed
    }
}
```

When used as a temporary (which is the documented pattern: `co_await FCoreTweenSequence::Create().Then(...).Run(this)`), the `FCoreTweenSequence` is destroyed after the first `co_await`, leaving `Steps` as freed memory. `FCoreTweenBuilder::Run()` explicitly avoids this by moving all state to locals — this function needs the same treatment.

**Suggested fix:**
```cpp
AsyncFlow::TTask<void> FCoreTweenSequence::Run(UObject* WorldContext)
{
    TArray<FCoreTweenBuilder> LocalSteps = MoveTemp(Steps);
    for (FCoreTweenBuilder& Step : LocalSteps)
    {
        co_await Step.Run(WorldContext);
    }
}
```

---

#### Issue 10: Use-After-Free on Dangling FlowState

**File:** `Source/CoreTween/Private/CoreTweenBuilder.cpp:361-364` and `Source/CoreTween/Private/CoreTween.cpp:26-43`

`FlowState` is wrapped in a non-owning `TSharedPtr` with a no-op deleter:
```cpp
TweenState->FlowState = MakeShareable(CurrentFlowState, [](AsyncFlow::FAsyncFlowState*) {});
```

When the coroutine completes, the promise's `TSharedPtr` deletes the `FAsyncFlowState`, but `TweenState->FlowState` still holds a non-null pointer. `IsValid()` returns `true`, so `Clear()` dereferences freed memory:
```cpp
if (State->FlowState.IsValid())   // Always true — pointer is non-null
{
    State->FlowState->Cancel();    // Use-after-free
}
```

**Suggested fix:** Either check `bFinished` before accessing `FlowState` in `Clear()`, set `FlowState = nullptr` at the end of the coroutine body, or use a properly shared reference.

---

#### Issue 11: Permanent GC Root Leak

**File:** `Source/CoreTween/Private/CoreTween.cpp:87-97`

`CreateTween()` sets `RF_MarkAsRootSet` on `UCoreTweenParamChain`. This is only cleared inside `RunTween()`. If Blueprint creates a tween chain but never calls `RunTween` (conditional branch, early return, error), the object is **permanently rooted** for the lifetime of the process.

**Suggested fix:** Use an alternative GC protection strategy — store active chains in a managed collection with cleanup, or use `AddToRoot()`/`RemoveFromRoot()` with a weak-ptr cleanup timer.

---

#### Issue 12: Spawner Never Releases Actors Back to Pool

**File:** `Source/CoreSpawning/Private/Spawner.cpp:78-103`

`ASpawner` acquires actors from the object pool via `SpawnerFactory::SpawnFromConfig` but has no code path to release them back. The only cleanup is `OnSpawnedActorDestroyed`, which fires when `Destroy()` is called — but destroying a pooled actor defeats pooling entirely.

`SpawnerFactory::ReleaseFromConfig` exists and correctly handles pool release, but is **never called by any code** in the entire codebase.

**Suggested fix:** Add a `ReleaseOne(AActor*)` method that calls `SpawnerFactory::ReleaseFromConfig`, removes from `SpawnedActors`, and unbinds `OnDestroyed`.

---

#### Issue 13: Interactor Scans Actors for IInteractable But Only a Component Implements It

**File:** `Source/CoreInteraction/Private/CoreInteractorComponent.cpp:57`

```cpp
if (!OtherActor->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
```

This checks if the **actor class** implements `IInteractable`. However, the only class implementing `IInteractable` is `UCoreInteractionComponent` (a `UActorComponent`). An actor that has a `UCoreInteractionComponent` attached does **not** register as implementing `IInteractable` at the actor class level. The scanner will never find any interactable actors.

**Suggested fix:** Either scan for `UCoreInteractionComponent` on overlapping actors and call interface methods on the component, or redesign so actors implement `IInteractable` directly.

---

### 🟡 MEDIUM SEVERITY ISSUES

---

#### Issue 14: Missing `Super::Initialize()` Calls

**Files:**
- `Private/SubSystems/LocalPlayer/UISubsystem.cpp:170` — no `Super::Initialize(Collection)`
- `Private/SubSystems/Game/AudioSubsystem.cpp` — same
- `Private/SubSystems/LocalPlayer/InputSubsystem.cpp` — same

This breaks the initialization chain. The engine's own subsystem initialization logic is skipped.

**Suggested fix:** Add `Super::Initialize(Collection);` as the first line in each `Initialize()`.

---

#### Issue 15: DamageTypeDataAsset Sets Incorrect Fallback Type

**File:** `Source/UnrealCoreFramework/Private/Data/DamageTypeDataAsset.cpp:20-23`

```cpp
if (!CoreDamageTypeClass)
{
    CoreDamageTypeClass = UDamageType::StaticClass();  // WRONG
}
```

The field is `TSubclassOf<UCoreDamageType>` but the fallback assigns `UDamageType::StaticClass()`. The inheritance goes `UCoreDamageType : UDamageType`, so `UDamageType` is **not** a valid `TSubclassOf<UCoreDamageType>`.

**Suggested fix:** Use `UCoreDamageType::StaticClass()`.

---

#### Issue 16: CorePage::InternalShown Accumulates Duplicate Delegate Bindings

**File:** `Source/UnrealCoreFramework/Private/UI/CorePage.cpp:30-36`

`AddDynamic` is called every time the page is shown. The corresponding `RemoveAll` only fires when the exit button is clicked. If the page is shown → hidden → shown again without clicking exit, duplicate bindings accumulate, causing multiple handler invocations per click.

**Suggested fix:** Use `AddUniqueDynamic` or call `RemoveDynamic` before `AddDynamic`.

---

#### Issue 17: CorePage References Undeclared Member Variable

**File:** `Source/UnrealCoreFramework/Private/UI/CorePage.cpp:1, 26-27, 51, 65-66`

`DisablePlayerControllerInput` is referenced 5 times but never declared in any header. The actual member is `DisablePlayerInput` (declared in `CorePage.h:35`).

**Suggested fix:** Replace all `DisablePlayerControllerInput` with `DisablePlayerInput`.

---

#### Issue 18: UISubsystem.h Missing Method Declarations

**File:** `Source/UnrealCoreFramework/Public/SubSystems/LocalPlayer/UISubsystem.h`

Two methods are defined in the .cpp but never declared in the header:
- `void CreateMainUIContainer()` (cpp line 102)
- `IPageableWidgetInterface* GetTopPage()` (cpp line 96)

**Suggested fix:** Add declarations to the public section of `UUISubsystem`.

---

#### Issue 19: TCoreTweenProp — Setting Only "From" Silently Tweens to Zero

**File:** `Source/CoreTween/Public/CoreTweenProperty.h:23-62`

`IsSet()` returns `bHasStart || bHasTarget`. If only `FromTranslation(100, 200)` is set without `ToTranslation()`, `IsSet()` returns true but `TargetValue` defaults to zero. The tween silently animates from the start value to zero.

**Suggested fix:** Either require `bHasTarget` in `IsSet()`, or in `OnBegin()` fill `TargetValue` from the current state when only start is specified.

---

#### Issue 20: TryInteract Broadcasts OnInteracted Even When Interaction Fails

**File:** `Source/CoreInteraction/Private/CoreInteractorComponent.cpp:91-99`

```cpp
IInteractable::Execute_Interact(FocusedActor, GetOwner());
OnInteracted.Broadcast(FocusedActor);  // Always fires — even if Interact returned early
```

Between the scan tick (0.1s) and `TryInteract`, the interaction state can change. Any system bound to `OnInteracted` will trigger on failed interactions.

**Suggested fix:** Call `Execute_CanInteract` as a guard before `Execute_Interact`, or have `Interact` return a success bool.

---

#### Issue 21: CoreInteractionComponent Bypasses Blueprint Overrides

**File:** `Source/CoreInteraction/Private/CoreInteractionComponent.cpp:12`

```cpp
if (!CanInteract_Implementation(Interactor))  // Bypasses BP override
```

`CanInteract` is a `BlueprintNativeEvent`. Calling `_Implementation()` directly bypasses the UE thunk dispatch, so any Blueprint override is silently ignored.

**Suggested fix:** Replace with `Execute_CanInteract(this, Interactor)`.

---

#### Issue 22: PoolableActor Bypasses Blueprint Overrides

**File:** `Source/CoreSpawning/Private/PoolableActor.cpp:23`

```cpp
ResetToPool_Implementation();  // Direct call — bypasses BP override
```

Same pattern as Issue 21. `ResetToPool` is a `BlueprintNativeEvent` but the `_Implementation` is called directly.

**Suggested fix:** Replace with `Execute_ResetToPool(this)`.

---

#### Issue 23: SpawnerVolume Random Point Ignores Box Rotation

**File:** `Source/CoreSpawning/Private/SpawnerVolume.cpp:21-24`

```cpp
const FVector Origin = SpawnVolume->GetComponentLocation();
const FVector Extent = SpawnVolume->GetScaledBoxExtent();
const FVector RandomPoint = FMath::RandPointInBox(FBox(Origin - Extent, Origin + Extent));
```

`FBox` is axis-aligned. If the `SpawnerVolume` is rotated, spawns occur in an axis-aligned bounding box, not the rotated volume.

**Suggested fix:** Generate the random point in local space, then transform via `SpawnVolume->GetComponentTransform().TransformPosition(LocalPoint)`.

---

#### Issue 24: ObjectPool Uses Raw AActor* Pointers

**File:** `Source/CoreSpawning/Public/ObjectPool.h`

`FObjectPoolBase` is a plain C++ class holding raw `AActor*`:
```cpp
TArray<AActor*> InactiveActors;
TSet<AActor*> ActiveActors;
```

Any external destruction of a pooled actor leaves dangling entries. There is no mechanism to detect or clean up stale pointers.

**Suggested fix:** Use `TWeakObjectPtr<AActor>` and validate with `IsValid()` before use, or make `FObjectPoolBase` a `UObject` with `UPROPERTY()` collections.

---

#### Issue 25: Three Modules Not Registered in .uplugin

**File:** `UnrealCoreFramework.uplugin`

Only `UnrealCoreFramework` and `CoreTween` are listed in the `Modules` array. `CoreInteraction`, `CoreSpawning`, and `CoreSave` are not registered. These modules will not be loaded by the engine. Additionally, `CoreSave` module files are completely empty (no `IMPLEMENT_MODULE`, no function definitions).

**Suggested fix:** Add all three modules to the `.uplugin` when ready, or remove them until implemented. At minimum, `CoreSaveModule.cpp` needs `IMPLEMENT_MODULE`.

---

## Build Configuration Notes

### UnrealCoreFramework.Build.cs
- `CoreUObject` and `Engine` appear in both `PublicDependencyModuleNames` and `PrivateDependencyModuleNames` — the private listing is redundant.
- `IWYU` support is commented out (`//IWYUSupport = IWYUSupport.Full;`).

### CoreTween.Build.cs
- Clean. Depends on `Core`, `UMG`, `AsyncFlow` (public) and `CoreUObject`, `Engine`, `Slate`, `SlateCore` (private).

### CoreInteraction.Build.cs / CoreSpawning.Build.cs
- Both have `Slate`/`SlateCore` as private dependencies but neither module uses Slate. These can be removed.

### CoreSave.Build.cs
- Depends on `AsyncFlow` but the module is empty. Should be cleaned up or populated.

---

## Architecture Observations

### Strengths
- **Clean subsystem hierarchy**: Base subsystem classes (`CoreEngineSubsystem`, `CoreWorldSubsystem`, `CoreLocalPlayerSubsystem`, etc.) provide good extension points
- **AsyncFlow integration**: The DataAssetManager's async loading with `co_await` and the tween coroutine approach are elegant designs
- **MVVM pattern**: ViewModel architecture with `ViewModelManagerSubsystem`, field notifications, and `UE_MVVM_SET_PROPERTY_VALUE` follows UE5's recommended MVVM approach
- **Tween builder pattern**: The fluent API (`UCoreTween::Create(Widget, 0.3f).ToOpacity(1.0f).ToScale(...)`) is well-designed and ergonomic
- **Object pooling**: Good separation between pool management, factory pattern, and poolable actor interface

### Areas for Improvement
- **Heavy commented-out code**: UISubsystem.cpp, CoreUiBlueprintFunctionLibrary.cpp, and MainUiContainer.cpp contain large blocks of commented-out logic. This suggests the UI stack navigation system is in active development but currently non-functional.
- **CoreSave module**: Entirely empty — just placeholder files. Should either be populated or removed.
- **Static state in UCoreTween**: `ActiveTweenStates` is a static `TArray`. This is problematic for PIE (Play In Editor) with multiple worlds, as tweens from one world leak into another. Consider moving to a world subsystem.
- **Missing error recovery**: Many subsystem `Initialize` methods get pointers (e.g., `GetWorld()->GetSubsystem<>()`) without null checks on the intermediate objects.

---

## Recommended Priority

1. **Immediate (blocks compilation):** Issues 1, 2, 3, 17, 18 — Fix corrupted files and missing declarations
2. **Next (crashes at runtime):** Issues 4, 5, 6, 7, 8 — Fix null dereferences and broken ViewModel/Tween systems
3. **Soon (memory/logic bugs):** Issues 9, 10, 11, 12, 13 — Fix use-after-free, leaks, and broken integrations
4. **When convenient:** Issues 14-25 — Fix medium-severity correctness issues and improve robustness
