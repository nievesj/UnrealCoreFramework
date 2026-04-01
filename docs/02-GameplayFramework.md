# Gameplay Framework

The `Game/Base/` directory provides base classes for every major gameplay framework type in Unreal Engine. These classes serve as the project-wide foundation — subclass them instead of the raw Engine types.

---

## Class Hierarchy

| Core Framework Class | Engine Parent | Purpose |
|---|---|---|
| `ACoreActor` | `AActor` | Base actor with BeginPlay/Tick overrides |
| `ACorePawn` | `APawn` | Base pawn with input setup |
| `ACoreCharacter` | `ACharacter` | Base character with input setup |
| `ACoreGameMode` | `AGameModeBase` | Game mode with `OnGameStateInitialized()` hook and configurable pawn class |
| `ACoreGameState` | `AGameStateBase` | Game state with `OnComponentsInitialized()` hook |
| `ACorePlayerController` | `APlayerController` | Base player controller |
| `ACorePlayerState` | `APlayerState` | Base player state |
| `UCoreGameInstance` | `UGameInstance` | Game instance with startup/shutdown hooks and local player registration |
| `ACoreGameSession` | `AGameSession` | Base game session for online state |
| `ACoreWorldSettings` | `AWorldSettings` | Base world settings |
| `ACoreSpectatorPawn` | `ASpectatorPawn` | Base spectator pawn |
| `UCoreCommonGameViewportClient` | `UCommonGameViewportClient` | Viewport client with CommonUI integration |
| `UCoreActorComponent` | `UActorComponent` | Base actor component |
| `UCoreSceneComponent` | `USceneComponent` | Base scene component |
| `ACoreThirdPersonCharacter` | `ACoreCharacter` | Ready-made third-person character with spring arm + follow camera |

---

## ACoreGameMode

The framework game mode extends `AGameModeBase` and provides:

- **`InitGameState()`** — Overridden to call `OnGameStateInitialized()` after the game state is created.
- **`OnGameStateInitialized()`** — Virtual hook for post-init logic. Override this instead of `InitGameState()`.
- **`PlayerPawnClass`** — `UPROPERTY(EditAnywhere)` to set the default pawn class from the editor.

```cpp
UCLASS()
class AMyGameMode : public ACoreGameMode
{
    GENERATED_BODY()
public:
    virtual void OnGameStateInitialized() override
    {
        // Game state is now fully initialized
    }
};
```

## ACoreGameState

Extends `AGameStateBase` with:

- **`PostInitializeComponents()`** — Calls `OnComponentsInitialized()` after all components are ready.
- **`OnComponentsInitialized()`** — Virtual hook for derived classes.
- **`ReceivedGameModeClass()`** — Overridden for game mode class reception.

## UCoreGameInstance

Extends `UGameInstance` with:

- **`Init()`** / **`OnStart()`** / **`Shutdown()`** / **`FinishDestroy()`** — Full lifecycle hooks.
- **`OnLocalPlayerAdded()`** — Called when a local player joins. Bind to this for per-player initialization.
- **`CreateGameModeForURL()`** — Overridden for custom game mode resolution.
- **PIE support** — `StartPlayInEditorGameInstance()` override for editor workflows.

## ACoreThirdPersonCharacter

A ready-to-use third-person character providing:

- **`USpringArmComponent* CameraBoom`** — Positions the camera behind the character.
- **`UCameraComponent* FollowCamera`** — The camera attached to the boom.
- **`TurnRateGamepad`** — Configurable gamepad turn rate (degrees/second).
- Movement methods: `MoveForward()`, `MoveRight()`, `TurnAtRate()`, `LookUpAtRate()`.
- Touch input handlers: `TouchStarted()`, `TouchStopped()`.
- Input binding via `SetupPlayerInputComponent()`.

```cpp
UCLASS()
class AMyCharacter : public ACoreThirdPersonCharacter
{
    GENERATED_BODY()
    // Inherits spring arm, follow camera, and basic movement
};
```

---

## Best Practices

1. Always subclass the Core Framework base classes rather than raw Engine types.
2. Use `ACoreGameMode` as your base game mode so subsystems and game flow integrate correctly.
3. Use `UCoreGameInstance` for state that persists across level transitions.
4. Override `OnGameStateInitialized()` / `OnComponentsInitialized()` instead of `InitGameState()` / `PostInitializeComponents()` directly.

