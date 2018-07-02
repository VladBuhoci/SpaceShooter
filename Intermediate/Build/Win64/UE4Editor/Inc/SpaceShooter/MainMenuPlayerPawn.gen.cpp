// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "GeneratedCppIncludes.h"
#include "Public/MainMenuPlayerPawn.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeMainMenuPlayerPawn() {}
// Cross Module References
	SPACESHOOTER_API UClass* Z_Construct_UClass_AMainMenuPlayerPawn_NoRegister();
	SPACESHOOTER_API UClass* Z_Construct_UClass_AMainMenuPlayerPawn();
	ENGINE_API UClass* Z_Construct_UClass_APawn();
	UPackage* Z_Construct_UPackage__Script_SpaceShooter();
// End Cross Module References
	void AMainMenuPlayerPawn::StaticRegisterNativesAMainMenuPlayerPawn()
	{
	}
	UClass* Z_Construct_UClass_AMainMenuPlayerPawn_NoRegister()
	{
		return AMainMenuPlayerPawn::StaticClass();
	}
	UClass* Z_Construct_UClass_AMainMenuPlayerPawn()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			static UObject* (*const DependentSingletons[])() = {
				(UObject* (*)())Z_Construct_UClass_APawn,
				(UObject* (*)())Z_Construct_UPackage__Script_SpaceShooter,
			};
#if WITH_METADATA
			static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[] = {
				{ "HideCategories", "Navigation" },
				{ "IncludePath", "MainMenuPlayerPawn.h" },
				{ "ModuleRelativePath", "Public/MainMenuPlayerPawn.h" },
			};
#endif
			static const FCppClassTypeInfoStatic StaticCppClassTypeInfo = {
				TCppClassTypeTraits<AMainMenuPlayerPawn>::IsAbstract,
			};
			static const UE4CodeGen_Private::FClassParams ClassParams = {
				&AMainMenuPlayerPawn::StaticClass,
				DependentSingletons, ARRAY_COUNT(DependentSingletons),
				0x00900080u,
				nullptr, 0,
				nullptr, 0,
				nullptr,
				&StaticCppClassTypeInfo,
				nullptr, 0,
				METADATA_PARAMS(Class_MetaDataParams, ARRAY_COUNT(Class_MetaDataParams))
			};
			UE4CodeGen_Private::ConstructUClass(OuterClass, ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AMainMenuPlayerPawn, 3963317194);
	static FCompiledInDefer Z_CompiledInDefer_UClass_AMainMenuPlayerPawn(Z_Construct_UClass_AMainMenuPlayerPawn, &AMainMenuPlayerPawn::StaticClass, TEXT("/Script/SpaceShooter"), TEXT("AMainMenuPlayerPawn"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AMainMenuPlayerPawn);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
