---
description: multi-agent planning via claude opus 4.6 with gpt-5/codex primary executors, gemini, and optional kimi k2.5
auto_execution_mode: 0
---

## Overview
Use this workflow when you want Claude Opus 4.6 to design the overall plan, then hand off each task to the agent best suited for it.

The primary executor pool is **GPT-5 / Codex** and **Gemini**. Claude Sonnet 4.6 is reserved for complex QA and architectural work only — prefer GPT or Gemini for implementation. **Kimi K2.5** is available on-demand via the `Enable Kimi` setting flag for frontend/visual coding tasks.

The key requirement: **Claude must output a Markdown checkbox plan** (not prose) that you can execute phase-by-phase with other agents.

This workflow is **plan-only**: it must end after writing the plan to `plan.md`. Execution happens in a separate workflow.

## Settings / State Flags

Include a `Setting:` line at the top of your task prompt to activate optional behaviours. Multiple flags can be combined:

```
Setting: Enable Kimi, Almost Limit[OpenAI, Claude]
```

### `Enable Kimi`
Unlocks Kimi K2.5 (via OpenCode Zen, prepaid credits). Use for frontend/visual coding, design-to-code (screenshot/Figma → React/Vue/HTML), and visual debugging. Without this flag, Kimi K2.5 must not be assigned to any phase.

### `Almost Limit[OpenAI]`
Signals that OpenAI API usage is approaching ~80% of quota.
- Prefer `GPT-5.4-mini` over `GPT-5.4`, and `GPT-5.2` over `GPT-5.3-Codex`.
- If a phase would normally use a large OpenAI model, downgrade or reassign to a Gemini or Claude model.

### `Almost Limit[Claude]`
Signals that Claude API usage is approaching ~80% of quota.
- If `Almost Limit[OpenAI]` is **not** active: shift as much work as possible to OpenAI models (implementation AND QA).
- QA reassignment under this flag:
  - Simple QA → GPT-5.2
  - Standard QA → GPT-5.3-Codex
  - Complex QA → GPT-5.4
- If **both** limits are active: use mini/lite models and Gemini — Claude Haiku 4.5 for Claude-side, GPT-5.4-mini for OpenAI-side, Gemini 3.1 Flash Lite / Gemini 2.5 Flash Lite for Gemini-side.

---

## Steps
1. **Collect current context**
   - Summarize the problem, repo state, constraints, and desired deliverables.
   - Note any active Settings flags, deadlines, or testing requirements.
2. **Open a planning session with Claude Opus 4.6**
   - Feed the context above plus the agent roster below.
   - Use the **copy/paste prompt template** below.
   - Do not accept Claude's answer unless it matches the required checkbox format.
3. **Review and finalize the plan**
   - Sanity-check scope, ordering, and dependencies.
   - If adjustments are needed, iterate with Claude until the plan is actionable.
   - Lock the plan before delegating work.
4. **Write the plan to `plan.md` (repo root)**
   - Save Claude's output verbatim into `plan.md`.
   - Ensure the plan is Markdown with checkboxes.
5. **Stop**
   - Do not execute any phases in this workflow.
   - Next: run the `/execute-plan` workflow to execute the next phase (or a user-specified phase).

## Claude Opus prompt template (forces checkbox plan)
Copy/paste the following into Claude Opus 4.6. Replace the bracketed sections.

