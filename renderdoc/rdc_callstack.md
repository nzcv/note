[28 tools called]

## Python Callstack Workflow

- RenderDoc only writes CPU callstack addresses when the capture options `captureCallstacks` (and optionally `captureCallstacksOnlyActions`) are set before the capture is made. These options correspond to the Python `renderdoc.CaptureOptions` members shown below, so be sure to enable them via the injected application or when launching through the Python API:

```114:134:api/replay/capture_options.h
  DOCUMENT(R"(Capture CPU callstacks for API events
...
  bool captureCallstacks;
  DOCUMENT(R"(When capturing CPU callstacks, only capture them from actions.
...
  bool captureCallstacksOnlyActions;
```

- After opening a capture file or connecting to a remote server, the shared `ICaptureAccess` interface exposes the methods you need: `HasCallstacks()` lets you check whether the capture actually contains stacks, `InitResolver()` loads the resolve database and symbols, and `GetResolve()` turns the raw addresses into human-readable frames once the resolver is ready. The relevant API surface is documented here:

```1295:1328:api/replay/renderdoc_replay.h
  virtual bool HasCallstacks() = 0;
  virtual ResultDetails InitResolver(bool interactive, RENDERDOC_ProgressCallback progress) = 0;
  virtual rdcarray<rdcstr> GetResolve(const rdcarray<uint64_t> &callstack) = 0;
```

- The per-event callstack data itself lives on each structured-data chunk’s metadata. Every `ActionDescription` contains a list of `APIEvent`s (each one points at a chunk index), and each chunk’s `metadata.callstack` is exactly the array you pass to `GetResolve()`:

```252:301:api/replay/structured_data.h
.. data:: HasCallstack
  This chunk has a callstack.
...
struct SDChunkMetaData
{
  uint32_t chunkID = 0;
  SDChunkFlags flags = SDChunkFlags::NoFlags;
  ...
  rdcarray<uint64_t> callstack;
};
```

- Putting that together in Python typically looks like this:

```python
import renderdoc as rd

cap = rd.OpenCaptureFile()
res = cap.OpenFile(r"c:\captures\demo.rdc", "", None)
if res != rd.ResultCode.Succeeded:
    raise RuntimeError(res.message)

if not cap.HasCallstacks():
    print("Capture has no callstacks")
else:
    cap.InitResolver(interactive=False, progress=None)

    replay_controller = cap.OpenCapture(rd.ReplayOptions(), None)[1]
    structured = replay_controller.GetStructuredFile()

    def resolve_action(action: rd.ActionDescription):
        for evt in action.events:
            if evt.chunkIndex == rd.APIEvent.NoChunk:
                continue
            frames = structured.chunks[evt.chunkIndex].metadata.callstack
            if frames:
                return cap.GetResolve(frames)
        return []

    first_draw = replay_controller.GetRootActions()[0]
    print(resolve_action(first_draw))
```

  - `structured.chunks[evt.chunkIndex].metadata.callstack` yields the raw addresses.
  - Pass that list to `cap.GetResolve(...)` (or to a remote server handle) **after** `InitResolver` succeeds.
  - If you only want action-level callstacks, iterate `ActionDescription` objects from `replay_controller.GetRootActions()` (and descend into `children` as needed).

- Verification tips: ensure symbols for your platform are accessible before calling `InitResolver(False, …)`; if you need interactive selection of PDBs/dSYMs, pass `interactive=True`. Always guard the resolve path with `HasCallstacks()` so you can bail out early on captures where the option wasn’t enabled.
