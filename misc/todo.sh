#!/usr/bin/env bash
cd "$(dirname "$0")/.."
grep -rn "TODO" code/