```text
You are the PLANNER. Produce an EXECUTION PLAN that will be carried out by multiple specialized agents.

Hard requirements (must follow exactly):
1) Output MUST be Markdown.
2) Output MUST be a checkbox plan, with tasks using literal "- [ ]" checkboxes (no bullets without checkboxes).
3) Plan MUST be split into numbered Phases (Phase 1, Phase 2, ...).
4) Every Phase MUST specify exactly one assigned agent from the roster.
4a) Distribute execution across multiple agents (use at least 2 different executor agents across the phases).
4b) Apply the active Settings flags before assigning any agent:
    - Almost Limit[OpenAI]: prefer GPT-5.4-mini and GPT-5.2; downgrade or reassign large OpenAI models.
    - Almost Limit[Claude]: shift implementation and QA to OpenAI or Gemini models if OpenAI is not also at limit.
    - If both limits active: mini/lite models and Gemini only on both sides.
4c) Executor priority for implementation phases (follow this order unless limit flags override):
    1st: GPT-5 / Codex models (primary executors)
    2nd: Gemini models (UI work, component volume, and overflow)
    3rd: Kimi K2.5 (only if "Enable Kimi" is active — frontend/visual coding only)
    4th: Claude Sonnet 4.6 (only for tasks requiring deep architectural reasoning that GPT/Gemini cannot handle)
4d) Claude Sonnet 4.6 usage policy:
    - Do NOT assign Sonnet to general implementation, feature development, or routine coding.
    - Sonnet is reserved for: complex architectural refactors, nuanced multi-system integration, and Standard/Complex QA.
    - If a task can be handled by GPT-5.4, GPT-5.3-Codex, or Gemini 3.1 Pro High, assign it there instead of Sonnet.
4e) QA escalation path (default — overridden by limit flags above):
    - Simple QA (running test suites, build verification, manual smoke testing): Claude Haiku 4.5
    - Standard QA (integration testing, regression checks, code review): Claude Sonnet 4.6
    - Complex QA (architectural validation, security review, cross-cutting concerns): Claude Opus 4.6
5) Every Phase MUST include:
   - Goal (1-2 sentences)
   - Context for Agent (what to read / what invariants to respect)
   - Tasks section with checklist items using the format: "- [ ] **N.M** ..." and each item being concrete and verifiable
   - Acceptance Criteria section as a checklist
   - Validation steps (tests to run, manual verification)
6) Include a Dependency Graph section and a Recommended Execution Order table.
7) Include a Handoff Prompts section: for each Phase, provide a one-line message I can paste into the assigned agent, e.g. "Run /execute-plan Phase 3". Each handoff prompt must also echo the active Settings flags.
8) Keep it actionable: no vague tasks like "improve" or "polish". Prefer file paths, components, and APIs.
9) Every phase must only be assigned to one agent. You cannot have multiple agents working on the same phase.

Active settings: [PASTE ACTIVE FLAGS HERE, e.g. "Enable Kimi, Almost Limit[OpenAI]" — or "None"]

Project context:
[PASTE CONTEXT HERE]

Agent roster — always available (choose from these unless limit flags apply):

Claude Pro (use sparingly — prefer GPT/Gemini for implementation):
- Claude Opus 4.6 (Planner & Complex QA — architectural validation, security review, cross-cutting concerns)
- Claude Sonnet 4.6 (Standard QA, complex architectural refactors ONLY — do not use for general implementation)
- Claude Haiku 4.5 (Simple QA, quick tasks, boilerplate, simple fixes, documentation)

GPT-5 / Codex — OpenAI (primary executor pool):
- GPT-5.4 (High-complexity reasoning, cross-cutting analysis, large refactors)
- GPT-5.4-mini (Balanced general-purpose tasks, mid-complexity implementation)
- GPT-5.3-Codex (Coding specialist — complex multi-file implementation, agentic coding)
- GPT-5.2 (General implementation, feature development)

Gemini (always available — append "High" or "Low" effort when assigning):
- Gemini 3.1 Pro High/Low (Deep reasoning, large-scale refactoring, thorough analysis, UI generation — use High for complex work, Low for volume/styling)
- Gemini 3 Flash (Fast general tasks, boilerplate, simple fixes)
- Gemini 3.1 Flash Lite (Lightweight quick tasks, documentation, simple bug fixes)
- Gemini 2.5 Pro (Proven stable model — complex implementation, thorough analysis)
- Gemini 2.5 Flash (Fast general-purpose tasks, moderate implementation)
- Gemini 2.5 Flash Lite (Cheapest/fastest — trivial tasks, boilerplate, formatting)

[IF "Enable Kimi" was specified, also available:]
- Kimi K2.5 via OpenCode Zen (Frontend/visual coding, design-to-code, screenshot-to-component, visual debugging)

Now produce the plan.
```

## Agent roster & expertise

### Claude Pro (use sparingly — reserve quota for planning & QA)
- **Claude Opus 4.6 (Planner & Complex QA)**: High-level strategy, risk analysis, coordination, architectural validation, security review, and final review for complex concerns. This is the planner — do not waste quota on implementation.
- **Claude Sonnet 4.6 (Standard QA & architectural work ONLY)**: Reserved for complex architectural refactors, nuanced multi-system integration, integration testing, regression checks, and code review. Do NOT assign to general implementation — use GPT-5.4 or GPT-5.3-Codex instead.
- **Claude Haiku 4.5 (Simple QA)**: Running test suites, build verification, manual smoke testing, quick tasks, boilerplate, simple bug fixes, and documentation.

