// The Audio2Face runner exposed as Model Context Protocol tools.

#pragma once

#include "CoreMinimal.h"
#include "ACETypes.h"
#include "ToolsetRegistry/ToolsetDefinition.h"
#include "ACEToolset.generated.h"

class UToolCallAsyncResultString;
class UToolCallAsyncResultACEStatus;
class UToolCallAsyncResultACEPod;

/**
 * Hosting Audio2Face yourself: the runner, its container, and the GPU it might be rented on.
 *
 * Every function forwards to `UACESubsystem` and adds nothing.
 *
 * **Solving is deliberately not here.** A face bank pointed at the Audio2Face provider goes through
 * FaceForge's own tools like any other, which is the point of the provider abstraction - an agent that
 * knows how to solve a face does not need to learn a second way to do it because the model happens to
 * be running on this machine.
 *
 * What is here is everything specific to running a model yourself, and the reason it is not working.
 * That last one matters most: local inference fails for a handful of mundane reasons - Docker closed,
 * no GPU passthrough, an engine not built for this card - and every one is fixable in seconds by
 * somebody who is told which it is.
 *
 * **There is no tool for setting an API key, and that is a design decision rather than an omission.**
 * An agent that can write secrets into the OS credential vault is a liability with no matching
 * benefit; entering a key is a human action performed once, in Project Settings or on the hub's Keys
 * page. Agents can ask whether a key exists, never set or read one.
 */
UCLASS(BlueprintType)
class FACEFORGEACETOOLSET_API UACEToolset : public UToolsetDefinition
{
	GENERATED_BODY()

public:

	virtual FString GetToolsetVersion() const override { return TEXT("0.1"); }

	// ---------------------------------------------------------------------------------------------
	// Where things stand
	// ---------------------------------------------------------------------------------------------

	/**
	 * Whether a face can be solved right now, and if not, the one thing to do about it.
	 *
	 * **Call this first, and read `Advice` before anything else.** Every other tool here exists to act
	 * on something this reports. A model whose engine is not built is not broken, it is unfinished;
	 * emotion detection being off is normal rather than a fault, because Audio2Face's own models are
	 * ungated and only Audio2Emotion needs an account.
	 *
	 * @return Runner, Docker, GPU, the installed models and their engines, and any rented pod.
	 */
	UFUNCTION(meta = (AICallable), Category = "FaceForgeACE|Discovery")
	static UToolCallAsyncResultACEStatus* GetAudio2FaceStatus();

	/**
	 * Recent output from the runner container. The first thing to read when it will not start.
	 *
	 * Local only - a runner on another machine writes its logs there.
	 *
	 * @param Lines How many lines from the end. Zero means forty.
	 */
	UFUNCTION(meta = (AICallable), Category = "FaceForgeACE|Discovery")
	static UToolCallAsyncResultString* GetAudio2FaceRunnerLogs(int32 Lines);

	// ---------------------------------------------------------------------------------------------
	// Getting it ready
	// ---------------------------------------------------------------------------------------------

	/**
	 * Build a model's TensorRT engine for this GPU now, rather than inside the first solve.
	 *
	 * An engine is compiled for one GPU architecture and one TensorRT version, so it cannot ship in
	 * the image and cannot move between machines. About forty seconds, paid once per card. Worth doing
	 * deliberately before a batch instead of discovering it in the middle of one.
	 *
	 * Returns as soon as the build is *under way*. Ask Get Audio2Face Status for when it is done.
	 *
	 * @param ModelId A model id from Get Audio2Face Status. Empty uses the project default.
	 */
	UFUNCTION(meta = (AICallable), Category = "FaceForgeACE|Runner")
	static UToolCallAsyncResultString* PrepareAudio2FaceModel(const FString& ModelId);

	/**
	 * Build the runner image if it is missing, then start the container.
	 *
	 * **Minutes on a first run** - 5.6GB pulled, unpacking to about 15GB, then a compile of NVIDIA's
	 * SDK - and
	 * seconds afterwards. Refuses when the configured runner is not on this machine: a remote URL is
	 * somebody else's container and this plugin does not manage it.
	 */
	UFUNCTION(meta = (AICallable), Category = "FaceForgeACE|Runner")
	static UToolCallAsyncResultString* StartAudio2FaceRunner();

