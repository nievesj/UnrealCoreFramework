// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interfaces/IHttpRequest.h"

#include "RestRequest.generated.h"

UENUM(BlueprintType)
enum class EHttpRequestType : uint8
{
	GET,
	PUT,
	POST
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHttpResponseRecieved, int32, RequestIndex, int32, ResponseCode, const FString&, Message);

UCLASS()
class UNREALCOREFRAMEWORK_API URestRequest : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Rest Request")
	void Request(EHttpRequestType RequestType, FString Uri, FString JsonData = "");

private:
	TSharedRef<IHttpRequest> CreateRequest(EHttpRequestType RequestType, FString Uri, FString JsonData = "");

	UFUNCTION(BlueprintCallable, Category = Http)
	void SendHttpRequest(const FString& Url, const FString& RequestContent);

	/*Called when the server has responded to our http request*/
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	int32 SendHTTPRequest(const FString& InURL, const FString& InVerb, const FString& InMessage, const TMap<FString, FString>& InHeaders);
	void  OnHttpRequestCompletedImpl(FHttpRequestPtr InRequest, FHttpResponsePtr InResponse, bool bWasSuccessful);

	UPROPERTY(BlueprintAssignable, Category = "Movie Render Pipeline")
	FHttpResponseRecieved HTTPResponseRecievedDelegate;

	struct FOutstandingRequest
	{
		FOutstandingRequest()
			: RequestIndex(-1), Request(nullptr)
		{
		}

		int32			RequestIndex;
		FHttpRequestPtr Request;
	};

	TArray<FOutstandingRequest> OutstandingRequests;
};
