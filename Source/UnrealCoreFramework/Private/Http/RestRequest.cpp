// Fill out your copyright notice in the Description page of Project Settings.

#include "Http/RestRequest.h"

#include "Async/CoreAsyncTypes.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "AsyncFlow.h"
#include "AsyncFlowHttpAwaiter.h"

void URestRequest::Request(EHttpRequestType RequestType, FString Uri, FString JsonData)
{
	ActiveRequestTask = RequestTask(RequestType, Uri, JsonData);
	ActiveRequestTask.SetDebugName(FString::Printf(TEXT("HttpRequest_%s_%s"),
		RequestType == EHttpRequestType::GET ? TEXT("GET") :
		RequestType == EHttpRequestType::PUT ? TEXT("PUT") : TEXT("POST"),
		*Uri));
	ActiveRequestTask.Start();
}

AsyncFlow::TTask<FHttpResponsePtr> URestRequest::RequestTask(
	EHttpRequestType RequestType, const FString& Uri, const FString& JsonData)
{
	UCF_ASYNC_CONTRACT(this);

	TSharedRef<IHttpRequest> HttpRequest = CreateRequest(RequestType, Uri, JsonData);

	TTuple<FHttpResponsePtr, bool> Result = co_await AsyncFlow::ProcessHttpRequest(HttpRequest);
	FHttpResponsePtr Response = Result.Get<0>();
	const bool bSuccess = Result.Get<1>();

	if (bSuccess && Response.IsValid())
	{
		const int32 ResponseCode = Response->GetResponseCode();
		const FString ResponseBody = Response->GetContentAsString();
		HTTPResponseRecievedDelegate.Broadcast(0, ResponseCode, ResponseBody);

		UE_LOG(LogTemp, Log, TEXT("HTTP %d: %s"),
			ResponseCode, *ResponseBody.Left(200));
	}
	else
	{
		HTTPResponseRecievedDelegate.Broadcast(0, 0, TEXT("Request failed"));

		UE_LOG(LogTemp, Warning, TEXT("HTTP request failed for: %s"), *Uri);
	}

	co_return Response;
}

TSharedRef<IHttpRequest> URestRequest::CreateRequest(EHttpRequestType RequestType, const FString& Uri, const FString& JsonData)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Uri);
	Request->SetHeader(TEXT("User-Agent"), TEXT("X-UnrealEngine-Agent"));

	switch (RequestType)
	{
		case EHttpRequestType::GET:
			Request->SetVerb(TEXT("GET"));
			break;
		case EHttpRequestType::PUT:
			Request->SetVerb(TEXT("PUT"));
			Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
			Request->SetContentAsString(JsonData);
			break;
		case EHttpRequestType::POST:
			Request->SetVerb(TEXT("POST"));
			Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
			Request->SetContentAsString(JsonData);
			break;
		default:
			break;
	}

	return Request;
}

