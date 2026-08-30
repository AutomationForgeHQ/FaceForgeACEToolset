// Getting the Audio2Face runner working, and keeping a rented one from costing money.

#pragma once

#include "CoreMinimal.h"
#include "ToolsetRegistry/AgentSkill.h"
#include "ACERunnerSkill.generated.h"

/**
 * The machine, not the solver. Its sibling covers choosing and directing the solver itself.
 *
 * Everything here is about a container and a GPU: what "not working" usually means, what the first
 * run costs, and the one operation in the family that spends money continuously.
 */
UCLASS()
class FACEFORGEACETOOLSET_API UACERunnerSkill : public UAgentSkill
{
	GENERATED_BODY()

public:

	UACERunnerSkill()
	{
		Description = TEXT(
			"Running NVIDIA Audio2Face yourself: setting up its Docker container, the TensorRT engine "
			"it has to build for each GPU, diagnosing it when it will not start, and renting a GPU "
			"without leaving one billing.");

		Instructions = TEXT(
			"Audio2Face runs in a container on a machine with an NVIDIA GPU. That machine can be this "
			"one, a colleague's, or a rented one, and only the first is ours to start and stop - a "
			"runner at somebody else's address is their container, and trying to manage it is how you "
			"produce a confident error about a machine you cannot see.\n"
			"\n"
			"START BY ASKING, NOT BY FIXING\n"
			"\n"
			"There is one status call and it answers every question worth asking at once, because the "
			"questions are not independent: there is no point discussing a model on a machine whose "
			"Docker is closed. It reports a single next action. Do that one thing rather than "
			"reasoning from the parts, and re-ask afterwards.\n"
			"\n"
			"Four things go wrong and they need four different fixes. Docker not installed. Docker "
			"installed but closed - which on Windows means the desktop application is not open, and "
			"is the most common by a distance. The container not built or not running. And an engine "
			"not yet compiled for this card. Only the last of those is specific to this plugin, and "
			"none of them is a broken install.\n"
			"\n"
			"THE ENGINE IS A REAL WAIT, ONCE PER CARD\n"
			"\n"
			"The models ship inside the image, but the compiled form they actually run as is built for "
			"one GPU architecture and one library version, so it cannot ship and cannot move between "
			"machines. The first solve on a new card pays about forty seconds for it.\n"
			"\n"
			"That is worth paying deliberately before a batch rather than discovering inside one. Ask "
			"for it, then carry on with something else - the call returns as soon as the build is "
			"under way, not when it finishes, and the status call is how you learn it is done. A model "
			"without an engine is unfinished, not broken, and describing it as a failure sends "
			"somebody looking for a problem that is not there.\n"
			"\n"
			"AN IMAGE THAT DRIFTED IS THE ONLY ONE THAT IS SILENTLY WRONG\n"
			"\n"
			"The runner image carries a signature, a hash of the files that build it, so the plugin can "
			"tell the image it expects from the image that is actually here. Four answers, and only one is "
			"a problem: none built yet, current, an image carrying no signature, or drifted.\n"
			"\n"
			"Drifted means it was built from different runner files than the plugin now ships. It starts, "
			"it answers, it solves - the way an older version of the plugin did. Nothing errors, which is "
			"exactly why it is worth asking about instead of waiting to notice.\n"
			"\n"
			"So ask for the image state before ever suggesting a rebuild. Rebuilding a current image is "
			"gigabytes and many minutes of achieving nothing, and rebuild-it-and-see is the advice this "
			"check exists to replace.\n"
			"\n"
			"Restarting is the cheap thing and rebuilding is not. A restart takes seconds and rebuilds "
			"nothing, and it is what puts a newly entered token into effect, because the container reads "
			"its token only when it starts.\n"
			"\n"
			"NO ACCOUNT IS NEEDED, AND SAYING SO SAVES AN AFTERNOON\n"
			"\n"
			"Audio2Face's own models are ungated and built into the image, so lip-sync and any emotion "
			"you specify need no account, no token and no licence click. Only automatic emotion "
			"*detection* needs one, because that is a separate gated model.\n"
			"\n"
			"When it is unavailable, the reason names the exact step. Repeat it rather than "
			"improvising: for a gated model a token is not access, and having one without the other "
			"is the failure that looks like a bug. Everything installs perfectly and the feature is "
			"silently absent.\n"
			"\n"
			"RENTING SPENDS MONEY CONTINUOUSLY\n"
			"\n"
			"A rented GPU bills for every minute it runs, whether or not anything is solving. This is "
			"the only thing in the toolset with that property, and it is worth being blunt about "
			"before renting rather than after.\n"
			"\n"
			"The economics are not what they look like. A face solve is a fraction of a second, so an "
			"hour of finished dialogue is a few GPU-minutes - while a cold pod is five to fifteen "
			"before it can do anything at all. Renting is therefore for somebody with no NVIDIA card, "
			"or for a whole game's worth of lines in one sitting. It is never a way to make a solve "
			"faster, and a fast expensive card buys nothing: the wall-clock is the pod starting up.\n"
			"\n"
			"So: ask before renting, name the hourly rate, and stop it when the batch is done. "
			"Stopping is the important one and it is nearly free - a stopped pod costs nothing and "
			"comes back in seconds at the same address. Releasing it is irreversible and is the only "
			"way to stop paying entirely; ask before that, every time. If the conversation ends with "
			"a pod running, say so as the last thing you say.\n"
			"\n"
			"A key with only read access is the quiet failure here: it lists hardware happily and then "
			"creates nothing. Entering it is a human action - you can ask whether one exists, never "
			"set one.");
	}
};
