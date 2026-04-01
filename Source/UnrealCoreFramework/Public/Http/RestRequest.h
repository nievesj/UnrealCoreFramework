// MIT License
//
// Copyright (c) 2026 José M. Nieves
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

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

DECLARE_LOG_CATEGORY_EXTERN(LogCoreRestRequest, Log, All);

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
