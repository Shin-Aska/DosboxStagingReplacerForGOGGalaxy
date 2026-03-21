---
description: multi-agent planning via claude opus 4.6 with ultra budget executors
auto_execution_mode: 0
---

## Overview
Use this workflow when you want **Claude Opus 4.6** to design the overall plan, then hand off execution to **ultra-budget models**.

Because the executor models are more limited, the plan must be **extremely explicit, broken into small verifiable tasks, and anchored to file paths**.

The key requirement: **Claude must output a Markdown checkbox plan** (not prose) that you can execute phase-by-phase with other agents.

This workflow is **plan-only**: it must end after writing the plan to `plan.md`. Execution happens in a separate workflow.

## Steps
1. **Collect current context**
   - Summarize the problem, repo state, constraints, and desired deliverables.
   - Include:
     - Target user-visible behavior changes
     - Known entry points / files if you have them
     - Testing requirements (unit/integration/manual)
     - Any non-goals
2. **Open a planning session with Claude Opus 4.6**
   - Feed the context above plus the agent roster below.
   - Use the **copy/paste prompt template** below.
   - Do not accept Claude’s answer unless it matches the required checkbox format.
3. **Review and finalize the plan**
   - Ensure tasks are small enough for ultra-budget executors.
   - Ensure every checkbox is verifiable and references concrete files/identifiers.
   - Lock the plan before delegating work.
4. **Write the plan to `plan.md` (repo root)**
   - Save Claude’s output verbatim into `plan.md`.
   - Ensure the plan is Markdown with checkboxes.
5. **Stop**
   - Do not execute any phases in this workflow.
   - Next: run the `/execute-plan` workflow to execute the next phase (or a user-specified phase).

## Claude Opus prompt template (ultra-budget, strictest)
Copy/paste the following into Claude Opus 4.6. Replace the bracketed sections.

```text
You are the PLANNER. Produce an EXECUTION PLAN that will be carried out by multiple specialized agents (ultra-budget models).

Hard requirements (must follow exactly):
1) Output MUST be Markdown.
2) Output MUST be a checkbox plan, with tasks using literal "- [ ]" checkboxes (no bullets without checkboxes).
3) Plan MUST be split into numbered Phases (Phase 1, Phase 2, ...).
4) Every Phase MUST specify exactly one assigned agent from the roster (not Opus), except a final "QA / Integration" phase which can be Claude Opus 4.6.
   4a) Distribute execution across multiple non-Claude agents (use at least 2 different executor agents across the phases).
   4b) Every phase must only be assign to one agent. You cannot have multiple agents working on the same phase (that also means you cannot have subtasks assigned to different agents)
5) Ultra-budget constraint: assume executors will follow instructions literally.
   - Break work into small steps; each checkbox should be 10-30 minutes of focused work.
   - Each task must touch 1-3 files max.
   - Every task must include file paths and concrete identifiers (function names, component names, store keys, message types, CSS selectors).
   - If something is unknown, add an explicit "investigate" task with the exact search terms and files/dirs to inspect.
6) Every Phase MUST include (in this exact order):
   - **Agent:** <one from roster>
   - **Goal:** (1-2 sentences)
   - **Context for Agent:** what to read first + invariants/constraints
   - **Tasks:** checklist items using the exact format: "- [ ] **N.M** <imperative verb> ... (Files: ... )"
   - **Acceptance Criteria:** checklist
   - **Validation:** explicit commands (if applicable) + manual verification steps
7) Include a "Risks / Edge Cases" section as a checklist (so we can track mitigations).
8) Include a Dependency Graph section and a Recommended Execution Order table.
9) Include a Handoff Prompts section: for each Phase, provide a one-line message I can paste into the assigned agent, e.g. "Run /execute-plan Phase 3".
10) No vague tasks like "improve" or "polish". Replace them with concrete UI/behavior changes.

Project context:
[PASTE CONTEXT HERE]

Agent roster (choose from these):
- GPT-5.1-Codex
- SWE1.5
- Grok-Code-Fast-1

Now produce the plan.
```

## Agent roster & expertise
- **Claude Opus 4.6 (Planner & QA only)**: High-level strategy, risk analysis, coordination, final review, integration checklist.
- **GPT-5.1-Codex**: Coding-heavy tasks, refactors with explicit specs, tests, straightforward algorithmic work.
- **SWE1.5**: Smaller end-to-end implementations, bug fixes, incremental UI wiring, safe refactors.
- **Grok-Code-Fast-1**: Fast debugging passes, quick instrumentation, narrow changes with tight acceptance criteria.

## Handoff tips
- Keep a running checklist of completed subtasks and outstanding ones.
- When briefing ultra-budget models, paste:
  - The Phase section verbatim
  - Any relevant file excerpts
  - The acceptance criteria they must satisfy
- If an executor deviates from plan format or misses criteria, stop and re-brief with the specific missed checkboxes.

## Completion criteria
- Every plan step has an assigned agent, recorded outcome, and validation notes.
- Code/tests/docs are merged or staged per plan.
- Outstanding risks are documented for the next planning cycle.
