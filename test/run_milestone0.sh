#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
TMP_DIR="$(mktemp -d)"
trap 'rm -rf "$TMP_DIR"' EXIT

PASS_COUNT=0
FAIL_COUNT=0

pass() {
  printf '[PASS] %s\n' "$1"
  PASS_COUNT=$((PASS_COUNT + 1))
}

fail() {
  printf '[FAIL] %s\n' "$1"
  FAIL_COUNT=$((FAIL_COUNT + 1))
}

assemble_case() {
  local src_name="$1"
  local out_name="$2"
  local log_name="$3"
  local extra_flag="${4:-}"
  cp "$ROOT_DIR/test/milestone0/cases/$src_name" "$TMP_DIR/$src_name"
  if [[ -n "$extra_flag" ]]; then
    if (cd "$TMP_DIR" && "$ROOT_DIR/omc" "$extra_flag" "$src_name" "$out_name" >"$log_name" 2>&1); then
      return 0
    fi
    return 1
  fi
  if (cd "$TMP_DIR" && "$ROOT_DIR/omc" "$src_name" "$out_name" >"$log_name" 2>&1); then
    return 0
  fi
  return 1
}

expect_hex_case() {
  local name="$1"
  local expected_hex="$2"
  local out_name="${name}.bin"
  local log_name="${name}.log"

  if assemble_case "${name}.s65" "$out_name" "$log_name" -A; then
    local actual_hex
    actual_hex="$(xxd -p "$TMP_DIR/$out_name" | tr -d '\n' | tr '[:lower:]' '[:upper:]')"
    if [[ "$actual_hex" == "$expected_hex" ]]; then
      pass "$name"
    else
      fail "$name (expected bytes $expected_hex, got $actual_hex)"
    fi
  else
    fail "$name (unexpected non-zero exit)"
  fi
}

expect_fail_case() {
  local name="$1"
  local expected_text="$2"
  local out_name="${name}.bin"
  local log_name="${name}.log"

  if assemble_case "${name}.s65" "$out_name" "$log_name" -A; then
    fail "$name (expected failure)"
    return
  fi

  if grep -Fqi "$expected_text" "$TMP_DIR/$log_name"; then
    pass "$name"
  else
    fail "$name (missing expected error text: $expected_text)"
  fi
}

expect_sample_exit() {
  local src_name="$1"
  local should_pass="$2"
  local expected_text="${3:-}"

  cp "$ROOT_DIR/test/$src_name" "$TMP_DIR/$src_name"
  if (cd "$TMP_DIR" && "$ROOT_DIR/omc" "$src_name" "${src_name%.*}.xex" >"$src_name.log" 2>&1); then
    if [[ "$should_pass" == "1" ]]; then
      pass "$src_name"
    else
      fail "$src_name (expected failure)"
    fi
    return
  fi

  if [[ "$should_pass" == "1" ]]; then
    fail "$src_name (unexpected non-zero exit)"
  elif [[ -n "$expected_text" ]] && grep -Fqi "$expected_text" "$TMP_DIR/$src_name.log"; then
    pass "$src_name"
  else
    fail "$src_name (failed but did not include expected text)"
  fi
}

printf '[info] Building assembler...\n'
gcc -std=gnu11 -x c "$ROOT_DIR/OMC_MAIN.C" -o "$ROOT_DIR/omc"

printf '[info] Running smoke tests...\n'
expect_hex_case lsr_comment 4A60
expect_hex_case zp_y B61060
expect_hex_case indirect_valid B122A12260
expect_fail_case indirect_invalid "can't evaluate expression"

printf '[info] Running bundled sample tests...\n'
expect_sample_exit IO.S65 1
expect_sample_exit MK720.S65 1
expect_sample_exit MNOZ88.S65 1
expect_sample_exit SHORTSYS.S65 1
expect_sample_exit test.s65 0 "can't evaluate expression"

printf '\n[summary] passed=%d failed=%d\n' "$PASS_COUNT" "$FAIL_COUNT"
if [[ "$FAIL_COUNT" -ne 0 ]]; then
  exit 1
fi
