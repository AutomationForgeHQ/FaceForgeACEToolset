// Typed promises for tools that take a while, which is most of them here.

#pragma once

#include "CoreMinimal.h"
#include "ACETypes.h"
#include "ToolsetRegistry/ToolCallAsyncResult.h"
#include "ACEAsyncResult.generated.h"

/**
 * An async tool call that completes with the whole readiness picture.
 *
 * The registry finds the schema by reflecting over the property literally named Value, which is why
 * this exists rather than reusing the string result: an agent asking why a face will not solve should
 * get the evidence as fields it can reason about - is Docker up, is there an engine for this card -
 * rather than a sentence it has to parse.
 */
UCLASS(BlueprintType)
class FACEFORGEACETOOLSET_API UToolCallAsyncResultACEStatus : public UToolCallAsyncResult
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "FaceForgeACE")
	bool SetValue(const FACEStatus& InValue)
	{
		return MaybeBroadcastSuccessfulCompletion(FACEStatus(InValue), Value);
	}

	UPROPERTY(BlueprintReadOnly, Category = "FaceForgeACE")
	FACEStatus Value;
};

/**
 * An async tool call that completes with a rented pod's state.
 *
 * Typed for one field in particular: the hourly cost. A number an agent can compare is what lets it
 * warn about a pod left running; a sentence is not.
 */
UCLASS(BlueprintType)
class FACEFORGEACETOOLSET_API UToolCallAsyncResultACEPod : public UToolCallAsyncResult
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "FaceForgeACE")
	bool SetValue(const FACEPodStatus& InValue)
	{
		return MaybeBroadcastSuccessfulCompletion(FACEPodStatus(InValue), Value);
	}

	UPROPERTY(BlueprintReadOnly, Category = "FaceForgeACE")
	FACEPodStatus Value;
};

/**
 * An async tool call that completes with whether the built image matches this plugin.
 *
 * Typed because the useful answer is an enum, not prose: `Drifted` is the one state that is silently
 * wrong, and an agent deciding whether a rebuild is warranted should read a field rather than pattern
 * match on a sentence.
 */
UCLASS(BlueprintType)
class FACEFORGEACETOOLSET_API UToolCallAsyncResultACEImage : public UToolCallAsyncResult
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "FaceForgeACE")
	bool SetValue(const FACEImageStatus& InValue)
	{
		return MaybeBroadcastSuccessfulCompletion(FACEImageStatus(InValue), Value);
	}

	UPROPERTY(BlueprintReadOnly, Category = "FaceForgeACE")
	FACEImageStatus Value;
};
