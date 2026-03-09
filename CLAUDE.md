# GetGud C++ SDK

Client library for game developers to send telemetry data to GetGud middleware.

## Commands
```
mkdir build && cd build
cmake ..
make
```

## Architecture
- `include/GetgudSDK.h` — public C++ API
- `include/GetgudSDK_C.h` — C wrapper API
- `src/actions/` — game action types (spawn, damage, position, etc.)
- `src/actions_buffer/` — buffering and batching of actions
- `src/senders/` — HTTP transport to middleware
- `src/game_container/` — game/match state management
- `src/config/` — SDK configuration from config.json
- `examples/` — integration examples
- `tests/` — unit tests

## Key Patterns
- SDK buffers actions and sends them in batches to middleware's `/api/game_stream/`
- Config loaded from `config.json` (endpoint URLs, buffer sizes, timeouts)
- Thread-safe design — game devs call SDK from game loop threads
