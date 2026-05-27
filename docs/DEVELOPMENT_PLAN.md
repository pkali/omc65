# omc65 Development Plan

This plan turns the current TODO items into incremental milestones with objective acceptance criteria.

## Scope and Goals

1. Keep Mac/65 compatibility where practical.
2. Improve Linux behavior and path handling.
3. Build a repeatable regression test suite from bundled assembly samples and minimal repro snippets.
4. Reduce parser ambiguity and improve diagnostics for invalid addressing syntax.

## Milestone 0: Baseline and Safety Nets (1-2 days)

Deliverables:

1. Add a baseline test runner script that assembles all files in `test/` and reports pass/fail.
2. Add smoke tests for known bug patterns:
	- `LSR A;comment`
	- zero page `,Y` encoding (e.g. `LDX $10,Y`)
	- indirect syntax variants `($22),Y`, `($22,X)`, `($22),X`
3. Document expected outcomes for each sample in a simple golden file.

Acceptance criteria:

1. One command runs all tests and exits non-zero on failure.
2. Existing working samples (`IO.S65`, `MK720.S65`, `MNOZ88.S65`, `SHORTSYS.S65`) are green.

## Milestone 1: CLI and Platform Correctness (1-2 days)

Deliverables:

1. Fix option parsing so absolute Linux paths are accepted for source and destination arguments.
2. Fix `-N` option fallthrough (should not implicitly enable `-M`).
3. Preserve and print the real destination path in listing output.
4. Remove forced uppercasing of include filenames on case-sensitive file systems.
5. Correct build command examples in README.

Acceptance criteria:

1. Absolute path source files assemble correctly on Linux.
2. `-N` only toggles error-number mode; buffer size unchanged unless `-M` is passed.
3. Listing header shows the actual output path.
4. `.include` works with lowercase/mixed-case filenames on Linux.

## Milestone 2: Addressing Parser Hardening (2-4 days)

Deliverables:

1. Tighten address mode parsing for parenthesized indexed forms.
2. Distinguish invalid forms (`($22),X`) from evaluable expressions and report `bad addressing mode`.
3. Ensure error paths do not emit misleading opcodes for invalid modes.
4. Add dedicated tests for each legal and illegal addressing form.

Acceptance criteria:

1. Legal forms produce correct opcodes and byte lengths.
2. Illegal forms fail with deterministic diagnostics.
3. No generated code bytes are emitted for syntactically invalid operands.

## Milestone 3: Deterministic Regression Harness (2-3 days)

Deliverables:

1. Introduce snapshot tests for `.xex` output bytes of representative inputs.
2. Add listing/error-output verification for key diagnostics.
3. Add CI workflow to compile and run tests on Linux.

Acceptance criteria:

1. CI runs on every push/PR and blocks regressions.
2. Byte-level output changes are explicit and reviewed.

## Milestone 4: Refactoring and Readability (ongoing)

Deliverables:

1. Start translating high-impact Polish identifiers/comments to English incrementally.
2. Split monolithic file into modules (`parser`, `expressions`, `labels`, `macros`, `io`).
3. Add brief architecture notes for future contributors.

Acceptance criteria:

1. No behavior changes in refactor-only PRs (tests must stay green).
2. New contributors can trace assemble flow from docs in under 15 minutes.

## Priority Backlog (Ordered)

1. Absolute-path handling in CLI argument parsing.
2. `-N` fallthrough and destination-path reporting bug.
3. Include filename case handling.
4. Invalid parenthesized/indexed addressing diagnostics.
5. Test harness and CI.
6. Identifier translation and modularization.

## Suggested Workflow

1. Keep PRs small and milestone-focused.
2. Require a failing test before each bug fix when reproducible.
3. For parser changes, include at least one positive and one negative test per syntax branch.
4. Defer large refactors until Milestones 0-3 are green.

