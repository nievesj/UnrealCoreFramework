// Fill out your copyright notice in the Description page of Project Settings.

#include "Http/RestRequest.h"

#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"

void URestRequest::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		GLog->Log("Hey we received the following response!");
		GLog->Log(Response->GetContentAsString());
	}
}

void URestRequest::Request(EHttpRequestType RequestType, FString Uri, FString JsonData)
{
	TSharedRef<IHttpRequest> Request = CreateRequest(RequestType, Uri, JsonData);
	Request->OnProcessRequestComplete().BindUObject(this, &URestRequest::OnResponseReceived);
	Request->ProcessRequest();
}

TSharedRef<IHttpRequest> URestRequest::CreateRequest(EHttpRequestType RequestType, FString Uri, FString JsonData)
{
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(Uri);
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");
	switch (RequestType)
	{
		case EHttpRequestType::GET:
			Request->SetVerb(TEXT("GET"));
			break;
		case EHttpRequestType::PUT:
			Request->SetVerb(TEXT("PUT"));
			Request->SetHeader("Content-Type", TEXT("application/json"));
			Request->SetContentAsString(*JsonData);
			break;
		case EHttpRequestType::POST:
			Request->SetVerb(TEXT("POST"));
			Request->SetHeader("Content-Type", TEXT("application/json"));
			Request->SetContentAsString(*JsonData);
			break;
	}
	// FJsonObjectConverter::JsonObjectStringToUStruct()

	// Request->SetHeader(TEXT("Accepts"), TEXT("application/json"));
	// Request->SetHeader(AuthorizationHeader, AuthorizationHash);

	return Request;
}

void URestRequest::SendHttpRequest(const FString& Url, const FString& RequestContent)
{
	// Creating a request using UE4's Http Module
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();

	// Binding a function that will fire when we receive a response from our request
	Request->OnProcessRequestComplete().BindUObject(this, &URestRequest::OnResponseReceived);

	// This is the url on which to process the request
	Request->SetURL(Url);
	// We're sending data so set the Http method to POST
	Request->SetVerb("POST");

	// Tell the host that we're an unreal agent
	Request->SetHeader(TEXT("User-Agent"), "X-UnrealEngine-Agent");

	// In case you're sending json you can also make use of headers using the following command.
	// Just make sure to convert the Json to string and use the SetContentAsString.
	// Request->SetHeader("Content-Type", TEXT("application/json"));
	// Use the following command in case you want to send a string value to the server
	// Request->SetContentAsString("Hello kind server.");

	// Send the request
	Request->ProcessRequest();
}
