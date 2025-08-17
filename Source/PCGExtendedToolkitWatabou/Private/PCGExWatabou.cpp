// Copyright 2025 Timothé Lapetite and contributors
// Released under the MIT license https://opensource.org/license/MIT/

#pragma once

#include "PCGExWatabou.h"
#include "CoreMinimal.h"

FPCGExWatabouVersion::FPCGExWatabouVersion(const FString& InSemVer)
{
	// Cut off prerelease/build metadata
	FString CleanSemVer;
	if (!InSemVer.Split(TEXT("-"), &CleanSemVer, nullptr))
	{
		CleanSemVer = InSemVer;
	}

	TArray<FString> Parts;
	CleanSemVer.ParseIntoArray(Parts, TEXT("."));

	int32 Major = 0, Minor = 0, Patch = 0;
	if (Parts.Num() > 0) Major = FCString::Atoi(*Parts[0]);
	if (Parts.Num() > 1) Minor = FCString::Atoi(*Parts[1]);
	if (Parts.Num() > 2) Patch = FCString::Atoi(*Parts[2]);

	// Pack into a single int for easy comparison
	Version = (Major << 16) | (Minor << 8) | Patch;
}

FPCGExWatabouVersion::FPCGExWatabouVersion(const int32& InVersion)
	: Version(InVersion)
{
}

namespace PCGExWatabou
{
}
