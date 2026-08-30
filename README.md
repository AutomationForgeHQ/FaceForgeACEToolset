# FaceForge ACE Toolset

**The Audio2Face runner, as Model Context Protocol tools.** Every function forwards to
`UACESubsystem` and adds nothing; all logic, state and safety live in `FaceForgeACE`, and deleting
this plugin changes nothing about how it behaves.

---

## What is here, and what deliberately is not

**Solving is not here.** A face bank pointed at the Audio2Face provider goes through FaceForge's own
tools like any other — which is the point of the provider abstraction. An agent that knows how to
solve a face should not have to learn a second way to do it because the model happens to be running
on this machine.

What is here is everything specific to *hosting a model yourself*: the container, the TensorRT engine
it has to build for each GPU, the reason it is not working, and the GPU it might be rented on.

**There is no tool for setting an API key.** An agent that can write secrets into the OS credential
vault is a liability with no matching benefit; entering a key is a human action performed once, in
Project Settings or on the hub's Keys page. Agents can ask whether a key exists, never set or read
one. That is a design decision, not an omission — please do not "fix" it.

## The tools

| | |
|---|---|
| **Get Audio2Face Status** | whether a face can be solved right now, and the one thing to do if not |
| **Get Audio2Face Runner Logs** | what the container said. The first thing to read when it will not start |
| **Prepare Audio2Face Model** | build this GPU's TensorRT engine now rather than inside the first solve |
| **Start / Stop / Remove Audio2Face Runner** | the container |
| **Rent Audio2Face Gpu** | rent one from Runpod and point this project at it. **Spends money** |
| **Get Audio2Face Pod Status** | what the rented pod is doing and what it costs |
| **Set Audio2Face Pod State** | start, stop or terminate it |

Status returns a struct rather than a sentence, because an agent asking why a face will not solve
should get evidence it can reason about — is Docker up, is there an engine for this card — rather than
prose it has to parse. Errors raise rather than returning an `ok: false` envelope, and every message
names the call to make next.

## The skills

Two, split by outcome rather than by subject, because they are consulted at different moments:

- **`ACEFaceSkill`** — choosing between FaceForge's two solvers, emotion as an *input* rather than
  only a reading of the audio, and where the curves have to land on a MetaHuman or an authored head.
  Read while authoring a scene.
- **`ACERunnerSkill`** — getting the container up, the engine build that is paid once per GPU, the
  four things that go wrong and their four different fixes, and renting a GPU without leaving one
  billing. Read while something is broken.

A single skill would make each reader wade through the other's half to find their own.

## Its own plugin, on purpose

`ToolsetRegistry` is Experimental. Folding this adapter into `FaceForgeACE` would mean the capability
refuses to load anywhere the experimental plugins are switched off — so the toolset is separate, and
depends on `ToolsetRegistry` rather than on `ModelContextProtocol`. MCP is one transport reading the
registry; coupling to it would be backwards.
