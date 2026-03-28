// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interfaces/IHttpRequest.h"
#include "AsyncFlowTask.h"

#include "RestRequest.generated.h"

UENUM(BlueprintType)
enum class EHttpRequestType : uint8
{
	GET,
	PUT,
	POST
};

/** Delegate name kept for backward compatibility with existing Blueprint bindings */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHttpResponseRecieved, int32, RequestIndex, int32, ResponseCode, const FString&, Message);

/** Corrected spelling alias — use this in new code */
using FHttpResponseReceived = FHttpResponseRecieved;

UCLASS()
class UNREALCOREFRAMEWORK_API URestRequest : public UObject
{
	GENERATED_BODY()

public:
	/**
	 * Send an HTTP request (Blueprint entry point, fire-and-forget).
	 * Fires HTTPResponseRecievedDelegate on completion.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rest Request")
	void Request(EHttpRequestType RequestType, FString Uri, FString JsonData = TEXT(""));

	/**
	 * Send an HTTP request and co_await the response.
	 * @param RequestType GET, PUT, or POST
	 * @param Uri Target URL
	 * @param JsonData Optional JSON body for PUT/POST
	 * @return TTask resolving to the HTTP response pointer (nullptr on failure)
	 */
	AsyncFlow::TTask<FHttpResponsePtr> RequestTask(EHttpRequestType RequestType, const FString& Uri, const FString& JsonData = TEXT(""));

	/** Blueprint-bindable delegate fired when a response arrives */
	UPROPERTY(BlueprintAssignable, Category = "Rest Request")
	FHttpResponseRecieved HTTPResponseRecievedDelegate;

private:
	TSharedRef<IHttpRequest> CreateRequest(EHttpRequestType RequestType, const FString& Uri, const FString& JsonData = TEXT(""));

	/** Active request task for cancellation/ownership */
	AsyncFlow::TTask<FHttpResponsePtr> ActiveRequestTask;
};
