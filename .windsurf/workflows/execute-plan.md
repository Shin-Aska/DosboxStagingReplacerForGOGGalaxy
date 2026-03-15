---
description: execute next (or specified) phase from plan.md
auto_execution_mode: 3
---

## Overview
Use this workflow to execute **exactly one phase** from `plan.md`.

- If the user specifies a phase (e.g. "Phase 3"), execute that phase.
- Otherwise, execute the **next incomplete** phase in sequential order.

This workflow assumes the plan is sequential and uses Markdown checkboxes.

## Inputs
- **Optional:** a phase identifier from the user (examples: `Phase 3`, `3`).

## Steps
1. **Load the plan**
   - Open `plan.md` at the repo root.
   - If `plan.md` does not exist, stop and ask the user to run `/multi-agent-plan` first (or provide the plan content).

2. **Sanity-check the plan format**
   - Ensure `plan.md` contains at least one clearly labeled phase header (for example: `Phase 1`, `Phase 2`, ...).
   - Ensure at least one checkbox exists in the plan using Markdown checkbox syntax (`- [ ]` or `- [x]`).
   - If the user requested a specific phase, ensure that phase exists.
   - If the plan is missing phases, has no checkboxes, or the requested phase can’t be found, stop and ask the user to:
     - Fix `plan.md`, OR
     - Re-run `/multi-agent-plan` and re-save the output into `plan.md`.

3. **Select the phase to execute**
   - If the user provided a phase number, select that phase.
   - Else, pick the **first phase** (Phase 1, Phase 2, ...) that is incomplete.

   A phase is considered **incomplete** if any checkbox inside that phase is unchecked (`- [ ]`).
   - If all checkboxes in all phases are checked, stop and report that the plan is fully complete.

4. **Read the selected phase carefully**
   - Identify:
     - The assigned agent (if specified)
     - Context / invariants
     - Tasks checklist
     - Acceptance Criteria checklist
     - Validation steps

5. **Execute the phase**
   - Implement the tasks in code.
   - As each task is completed, update `plan.md` by checking off the corresponding item (`- [x]`).
   - Do not execute tasks from other phases.
   - If you discover missing information or blockers:
     - Add a new unchecked checkbox item under the selected phase describing the required investigation/work.
     - Stop and report the blocker.

6. **Validate**
   - Follow the phase’s Validation steps.
   - If validation fails, leave the relevant items unchecked and record a brief note under that phase (as a checkbox item) describing what failed.

7. **Close out (for this phase only)**
   - Ensure:
     - All completed tasks are checked off
     - Acceptance Criteria items are checked off only if truly satisfied
   - Summarize:
     - What changed (key files)
     - What’s left (any remaining unchecked items)
     - How to run validation again
