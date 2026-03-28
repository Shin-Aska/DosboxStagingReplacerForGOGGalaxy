---
apply: always
---

# Dosbox Staging Replacer Project Context

Dosbox Staging Replacer for GOG Galaxy is a native C++ command-line tool that updates GOG Galaxy DOS game launch entries to use a user-selected DOSBox executable, especially DOSBox Staging. It also edits affected DOSBox config files, resolves relative paths, and supports backup and restore of the Galaxy database.

## Tech Stack

- C++20
- CMake 3.30+
- SQLite amalgamation (`libs/sqlite`)
- `argparse` single-header CLI parsing (`libs/argparse/argparse.hpp`)
- Windows-first filesystem and installation discovery logic

## Project Structure

```text
main.cpp                         # CLI entrypoint and operation orchestration
helpers/
|-- CoreHelperModels.h           # Shared data structures
|-- exporters/                   # JSON/CSV/TXT output serialization
|-- finders/                     # Installed application discovery
|-- resolvers/                   # DOS path resolution helpers
|-- scanners/                    # Recursive directory scanning
`-- verifiers/                   # Installation and file validation
interfaces/
`-- StatementParser.*            # Parsing utilities used by config/script editing
services/
|-- gog/                         # GOG Galaxy database operations
|-- sql/                         # Lower-level SQLite service wrappers
|-- system/                      # Backup and filesystem-related services
`-- ScriptEditService.*          # DOSBox config and autoexec file rewriting
tests/                           # CTest-based native test executables
libs/
|-- sqlite/                      # Bundled SQLite source
`-- argparse/                    # Bundled CLI parser
docs/                            # Project documentation
```

## Core Behavior

- `--list-applications`, `--list-games`, `--show-playtasks`, and `--list-backups` are read-oriented operations with optional filtering and export formatting.
- `--backup` and `--restore` operate on the GOG Galaxy database file, usually `galaxy-2.0.db`.
- `--replace-dosbox` is the main mutation flow:
  - validates the selected DOSBox source, either installed or manual path
  - reads the target game and play task metadata from the Galaxy database
  - creates or reuses a custom play task and launch parameters
  - enables custom launch parameters for the product
  - rewrites DOSBox autoexec and config files inside the game installation
- Config rewrite options include fullscreen defaults, resolution defaults, and mapping cleanup.

## Development Notes

- This is a CLI utility, not a GUI or web app.
- The codebase is organized around services and helpers rather than frameworks or dependency injection containers.
- Windows paths, installed programs, and GOG Galaxy storage conventions are first-class concerns.
- Prefer `std::filesystem`, standard library algorithms, and existing service/helper abstractions over introducing new patterns.
- Keep output and behavior compatible with current CLI flags unless a change explicitly updates the interface.

## Hard Constraints

- Never describe this repo as Electron, React, TypeScript, or AI-editor based software.
- Never make unrequested breaking changes to CLI flags, option names, defaults, or output formats.
- Never modify GOG Galaxy database logic casually; preserve existing validation and backup/restore workflows.
- Never assume non-Windows behavior is the main target unless the code already supports it.
- Never remove tests to force success.
- Never commit unless explicitly requested.
