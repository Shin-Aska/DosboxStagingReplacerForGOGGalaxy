---
name: deploy-to-publish
description: Bumps the git tag using semantic-version rules before publishing
---

Use this skill whenever you need to publish a new build and the version tag must be incremented (format: `MAJOR.MINOR.PATCH`, e.g. `1.0.0`).

### Inputs
- **bumpType**: `patch` (default), `minor`, or `major`.

### Steps
1. Fetch the latest tags so you have the newest state:
   - `git fetch --tags`
2. Get the most recent tag (falls back to `0.0.0` if none exist):
   - `git describe --tags --abbrev=0`
3. Parse the tag into `MAJOR.MINOR.PATCH` numbers.
4. Update the `VERSION` in the `project()` command in `CMakeLists.txt` to match the new `MAJOR.MINOR.PATCH` value.
5. Update the `APP_VERSION` define in `main.cpp` to match the new `MAJOR.MINOR.PATCH` value.
6. If `CMakeLists.txt` or `main.cpp` was modified, commit and push the changes:
   - `git add CMakeLists.txt main.cpp`
   - `git commit -m "chore: bump version to <newVersion>"`
   - `git push origin`
7. Depending on `bumpType`, update the numbers:
   - `patch` (or unspecified): increment PATCH only, e.g. `1.0.10 -> 1.0.11`.
   - `minor`: increment MINOR, reset PATCH to `0`, e.g. `1.2.5 -> 1.3.0`.
   - `major`: increment MAJOR, reset MINOR and PATCH to `0`, e.g. `1.2.5 -> 2.0.0`.
8. Create the new tag locally (format as `MAJOR.MINOR.PATCH`):
   - `git tag <newVersion>`
9. Push the tag:
   - `git push origin <newVersion>`
10. The GitHub Release will be created as a **draft**. Once the CI finishes, you'll need to manually review and publish it from the GitHub Releases page.
11. Announce the new version or run any follow-up publish steps that depend on the bumped tag.
