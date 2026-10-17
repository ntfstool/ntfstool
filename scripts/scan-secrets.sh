#!/bin/bash
# Copyright © 2019-2026 NTFSTool.
# SPDX-License-Identifier: MIT
# Scan the source tree for things that must never be published:
#   - signing material (provisioning profiles, certificates, private keys, keychains)
#   - identifiers of the official signed builds (Team ID, store bundle id, App Store Connect files)
#   - endpoints of the official builds' data services (diagnostics upload, behaviour traces)
# Run it before every commit:  make scan
set -uo pipefail
cd "$(dirname "$0")/.."

# The [x] brackets keep each pattern from matching this file itself.
PATTERN='JHJET4[2]HR6|com[.]ntfstool[.]aile|appstoreconnec[t]|kc[.]pass|asc[.]env|BEGIN (RSA |EC |OPENSSH )?PRIVAT[E] KEY|trac[e][.]ntfstool[.]com|diag_uploa[d]|utm_source=appstor[e]|moves[a]y'

fail=0
hits=$(grep -rnIE "$PATTERN" . --exclude-dir=.build --exclude-dir='.build-*' --exclude-dir=build --exclude-dir=.git || true)
[ -n "$hits" ] && { echo "$hits"; fail=1; }

files=$(find . \( -path ./.build -o -path './.build-*' -o -path ./build -o -path ./.git \) -prune -o \
  \( -name '*.provisionprofile' -o -name '*.mobileprovision' -o -name '*.p12' -o -name '*.cer' \
     -o -name '*.keychain*' -o -name '*.p8' -o -name '*.pem' \) -print)
[ -n "$files" ] && { echo "$files"; fail=1; }

if [ $fail = 0 ]; then echo "✅ scan clean"; else echo "❌ sensitive content found" >&2; fi
exit $fail
