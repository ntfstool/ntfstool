# Copyright © 2019-2026 NTFSTool.
# SPDX-License-Identifier: MIT
# NTFSTool — one-command build targets. See README.md.
#
#   make            same as `make app`
#   make app        build build/NTFSTool.app (ad-hoc signed unless SIGN_IDENTITY is set; UNIVERSAL=1 for arm64 + x86_64)
#   make run        build and open the app
#   make demo       run the UI with in-memory demo disks (no driver, no Apple Developer account needed)
#   make test       run all unit tests (driver image tests are skipped unless mkntfs/ntfsfix are found)
#   make debug      fast debug build of the Swift package (compile check only)
#   make scan       fail if signing material or private identifiers slipped into the tree
#   make clean      remove build output
.PHONY: all app run demo test debug scan clean

all: app

app:
	@scripts/build-app.sh

run: app
	open build/NTFSTool.app

demo:
	swift build --product NTFSTool
	NTFSTOOL_DEMO=1 "$$(swift build --show-bin-path)/NTFSTool"

test:
	swift test

debug:
	swift build --product NTFSTool

scan:
	@scripts/scan-secrets.sh

clean:
	rm -rf .build .build-* build
