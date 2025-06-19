// Unreal Core Framework | Copyright (c) 2024 José M. Nieves - MIT License.

#pragma once

#include "Engine/DataTable.h"

#include "DataCoreFrameworkTypes.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct FCoreTableRowBase : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient, BlueprintReadOnly)
	FDataTableRowHandle SelfTableRowHandle;

protected:
	virtual void OnDataTableChanged(const UDataTable* InDataTable, const FName InRowName) override
	{
		SelfTableRowHandle.DataTable = InDataTable;
		SelfTableRowHandle.RowName = InRowName;
	}
};
