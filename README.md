# Unreal Core Framework
Current Version : Very early in development

Unreal Engine **5.7+** required. `AsyncFlow` plugin is a required dependency.

What is Unreal Core Framework?
---
This is a Plugin for Unreal Engine 5.7+ and its purpose is to help speed up development by providing a set of systems to help with prototyping and development. To do this we leverage the usage of SubSystems to add more functionality to the engine.

## AsyncFlow Integration

AsyncFlow is the framework's sole async runtime. All previous callback-based, `TFuture`/`TPromise`-based, and `FTSTicker`-based async patterns have been removed and replaced with `AsyncFlow::TTask<T>` coroutines.

**Migration rules:**
- All async APIs now return `AsyncFlow::TTask<T>`. Old callback overloads have been removed.
- Use `co_await` for all new and existing async code.
- Blueprint-callable `UFUNCTION` entry points remain but are re-implemented on top of coroutines internally.

**Canonical examples:**

| Domain | Usage |
|--------|-------|
| Asset loading | `co_await DataAssetManager->LoadDataAssetAsyncTask(AssetId)` |
| Level transitions | `co_await LevelManager->LoadLevelTask(Path, ELevelOperation::Stream)` |
| HTTP requests | `auto Response = co_await RestRequest->RequestTask(EHttpRequestType::GET, Url)` |
| UI transitions | `co_await TweenManager->PlayWidgetTransitionEffectTask(Widget, Options, Mode)` |

* Planned Features
	* UI SubSystem - Centralized place where menus are created and tracked
	 	* Base set of classes to define menu widgets named Page
	  	* Base intro/outro animation control using CoreTween (coroutine-native tweening library) or Widget Animations
	* Next System: Object Pooling System
 		* Spawner Actor
   		* Spawner Volume
     	* Factory
	* Sample project
