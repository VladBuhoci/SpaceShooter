// This application is the final year project (2018-2019) of a Computer Science student (me - Vlad Buhoci).

#pragma once

#include "CoreMinimal.h"
#include "ItemAttributes.generated.h"

// Forward declarations.
class UTexture2D;


USTRUCT(BlueprintType)
struct FItemAttribute_Float
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Attribute")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Attribute")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Attribute")
	float Value;

	FItemAttribute_Float()
	{}

	FItemAttribute_Float(FText _Name, float _Value)
		: Icon(nullptr), Name(_Name), Value(_Value)
	{}

	FItemAttribute_Float(UTexture2D* _Icon, FText _Name, float _Value)
		: Icon(_Icon), Name(_Name), Value(_Value)
	{}
};

USTRUCT(BlueprintType)
struct FItemAttribute_IntervalInt
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Attribute")
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Attribute")
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Attribute")
	int32 ValueLow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item | Attribute")
	int32 ValueHigh;

	FItemAttribute_IntervalInt()
	{}

	FItemAttribute_IntervalInt(FText _Name, int32 _ValueLow, int32 _ValueHigh)
		: Icon(nullptr), Name(_Name), ValueLow(_ValueLow), ValueHigh(_ValueHigh)
	{}

	FItemAttribute_IntervalInt(UTexture2D* _Icon, FText _Name, int32 _ValueLow, int32 _ValueHigh)
		: Icon(_Icon), Name(_Name), ValueLow(_ValueLow), ValueHigh(_ValueHigh)
	{}
};
