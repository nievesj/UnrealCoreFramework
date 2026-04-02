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

#include "Http/RestRequest.h"

#include "Async/CoreAsyncTypes.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "AsyncFlow.h"
#include "AsyncFlowHttpAwaiter.h"

DEFINE_LOG_CATEGORY(LogCoreRestRequest);

void URestRequest::Request(EHttpRequestType RequestType, FString Uri, FString JsonData)
{
	ActiveRequestTask = RequestTask(RequestType, Uri, JsonData);
	ActiveRequestTask.SetDebugName(FString::Printf(TEXT("HttpRequest_%s_%s"),
		RequestType == EHttpRequestType::GET ? TEXT("GET") : RequestType == EHttpRequestType::PUT ? TEXT("PUT")
																								  : TEXT("POST"),
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

		UE_LOG(LogCoreRestRequest, Log, TEXT("HTTP %d: %s"), ResponseCode, *ResponseBody.Left(200));
	}
	else
	{
		HTTPResponseRecievedDelegate.Broadcast(0, 0, TEXT("Request failed"));

		UE_LOG(LogCoreRestRequest, Warning, TEXT("HTTP request failed for: %s"), *Uri);
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
