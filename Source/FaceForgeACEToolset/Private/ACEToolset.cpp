#include "ACEToolset.h"

#include "ACEAsyncResult.h"
#include "ACESubsystem.h"

#include "Kismet/KismetSystemLibrary.h"
#include "ToolsetRegistry/ToolCallAsyncResultString.h"

namespace ACEToolsetPrivate
{
	/**
	 * Every tool here is one call that takes seconds to minutes, so they all share this shape.
	 *
	 * Rooted because only the completion lambda refers to the result and the garbage collector cannot
	 * see that. The subsystem always invokes its callback, including on its own synchronous refusal
	 * paths, so every route through here unroots exactly once.
	 */
	static UToolCallAsyncResultString* RunStringTool(
		TFunction<void(UACESubsystem&, FACEOutcome)> Operation)
	{
		UToolCallAsyncResultString* Result = NewObject<UToolCallAsyncResultString>();
		Result->AddToRoot();

		UACESubsystem* Subsystem = UACESubsystem::Get();
		if (!Subsystem)
		{
			Result->SetError(TEXT(
				"FaceForge ACE is not available. The plugin is disabled, or this is not an editor session."));
			Result->RemoveFromRoot();
			return Result;
		}

		Operation(*Subsystem, [Result](bool bSucceeded, const FString& Message)
		{
			if (bSucceeded)
			{
				Result->SetValue(Message);
			}
			else
			{
				Result->SetError(Message);
			}

			Result->RemoveFromRoot();
		});

		return Result;
	}
}

UToolCallAsyncResultACEStatus* UACEToolset::GetAudio2FaceStatus()
{
	UToolCallAsyncResultACEStatus* Result = NewObject<UToolCallAsyncResultACEStatus>();
	Result->AddToRoot();

	UACESubsystem* Subsystem = UACESubsystem::Get();
	if (!Subsystem)
	{
		Result->SetError(TEXT("FaceForge ACE is not available."));
		Result->RemoveFromRoot();
		return Result;
	}

	Subsystem->GetStatus([Result](const FACEStatus& Status)
	{
		Result->SetValue(Status);
		Result->RemoveFromRoot();
	});

	return Result;
}

UToolCallAsyncResultACEImage* UACEToolset::GetAudio2FaceImageState()
{
	UToolCallAsyncResultACEImage* Result = NewObject<UToolCallAsyncResultACEImage>();
	Result->AddToRoot();

	UACESubsystem* Subsystem = UACESubsystem::Get();
	if (!Subsystem)
	{
		Result->SetError(TEXT("FaceForge ACE is not available."));
		Result->RemoveFromRoot();
		return Result;
	}

	Subsystem->GetImageState([Result](const FACEImageStatus& Image)
	{
		Result->SetValue(Image);
		Result->RemoveFromRoot();
	});

	return Result;
}

UToolCallAsyncResultString* UACEToolset::GetAudio2FaceRunnerLogs(int32 Lines)
{
	UToolCallAsyncResultString* Result = NewObject<UToolCallAsyncResultString>();
	Result->AddToRoot();

	UACESubsystem* Subsystem = UACESubsystem::Get();
	if (!Subsystem)
	{
		Result->SetError(TEXT("FaceForge ACE is not available."));
		Result->RemoveFromRoot();
		return Result;
	}

	Subsystem->GetRunnerLogs(Lines, [Result](const FString& Logs)
	{
		// Empty output is a real answer rather than a failure: a container that has written nothing is
		// usually one that has not started, which the status tool explains properly.
		Result->SetValue(Logs.IsEmpty()
			? TEXT("The runner container has written nothing. Get Audio2Face Status says whether it is "
			       "running at all.")
			: Logs);

		Result->RemoveFromRoot();
	});

	return Result;
}

UToolCallAsyncResultString* UACEToolset::PrepareAudio2FaceModel(const FString& ModelId)
{
	return ACEToolsetPrivate::RunStringTool(
		[ModelId](UACESubsystem& Subsystem, FACEOutcome OnComplete)
		{
			Subsystem.PrepareModel(ModelId, MoveTemp(OnComplete));
		});
}

UToolCallAsyncResultString* UACEToolset::StartAudio2FaceRunner()
{
	return ACEToolsetPrivate::RunStringTool(
		[](UACESubsystem& Subsystem, FACEOutcome OnComplete)
		{
			Subsystem.StartRunner(MoveTemp(OnComplete));
		});
}

UToolCallAsyncResultString* UACEToolset::StopAudio2FaceRunner()
{
	return ACEToolsetPrivate::RunStringTool(
		[](UACESubsystem& Subsystem, FACEOutcome OnComplete)
		{
			Subsystem.StopRunner(MoveTemp(OnComplete));
		});
}

UToolCallAsyncResultString* UACEToolset::RestartAudio2FaceRunner()
{
	return ACEToolsetPrivate::RunStringTool(
		[](UACESubsystem& Subsystem, FACEOutcome OnComplete)
		{
			Subsystem.RestartRunner(MoveTemp(OnComplete));
		});
}

UToolCallAsyncResultString* UACEToolset::RebuildAudio2FaceRunner()
{
	return ACEToolsetPrivate::RunStringTool(
		[](UACESubsystem& Subsystem, FACEOutcome OnComplete)
		{
			Subsystem.RebuildRunner(MoveTemp(OnComplete));
		});
}

UToolCallAsyncResultString* UACEToolset::RemoveAudio2FaceRunner()
{
	return ACEToolsetPrivate::RunStringTool(
		[](UACESubsystem& Subsystem, FACEOutcome OnComplete)
		{
			Subsystem.RemoveRunner(MoveTemp(OnComplete));
		});
}

UToolCallAsyncResultString* UACEToolset::RentAudio2FaceGpu()
{
	return ACEToolsetPrivate::RunStringTool(
		[](UACESubsystem& Subsystem, FACEOutcome OnComplete)
		{
			if (!Subsystem.HasRunpodApiKey())
			{
				// Refused here rather than after a failed API call, so the message names the human
				// action instead of reporting a 401.
				OnComplete(false, TEXT(
					"No Runpod API key is stored. A person sets it once - Project Settings > Automation "
					"Forge > FaceForge ACE, or the hub's Keys page under General, or the RUNPOD_API_KEY "
					"environment variable. It needs *write* access; a read-only key lists hardware and "
					"then creates nothing."));
				return;
			}

			Subsystem.CloudRent(MoveTemp(OnComplete));
		});
}

UToolCallAsyncResultACEPod* UACEToolset::GetAudio2FacePodStatus()
{
	UToolCallAsyncResultACEPod* Result = NewObject<UToolCallAsyncResultACEPod>();
	Result->AddToRoot();

	UACESubsystem* Subsystem = UACESubsystem::Get();
	if (!Subsystem)
	{
		Result->SetError(TEXT("FaceForge ACE is not available."));
		Result->RemoveFromRoot();
		return Result;
	}

	Subsystem->CloudStatus([Result](bool bOk, const FACEPodStatus& Pod, const FString& Message)
	{
		if (bOk)
		{
			Result->SetValue(Pod);
		}
		else
		{
			Result->SetError(Message);
		}

		Result->RemoveFromRoot();
	});

	return Result;
}

UToolCallAsyncResultString* UACEToolset::SetAudio2FacePodState(const FString& Action)
{
	return ACEToolsetPrivate::RunStringTool(
		[Action](UACESubsystem& Subsystem, FACEOutcome OnComplete)
		{
			Subsystem.CloudAction(Action, MoveTemp(OnComplete));
		});
}
