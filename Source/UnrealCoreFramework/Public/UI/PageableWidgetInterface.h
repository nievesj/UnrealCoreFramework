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

#include "UObject/Interface.h"

#include "PageableWidgetInterface.generated.h"

/** Interface for widgets that can be opened and closed as pages in the UI stack. */
UINTERFACE(MinimalAPI)
class UPageableWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

class UNREALCOREFRAMEWORK_API IPageableWidgetInterface
{
	GENERATED_BODY()

public:
	/** Open and activate this page. */
	virtual void Open();

	/** Close and deactivate this page. */
	virtual void Close();

	/** @return true if player input should be disabled while this page is open. */
	virtual bool GetDisablePlayerInput() const { return false; }
};