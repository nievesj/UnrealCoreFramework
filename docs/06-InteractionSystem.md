# Interaction System

The CoreInteraction module provides a component-based interaction system with an interface, proximity detection, cooldowns, and Blueprint events.

---

## Architecture

```
IInteractable (Interface)
    ├── Interact(Interactor)
    ├── CanInteract(Interactor)
    └── GetInteractionPrompt()

UCoreInteractionComponent (on interactable actors)
    implements IInteractable
    ├── PromptText, InteractionRange, CooldownDuration, bEnabled
    └── OnInteracted delegate

UCoreInteractorComponent (on the player)
    ├── DetectionRadius
    ├── TryInteract()
    ├── GetFocusedInteractable()
    ├── OnInteractableFound / OnInteractableLost / OnInteracted
    └── ScanForInteractables() [tick-based detection]
```

---

## IInteractable Interface

Blueprint-native interface that any actor can implement:

```cpp
UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
void Interact(AActor* Interactor);

UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
FText GetInteractionPrompt() const;

UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
bool CanInteract(AActor* Interactor) const;
```

You can implement this interface in C++ or Blueprint on any actor or component.

---

## UCoreInteractionComponent

Place this on actors that can be interacted with. It implements `IInteractable` with configurable parameters.

### Properties

| Property | Type | Default | Description |
|---|---|---|---|
| `PromptText` | `FText` | `"Interact"` | Display text shown to the player |
| `InteractionRange` | `float` | `200.0` | Maximum distance for interaction |
| `CooldownDuration` | `float` | `0.0` | Minimum seconds between interactions |
| `bEnabled` | `bool` | `true` | Whether interaction is currently available |

### Delegate

```cpp
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteracted, AActor*, Interactor);

UPROPERTY(BlueprintAssignable)
FOnInteracted OnInteracted;
```

### Behavior

- `CanInteract_Implementation()` checks `bEnabled`, range, and cooldown elapsed time.
- `Interact_Implementation()` broadcasts `OnInteracted` and updates the cooldown timestamp.
- `GetInteractionPrompt_Implementation()` returns `PromptText`.

### Setup

```cpp
// In your interactable actor constructor
InteractionComp = CreateDefaultSubobject<UCoreInteractionComponent>(TEXT("Interaction"));
InteractionComp->PromptText = FText::FromString(TEXT("Open Chest"));
InteractionComp->InteractionRange = 150.0f;
InteractionComp->CooldownDuration = 1.0f;
InteractionComp->OnInteracted.AddDynamic(this, &AChest::HandleOpened);
```

---

## UCoreInteractorComponent

Place this on the player character. It detects nearby interactable actors via sphere overlap and manages focus.

### Properties

| Property | Type | Default | Description |
|---|---|---|---|
| `DetectionRadius` | `float` | `300.0` | Sphere overlap radius for scanning |

### Methods

| Method | Description |
|---|---|
| `TryInteract()` | Interact with the currently focused interactable |
| `GetFocusedInteractable()` | Returns the nearest valid interactable actor |

### Delegates

```cpp
UPROPERTY(BlueprintAssignable)
FOnInteractableFound OnInteractableFound;   // New interactable entered range

UPROPERTY(BlueprintAssignable)
FOnInteractableLost OnInteractableLost;     // Previously focused interactable left range

UPROPERTY(BlueprintAssignable)
FOnInteractedWith OnInteracted;             // Interaction was executed
```

### Behavior

- **Tick-based scanning:** `TickComponent()` calls `ScanForInteractables()` each frame.
- **Focus management:** Maintains `FocusedActor` — the nearest actor implementing `IInteractable` (via `UCoreInteractionComponent`).
- **Auto-delegation:** `TryInteract()` calls `IInteractable::Execute_Interact()` on the focused actor.

### Setup

```cpp
// In your player character constructor
InteractorComp = CreateDefaultSubobject<UCoreInteractorComponent>(TEXT("Interactor"));
InteractorComp->DetectionRadius = 250.0f;

// Bind events for UI prompts
InteractorComp->OnInteractableFound.AddDynamic(this, &AMyPlayer::ShowPrompt);
InteractorComp->OnInteractableLost.AddDynamic(this, &AMyPlayer::HidePrompt);

// Trigger interaction from Enhanced Input
void AMyPlayer::HandleInteractAction()
{
    InteractorComp->TryInteract();
}
```

---

## Complete Example

```cpp
// === Door Actor ===
UCLASS()
class ADoor : public AActor
{
    GENERATED_BODY()
public:
    ADoor()
    {
        InteractionComp = CreateDefaultSubobject<UCoreInteractionComponent>(TEXT("Interaction"));
        InteractionComp->PromptText = FText::FromString(TEXT("Open Door"));
        InteractionComp->CooldownDuration = 2.0f;
    }

    virtual void BeginPlay() override
    {
        Super::BeginPlay();
        InteractionComp->OnInteracted.AddDynamic(this, &ADoor::HandleOpened);
    }

    UFUNCTION()
    void HandleOpened(AActor* Interactor)
    {
        // Open the door
    }

private:
    UPROPERTY(VisibleAnywhere)
    UCoreInteractionComponent* InteractionComp;
};

// === Player Character ===
UCLASS()
class AMyPlayer : public ACoreCharacter
{
    GENERATED_BODY()
public:
    AMyPlayer()
    {
        InteractorComp = CreateDefaultSubobject<UCoreInteractorComponent>(TEXT("Interactor"));
        InteractorComp->DetectionRadius = 300.0f;
    }

    void HandleInteractInput()
    {
        InteractorComp->TryInteract();
    }

private:
    UPROPERTY(VisibleAnywhere)
    UCoreInteractorComponent* InteractorComp;
};
```

---

## Module Dependencies

CoreInteraction depends only on `Core`, `CoreUObject`, and `Engine`. It has no dependency on the main UnrealCoreFramework module and can be used standalone.