	/** Stop the container. It keeps its TensorRT engine cache, so starting again is seconds. */
	UFUNCTION(meta = (AICallable), Category = "FaceForgeACE|Runner")
	static UToolCallAsyncResultString* StopAudio2FaceRunner();

	/**
	 * Stop the container and start it again. Seconds, and it rebuilds nothing.
	 *
	 * The runner reads its Hugging Face token at startup, so this is what puts a token entered while
	 * it was running into effect. Starting an already-running container does nothing.
	 */
	UFUNCTION(meta = (AICallable), Category = "FaceForgeACE|Runner")
	static UToolCallAsyncResultString* RestartAudio2FaceRunner();

	/**
	 * Whether the built image was made from the runner files this plugin ships.
	 *
	 * **Ask this before suggesting a rebuild.** A drifted image is the only one that is silently
	 * wrong: it starts, answers and solves, in the way an older version of this plugin did. The other
	 * three states - none built, current, or an image carrying no signature - are all fine and want
	 * different sentences.
	 */
	UFUNCTION(meta = (AICallable), Category = "FaceForgeACE|Runner")
	static UToolCallAsyncResultACEImage* GetAudio2FaceImageState();

	/**
	 * Remove the container and build the image again. The only tool here that rebuilds.
	 *
	 * **Minutes**, so it is not a way to restart something. Get Audio2Face Image State says whether it
	 * is warranted; a current image being rebuilt is fifteen gigabytes of nothing. The TensorRT engine
	 * lives on a named volume and is untouched, so the first solve afterwards does not build one.
	 */
	UFUNCTION(meta = (AICallable), Category = "FaceForgeACE|Runner")
	static UToolCallAsyncResultString* RebuildAudio2FaceRunner();

	/**
	 * Remove the container. The image and the engine cache survive, so this is not a reinstall.
	 *
	 * Reach for it when the container itself is wrong - a changed compose file, a half-created
	 * container - rather than as a way to stop it. Stop is the way to stop it.
	 */
	UFUNCTION(meta = (AICallable), Category = "FaceForgeACE|Runner")
	static UToolCallAsyncResultString* RemoveAudio2FaceRunner();

	// ---------------------------------------------------------------------------------------------
	// Renting one
	// ---------------------------------------------------------------------------------------------

	/**
	 * Rent a GPU from Runpod, start the runner on it, and point this project at it.
	 *
	 * **This spends money from the moment it succeeds, and keeps spending while the pod runs whether
	 * or not anything is solving.** Ask the user before calling it, and tell them the hourly rate the
	 * result reports.
	 *
	 * Worth knowing before suggesting it: a face solve is a fraction of a second, so an hour of
	 * finished dialogue is about four GPU-minutes, while a cold pod is five to fifteen. Renting is for
	 * someone with no NVIDIA card, or for a whole-game batch - not for a handful of lines, and never
	 * as a way to make a solve faster.
	 *
	 * Needs a Runpod key with *write* access, which a human sets. Get Audio2Face Status says whether
	 * one is present.
	 */
	UFUNCTION(meta = (AICallable), Category = "FaceForgeACE|Cloud")
	static UToolCallAsyncResultString* RentAudio2FaceGpu();

	/** What the rented pod is doing and what it costs. Empty pod id when nothing is rented. */
	UFUNCTION(meta = (AICallable), Category = "FaceForgeACE|Cloud")
	static UToolCallAsyncResultACEPod* GetAudio2FacePodStatus();

	/**
	 * Start, stop or terminate the rented pod.
	 *
	 * **Stop is the one to reach for**, and it is the difference between pennies and a surprise bill:
	 * a stopped pod costs nothing and starts again in seconds with the same URL.
	 *
	 * **Terminate is irreversible.** It releases the hardware for good and is the only way to stop
	 * paying entirely. Ask the user first, every time.
	 *
	 * @param Action start, stop or terminate.
	 */
	UFUNCTION(meta = (AICallable), Category = "FaceForgeACE|Cloud")
	static UToolCallAsyncResultString* SetAudio2FacePodState(const FString& Action);
};
