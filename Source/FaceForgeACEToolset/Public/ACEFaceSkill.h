// Choosing Audio2Face over the engine's own solver, and getting a performance out of it.

#pragma once

#include "CoreMinimal.h"
#include "ToolsetRegistry/AgentSkill.h"
#include "ACEFaceSkill.generated.h"

/**
 * The solver, not the machine it runs on. Its sibling covers keeping the runner alive.
 *
 * Split that way because they are two different jobs with two different audiences: this one is
 * consulted while authoring a scene, the other while something is broken, and a single skill would
 * make each of them read the other's half to find its own.
 */
UCLASS()
class FACEFORGEACETOOLSET_API UACEFaceSkill : public UAgentSkill
{
	GENERATED_BODY()

public:

	UACEFaceSkill()
	{
		Description = TEXT(
			"Choosing between FaceForge's two face solvers, and directing a performance with NVIDIA "
			"Audio2Face: emotion as an input rather than only a reading of the audio, and what it "
			"takes to land ARKit curves on a MetaHuman or an authored head.");

		Instructions = TEXT(
			"FaceForge has two solvers and they are not ranked. Which one is right depends on the "
			"character and the line, and picking by habit is how a scene ends up with the wrong "
			"one.\n"
			"\n"
			"WHICH SOLVER\n"
			"\n"
			"The engine's own solver is free, runs on the CPU, needs nothing installed, and produces "
			"head motion as well as a face. Reach for it when there is no NVIDIA card, when the head "
			"is part of the performance, or when a single line needs solving and setting up a "
			"container would cost more than the result is worth.\n"
			"\n"
			"Audio2Face needs a GPU and a running container, and buys two things with that. It emits "
			"ARKit blendshapes natively, so a Character Creator, Reallusion, Daz or authored head "
			"needs no translation at all - where the engine solver needs a lossy one that leaves a "
			"quarter of the shapes undriven. And it takes emotion as an *input*, which the other "
			"cannot do at all. It produces no head motion; if the head is the performance, that "
			"decides it.\n"
			"\n"
			"Say which you chose and why, once, rather than silently defaulting.\n"
			"\n"
			"EMOTION IS AN INPUT, AND THAT IS THE INTERESTING PART\n"
			"\n"
			"The engine solver reads a performance out of the audio and that is all it can do. "
			"Audio2Face blends what it hears against what you ask for, so a line can be angrier than "
			"it was read - because the player just failed the quest, because the scene turned - "
			"without regenerating a second of audio. When a scene's meaning depends on state the "
			"voice recording could not know about, that is the tool for it, and it is worth offering "
			"unprompted.\n"
			"\n"
			"Its emotions are a set of named sliders that mix, not one choice from a list. A clip's "
			"mood maps onto them, and the mapping is lossy in a way worth knowing: several of "
			"FaceForge's moods came from the other solver's vocabulary and have no counterpart here. "
			"Those solve neutral and say so in the log. If a line needs one of them, either accept a "
			"neutral emotional read with correct lip-sync, or pick the nearest emotion the model "
			"actually has and be explicit that you substituted.\n"
			"\n"
			"Emotion detection - reading the emotion out of the audio automatically - is a separate, "
			"optional model that needs an account, and it is off unless somebody set one up. Emotion "
			"you specify yourself always works. Do not treat its absence as a broken install; the "
			"status tool explains it in one line, and tells apart having no token from having a token "
			"on an account that never accepted the model's terms - which look identical and are fixed "
			"in different places.\n"
			"\n"
			"With detection on, the two are not a choice between each other. A mood of Auto Detect lets "
			"the audio decide; a named mood overrides it; and a project setting makes a named mood a "
			"*lean* on the detected one instead - the difference between telling an actor to be angry "
			"and telling them to play it angrier. Asking for detection is always safe: a runner without "
			"the model solves the line anyway with whatever emotion it was given and says why in the "
			"clip's warning. That matters because Auto Detect is the default mood, so anything stricter "
			"would break every bank that never asked for emotion at all.\n"
			"\n"
			"WHERE THE CURVES HAVE TO LAND\n"
			"\n"
			"A solved clip is a set of named curves and nothing about the numbers says whether the "
			"names mean anything to the character. Curves in the wrong vocabulary import cleanly, "
			"evaluate to zero, animate nothing and warn nobody, so this is the failure to design "
			"against rather than to debug.\n"
			"\n"
			"An ARKit head needs no translation. A MetaHuman does, because its rig reads a different "
			"control set - and the engine ships that translation as data rather than anybody having "
			"to invent it. Read that asset into a curve mapping and set it on the bank; do not write "
			"mapping rules for a MetaHuman by hand, because you would be guessing at a rig's internal "
			"expression logic from the outside and the result looks plausible while being wrong.\n"
			"\n"
			"Before baking anything, ask for the coverage report and read what it says. A face that "
			"drives nothing looks exactly like a character who chose not to move, and the report is "
			"the only thing that tells the two apart before somebody watches a rendered take.\n"
			"\n"
			"WHAT IT COSTS\n"
			"\n"
			"Nothing, on your own machine, and that inverts the instinct a paid voice provider "
			"teaches. There is no bill and no quota, so solving a line three ways to compare them is "
			"free and worth suggesting. What is scarce is wall-clock, and not much of it: a line of "
			"dialogue is a fraction of a second once the model is loaded.\n"
			"\n"
			"Solving and baking are separate on purpose, and the asymmetry is the point. Solving is "
			"the expensive half and depends only on the audio; baking is milliseconds and depends on "
			"the character. Putting the same clip on a second character is a re-bake, never a "
			"re-solve, so never re-solve to change a target.\n"
			"\n"
			"A solved clip that somebody has since edited or captured by hand is not yours to "
			"overwrite. If the audio moved underneath one of those, say so and stop - that is a "
			"person's decision, and it is the only line in any report with a real cost attached.");
	}
};
