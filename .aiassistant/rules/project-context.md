---
apply: always
---

# Amagon Project Context

Amagon is an offline, AI-powered visual HTML editor built with Electron, React, and TypeScript.

## Tech Stack

- Electron, Vite, React, TypeScript, Zustand, Monaco Editor, Bootstrap 5

## Project Structure

```text
src/
├── main/                 # Electron main process
│   └── aiService.ts     # AI provider adapters + secure key storage
├── preload/             # Electron preload scripts
├── preview/             # Canvas runtime (iframe content)
├── renderer/            # React app
│   ├── components/      # AiAssistant, Canvas, CodeEditor, Inspector, ThemeEditor
│   ├── hooks/           # Custom React hooks
│   ├── registry/        # Block definitions (50+ types)
│   ├── store/           # Zustand stores (EditorStore, ProjectStore, AiStore)
│   ├── styles/          # CSS styles
│   └── utils/           # Utility functions
└── types/               # TypeScript types
```

## Hard Constraints

- Never use `as any`, `@ts-ignore`, `@ts-expect-error` for type suppression.
- Never use empty catch blocks.
- Never delete failing tests to force pass.
- Never commit unless explicitly requested.
- Never leave code in broken state after failures.