### GPT-5 / Codex — OpenAI (primary executor pool)
- **GPT-5.4**: Highest-capability OpenAI model. Best for high-complexity reasoning, cross-cutting analysis, and large-scale refactors. Also serves as Complex QA substitute when Claude is at limit.
- **GPT-5.4-mini**: Balanced model for general-purpose tasks and mid-complexity implementation. Preferred when `Almost Limit[OpenAI]` is active instead of GPT-5.4.
- **GPT-5.3-Codex**: Coding specialist. Best for complex multi-file implementation and agentic coding tasks. Primary choice for implementation phases.
- **GPT-5.2**: Solid general-purpose model for feature development and standard implementation work. Lightweight alternative when conserving OpenAI quota.

### Gemini (always available)
Models are available via Gemini CLI or Antigravity (separate quotas). Append effort level (High/Low) when assigning Gemini 3.1 Pro to indicate reasoning depth vs. speed.
- **Gemini 3.1 Pro High**: Deep reasoning, large-scale refactoring, thorough code analysis, and complex implementation. Use for tasks needing careful multi-step reasoning.
- **Gemini 3.1 Pro Low**: UI/UX component generation, widget libraries, styling/theming, and well-scoped implementation. Use for volume work where speed matters more than depth.
- **Gemini 3 Flash**: Fast general tasks, boilerplate generation, simple fixes, and documentation.
- **Gemini 3.1 Flash Lite**: Lightweight quick tasks, documentation, trivial bug fixes. Cheapest 3.x option.
- **Gemini 2.5 Pro**: Proven stable model for complex implementation and thorough analysis. Fallback when 3.1 Pro quota is constrained.
- **Gemini 2.5 Flash**: Fast general-purpose tasks and moderate implementation. Good balance of speed and capability.
- **Gemini 2.5 Flash Lite**: Cheapest/fastest option for trivial tasks, boilerplate, and formatting.

### Kimi K2.5 via OpenCode Zen (opt-in via `Enable Kimi` setting)
- **Kimi K2.5**: Frontend/visual coding specialist. Design-to-code (screenshot/Figma → React/Vue/HTML), visual debugging (renders output and self-corrects), and UX polish. Accessed via OpenCode Zen prepaid credits — use when visual coding tasks would benefit from native multimodal understanding. When handing off design work, attach screenshots or design references directly.

---

## Handoff tips
- Keep a running checklist of completed subtasks and outstanding ones.
- Include links to PRs, commits, or files when briefing agents.
- Always echo active Settings flags at the top of each handoff prompt.
- Note blocking issues immediately so Claude can replan if necessary.
- When handing off to Kimi K2.5, attach screenshots or design references directly — it excels at visual-to-code when given visual input.

## Completion criteria
- Every plan step has an assigned agent, recorded outcome, and validation notes.
- Code/tests/docs are merged or staged per plan.
- Outstanding risks are documented for the next planning cycle.

---

## Final Step: Update Project Documentation

After all phases are complete, update the living documentation to reflect what changed.

**Files to update:**
- `GUIDELINES.md` — project structure, IPC channels, Zustand stores, data models, key files
- `.aiassistant/rules/project-context.md` — project structure, key new systems

**What to check and update in each file:**

1. **Project structure tree** — add/remove/rename any `src/` directories or key files introduced in this plan
2. **Zustand stores table** — add any new stores; update responsibilities if existing stores changed significantly
3. **IPC channels table** — add any new namespaces or channels; update existing entries if signatures changed
4. **Data models** — add new interfaces or extend existing ones documented there (e.g. new fields on project file schema)
5. **Key systems sections** — if a new major subsystem was introduced, add a dedicated section explaining its architecture (similar to the Publish-to-Web or Tutorial sections)
6. **Key Files to Read First** — add any new entry-point files that future AI assistants or developers should read
7. **Last updated date** — bump to today's date in `GUIDELINES.md`

**Agent assignment:** Assign this step to **Claude Haiku 4.5** unless the changes are architectural (new subsystem, significant restructuring) — in that case use **Claude Sonnet 4.6**.

**Handoff prompt template:**
```
[Active settings: PASTE FLAGS]
Review the changes made in this plan and update GUIDELINES.md and .aiassistant/rules/project-context.md to reflect them. Check: project structure tree, Zustand stores, IPC channels, data models, new system sections, key files list, and last-updated date. Only update what actually changed.
```