# CoreTween Engine

CoreTween is a coroutine-based tweening engine for UMG widgets. It uses AsyncFlow's `TTask<void>` as its async primitive, allowing tweens to be `co_await`-ed inline.

---

## Key Concepts

- **Builder pattern** — `UCoreTween::Create()` returns an `FCoreTweenBuilder` by value. Chain property setters, then call `Run()`.
- **Per-world isolation** — All tween state lives in `UCoreTweenWorldSubsystem`, so PIE worlds don't interfere.
- **Target abstraction** — `ICoreTweenTarget` decouples interpolation from the concrete target type (widget, actor, etc.).
- **Async-first** — `Run()` returns a `TTask<void>` that resolves when the tween completes.

---

## Quick Reference

### Create and Run

```cpp
co_await UCoreTween::Create(MyWidget, 0.5f)
    .FromOpacity(0.0f)
    .ToOpacity(1.0f)
    .Easing(ECoreTweenEasingType::EaseOutCubic)
    .Run(this);
```

### Parameters

| Parameter | Default | Description |
|---|---|---|
| `Widget` | (required) | The UWidget to animate |
| `Duration` | `1.0f` | Animation duration in seconds |
| `Delay` | `0.0f` | Delay before animation starts |
| `bAdditive` | `false` | If false, existing tweens on this widget are cancelled first |

### Cancel and Query

```cpp
// Cancel all tweens on a widget
UCoreTween::Clear(MyWidget);

// Check if tweening
bool bActive = UCoreTween::GetIsTweening(MyWidget);

// Force-complete all tweens in the world
UCoreTween::CompleteAll(WorldContextObject);
```

---

## Animatable Properties

| Builder Method | Type | Description |
|---|---|---|
| `FromTranslation()` / `ToTranslation()` | `FVector2D` | Render transform translation |
| `FromScale()` / `ToScale()` | `FVector2D` | Render transform scale |
| `FromOpacity()` / `ToOpacity()` | `float` | Widget opacity (0–1) |
| `FromColor()` / `ToColor()` | `FLinearColor` | Widget tint color |
| `FromRotation()` / `ToRotation()` | `float` | Render transform rotation (degrees) |
| `FromCanvasPosition()` / `ToCanvasPosition()` | `FVector2D` | Canvas panel slot position |
| `FromPadding()` / `ToPadding()` | `FMargin` | Widget padding |
| `FromVisibility()` / `ToVisibility()` | `ESlateVisibility` | Discrete snap at alpha >= 1.0 |
| `FromMaxDesiredHeight()` / `ToMaxDesiredHeight()` | `float` | Size box max desired height |

If only `To` is specified, the tween reads the current value from the widget as the start.

---

## Configuration

| Builder Method | Description |
|---|---|
| `Easing(Type, Param)` | Set easing curve. Optional param for Back overshoot, Elastic amplitude, Spring damping. |
| `SetDelay(Seconds)` | Override the initial delay. |
| `SetLoops(Count)` | Number of loops. Use -1 for infinite (not recommended for awaitable tweens). |
| `SetPingPong(bool)` | If true, the tween reverses direction each loop. |
| `SetTimeSource(Source)` | `GameTime` (pause-aware), `Unpaused` (default), or `RealTime`. |
| `ToReset()` | Tween all set properties back to their original values. |

### Callbacks

```cpp
UCoreTween::Create(Widget, 0.5f)
    .ToOpacity(1.0f)
    .OnStart(FCoreTweenSignature::CreateLambda([]() { /* started */ }))
    .OnComplete(FCoreTweenSignature::CreateLambda([]() { /* done */ }))
    .Run(this);
```

---

## Easing Functions

30+ easing curves available via `ECoreTweenEasingType`:

| Family | In | Out | InOut |
|---|---|---|---|
| Quad | `EaseInQuad` | `EaseOutQuad` | `EaseInOutQuad` |
| Cubic | `EaseInCubic` | `EaseOutCubic` | `EaseInOutCubic` |
| Quart | `EaseInQuart` | `EaseOutQuart` | `EaseInOutQuart` |
| Quint | `EaseInQuint` | `EaseOutQuint` | `EaseInOutQuint` |
| Expo | `EaseInExpo` | `EaseOutExpo` | `EaseInOutExpo` |
| Sine | `EaseInSine` | `EaseOutSine` | `EaseInOutSine` |
| Circ | `EaseInCirc` | `EaseOutCirc` | `EaseInOutCirc` |
| Back | `EaseInBack` | `EaseOutBack` | `EaseInOutBack` |
| Elastic | `EaseInElastic` | `EaseOutElastic` | `EaseInOutElastic` |
| Bounce | `EaseInBounce` | `EaseOutBounce` | `EaseInOutBounce` |
| — | `Linear` | — | — |
| — | `Spring` | — | — |

Back, Elastic, and Spring accept an optional parameter via `Easing(Type, Param)`:
- **Back:** overshoot (default 1.70158)
- **Elastic:** amplitude (default 1.0)
- **Spring:** damping (default 0.5)

All easing functions are pure static methods on `FCoreTweenEasing`.

---

## Sequences

Chain multiple tweens sequentially:

```cpp
co_await FCoreTweenSequence::Create()
    .Then(UCoreTween::Create(Widget, 0.3f).FromOpacity(0.0f).ToOpacity(1.0f))
    .Then(UCoreTween::Create(Widget, 0.2f).FromScale(FVector2D(0.5f)).ToScale(FVector2D(1.0f)))
    .Then(UCoreTween::Create(Widget, 0.1f).ToRotation(360.0f))
    .Run(this);
```

Each step runs to completion before the next begins.

---

## Time Sources

```cpp
enum class ECoreTweenTimeSource : uint8
{
    GameTime,   // Pauses with the game, affected by time dilation
    Unpaused,   // Ticks during pause (default for widget tweens)
    RealTime    // Wall-clock time, ignores pause and dilation
};
```

---

## Property Templates

### TCoreTweenProp<T>

Interpolating property. Lerps between `StartValue` and `TargetValue` using eased alpha. Supports `FVector2D`, `float`, `FLinearColor`, `FVector4`.

### TCoreTweenInstantProp<T>

Discrete (snap) property. Switches to `TargetValue` when alpha >= 1.0. Used for `ESlateVisibility`.

---

## Target System

`ICoreTweenTarget` provides the abstraction layer between the interpolation engine and the object being animated.

**Built-in targets:**
- `FWidgetTweenTarget` — For `UWidget` objects
- `FActorTweenTarget` — For `AActor` objects (stub, future)
- `FSceneComponentTweenTarget` — For `USceneComponent` objects (stub, future)

---

## Blueprint Support

All CoreTween operations are exposed to Blueprints via `UCoreTweenBlueprintFunctionLibrary`:

- `CreateTween()` → returns `UCoreTweenParamChain`
- Chain property nodes: `ToTranslation`, `FromOpacity`, `SetEasing`, etc.
- `RunTween()` — Execute the tween
- `ClearTweens()` / `IsTweening()` — Management

---

## Per-World Subsystem

`UCoreTweenWorldSubsystem` stores `TArray<TSharedPtr<FCoreTweenState>>` per world:

- **`RegisterTweenState()`** — Called by `FCoreTweenBuilder::Run()`.
- **`Clear(Widget)`** — Cancel all tweens on a widget.
- **`GetIsTweening(Widget)`** — Check if any tween is active.
- **`CompleteAll()`** — Force all tweens to their end state.
- Automatic pruning of finished tween states.

### FCoreTweenState

Shared mutable state for a running tween:

- `bForceComplete` — Atomic flag checked each tick by the coroutine.
- `FlowState` — Reference to the AsyncFlow state for cancellation.
- `Widget` — Weak pointer to the animated widget.
- `bFinished` — Set when the coroutine exits.
- `RunningTask` — Keeps the coroutine frame alive.

