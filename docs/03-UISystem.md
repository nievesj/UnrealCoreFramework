# UI System

The UI system is built on **CommonUI** and provides a layered widget stack architecture with animation support, modal dialogs, page navigation, and MVVM binding.

---

## Architecture

```
UMainUiContainer (root viewport widget)
├── HUDStack    (UCoreStackWidgetContainer)
├── PageStack   (UCoreStackWidgetContainer)
└── ModalStack  (UCoreStackWidgetContainer)
```

Each stack is a `UCommonActivatableWidgetStack` that manages widget activation/deactivation lifecycle.

---

## Core Types

### UCoreWidget

**Parent:** `UCommonActivatableWidget` + `IAnimatableWidgetInterface`

The base class for all framework widgets. Provides:

- **`Show()` / `Hide()`** — Activate/deactivate with animation support.
- **`ShowTask()` / `HideTask()`** — Coroutine variants that `co_await` until the animation completes.
- **`OnShown()` / `OnHidden()`** — Blueprint-implementable events.
- **`OnViewModelBound()`** — Called when a ViewModel is bound to this widget.
- **Animation settings** — `FCoreWidgetAnimationSettings` with multiple animation backends:
  - `None` — No animation.
  - `CommonUiDefault` — Built-in CommonUI transitions.
  - `WidgetTween` — CoreTween-powered transitions (fade, scale, translation).
  - `WidgetAnimation` — UMG widget animations (`IntroAnimation` / `OutroAnimation`).
- **`bDestroyOnDeactivated`** — If true, the widget is destroyed when deactivated.

```cpp
// Show a widget and wait for its intro animation
co_await MyWidget->ShowTask();

// Hide and wait for outro animation
co_await MyWidget->HideTask();
```

### UCorePage

**Parent:** `UCoreWidget` + `IPageableWidgetInterface`

A full-screen page widget with:

- **`Open()` / `Close()`** — Page lifecycle from `IPageableWidgetInterface`.
- **`DisablePlayerInput`** — When true, disables controller input while this page is open.
- **`ExitButton`** — Optional bound `UButton` that auto-wires to close the page.
- **Page history** — The `UUISubsystem` tracks open pages for back navigation.

### UCoreModal

**Parent:** `UCoreWidget`

Modal dialog that blocks input to widgets behind it:

- **`ShowAndWaitTask()`** — `co_await` to get `ECoreModalResult` (Confirmed, Cancelled, Dismissed).
- **`Confirm()` / `Cancel()` / `Dismiss()`** — Resolve the modal programmatically.
- **`SetTitle()` / `SetMessage()`** — Set text on bound `UTextBlock` widgets.
- **`OnModalResult`** — Blueprint-assignable delegate for result notification.
- **Optional bound widgets:** `ConfirmButton`, `CancelButton`, `CloseButton`, `TitleText`, `MessageText`.

```cpp
UCoreModal* Modal = Cast<UCoreModal>(
    UISub->AddWidgetToStack(ModalClass, EWidgetContainerType::Modal));
Modal->SetTitle(FText::FromString(TEXT("Confirm Action")));
Modal->SetMessage(FText::FromString(TEXT("Are you sure?")));

ECoreModalResult Result = co_await Modal->ShowAndWaitTask();
if (Result == ECoreModalResult::Confirmed)
{
    // User confirmed
}
```

### UMainUiContainer

**Parent:** `UCorePage`

The root viewport widget containing three stacks:

- **`HUDStack`** — For persistent HUD elements.
- **`PageStack`** — For full-screen pages (menus, inventories).
- **`ModalStack`** — For modal dialogs and prompts.

Each stack is a `UCoreStackWidgetContainer` bound via `BindWidget`.

```cpp
// Add a widget to a specific stack
UMainUiContainer* Container = UISub->GetMainUIContainer();
UCoreWidget* Widget = Container->AddWidgetToStack<UCoreWidget>(
    WidgetClass, EWidgetContainerType::Page);
```

### UCoreStackWidgetContainer

**Parent:** `UCommonActivatableWidgetStack`

Type-safe widget stack with:

- **`AddWidgetToStack<T>()`** — Template method that validates class hierarchy before adding.
- **`AddWidgetWithAnimation()`** — Add with a named animation preset.
- **`RemoveWidgetFromStack()`** — Remove with optional exit animation.
- **`GetTopWidget()`** — Returns the top-most widget.
- **`ClearWidgetStack()`** — Remove all widgets, optionally animated.

### ACoreHUD

**Parent:** `AHUD`

HUD actor that creates the main HUD widget:

- **`ShowMainHUD()` / `HideMainHUD()`** — Create/destroy the main HUD widget.
- **`MainPageClass`** — The widget class to instantiate (falls back to settings if not set).

### UCoreInputActionButton

**Parent:** `UCommonButtonBase`

Button widget with automatic input action label:

- **`ButtonLabelText`** — Displayed text.
- **`InputActionLabel`** — Bound `UCommonTextBlock` that updates with the current text style.

---

## Widget Container Types

```cpp
enum class EWidgetContainerType : uint8
{
    HUD,    // Persistent overlay elements
    Page,   // Full-screen pages
    Modal   // Modal dialogs/prompts
};
```

## Modal Results

```cpp
enum class ECoreModalResult : uint8
{
    Confirmed,  // OK / Yes
    Cancelled,  // Cancel / No
    Dismissed   // Close without choice
};
```

## Animation Types

```cpp
enum class EWidgetAnimationType : uint8
{
    None,              // No animation
    CommonUiDefault,   // Built-in CommonUI transitions
    WidgetTween,       // CoreTween-based transitions
    WidgetAnimation    // UMG WidgetAnimation assets
};
```

## Transition Options

### FWidgetTweenTransitionOptions

| Property | Type | Description |
|---|---|---|
| `TransitionType` | `EWidgetTransitionType` | Fade, Scale, or Translation |
| `WidgetTranslationType` | `EWidgetTranslationType` | FromLeft, FromRight, FromTop, FromBottom |
| `EasingType` | `ECoreTweenEasingType` | Easing curve for the transition |
| `TransitionTime` | `float` | Duration in seconds |
| `FadeFrom` / `FadeTo` | `float` | Opacity range |
| `ScaleFrom` / `ScaleTo` | `FVector2D` | Scale range (when TransitionType == Scale) |
| `UseViewportAsTranslationOrigin` | `bool` | Derive translation from viewport edges |
| `TranslationFrom` / `TranslationTo` | `FVector2D` | Manual translation positions |
| `TranslationFromOffset` / `TranslationToOffset` | `FVector2D` | Offset from calculated positions |

---

## UUISubsystem

**Parent:** `UCoreLocalPlayerSubsystem`

Per-player subsystem managing the UI stack:

- **`CreateMainUIContainer()`** — Creates the root container from settings.
- **`AddWidgetToStack()`** — Add a widget to a specific container stack.
- **`RemoveWidgetFromStack()`** — Remove a widget from a stack.
- **`CreateViewportPage()` / `RemoveViewportPage()`** — Direct viewport page management.
- **`GoBack()`** — Navigate to the previous page in history.
- **`GetTopPage()`** — Returns the current top page.
- **`ShouldDisablePlayerControllerInput()`** — True if any open page requests input disable.
- **`SetPlayerControllerInput()`** — Enable/disable controller input.

## Blueprint Function Library

`UCoreUiBlueprintFunctionLibrary` exposes UI operations to Blueprints:

- `CreateViewportPage()` / `RemoveViewportPage()`
- `AddWidgetToStack()` / `RemoveWidgetFromStack()`
- `CreateMainHUD()` / `CreateMainMenu()` / `CreatePauseMenu()`
- `GetMainPage()` — Retrieve a main page by type (MainMenu, MainHUD, PauseMenu)
- `CreatePrompt()` — Create a yes/no prompt widget

---

## Interfaces

### IPageableWidgetInterface

```cpp
virtual void Open();
virtual void Close();
virtual bool GetDisablePlayerInput() const;
```

### IAnimatableWidgetInterface

```cpp
virtual FCoreWidgetAnimationSettings GetWidgetAnimationSettings();
virtual void OnAnimationStarted(const EWidgetTransitionMode& TransitionMode);
virtual void OnAnimationCompleted(const EWidgetTransitionMode& TransitionMode);
virtual bool ShouldPlayAnimations() const;
```

