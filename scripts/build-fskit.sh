#!/bin/bash
# Copyright © 2019-2026 NTFSTool.
# SPDX-License-Identifier: MIT
# Build the FSKit file system module (NTFSToolFS.appex) and embed it in the app.
# Called by scripts/build-app.sh:
#
#   scripts/build-fskit.sh "<path to .app>" [--universal]
#
# What it does:
#   1. compiles ntfstool_bridge.c (C) for each architecture and links it with the
#      prebuilt libntfstool-driver in Vendor/NTFSToolNTFS.xcframework,
#   2. compiles the Swift driver (Sources/NTFSToolDriver), the module
#      (FSKitModule/NTFSToolFS) and the shared event log into one executable,
#      using _NSExtensionMain as the entry point (like Apple's FSKit modules),
#   3. writes the extension's Info.plist (FSKit attributes, see below).
#
# Output: <App>/Contents/Extensions/NTFSToolFS.appex (unsigned; build-app.sh signs
# everything inside-out afterwards).
#
# Environment:
#   FSKIT_ID         bundle identifier of the module   (default com.ntfstool.opensource.fskit)
#   FSKIT_SHORT      FSKit short name, i.e. `mount -F -t <name>` and the type in the
#                    mount table (default ntfstooloss; must not be "ntfs", which is
#                    Apple's built-in read-only module)
#   FSKIT_DISPLAY    human readable name                (default NTFSTool)
#   FSKIT_AUTOMOUNT  1 = declare FSMediaTypes so the system mounts NTFS disks with this
#                    module automatically when plugged in (default 1). FSMediaTypes alone
#                    is not enough: the check/format option syntax and FSSubType must be
#                    declared too, otherwise the system ignores the module.
#   FSKIT_LOG_GROUP  App Group for the module's JSON event log (empty = no event log)
#   FSKIT_MIN        minimum macOS version               (default 15.4, FSKit's minimum)
#   FSKIT_VERSION / FSKIT_BUILD  version strings; must match the host app
#
# Requires an Xcode SDK that contains FSKit (Xcode 16.3 or later).
set -euo pipefail
cd "$(dirname "$0")/.."
APP="$1"; shift || true
ARCHS=(arm64)
[ "${1:-}" = "--universal" ] && ARCHS=(arm64 x86_64)
MIN="${FSKIT_MIN:-15.4}"
FSKIT_ID="${FSKIT_ID:-com.ntfstool.opensource.fskit}"
FSKIT_SHORT="${FSKIT_SHORT:-ntfstooloss}"
FSKIT_AUTOMOUNT="${FSKIT_AUTOMOUNT:-1}"
FSKIT_DISPLAY="${FSKIT_DISPLAY:-NTFSTool}"
FSKIT_LOG_GROUP="${FSKIT_LOG_GROUP:-}"
OUT=.build/fskit
mkdir -p "$OUT"
SDK=$(xcrun --show-sdk-path)
CSRC=(Sources/NTFSToolBridge/ntfstool_bridge.c)
# Prebuilt libntfstool-driver (static, arm64 + x86_64) and its headers.
NTFSLIB=Vendor/NTFSToolNTFS.xcframework/macos-arm64_x86_64
# NTFSToolEventLog.swift is shared with the app (Foundation only) for the optional event log.
SWSRC=(Sources/NTFSToolDriver/*.swift FSKitModule/NTFSToolFS/*.swift Sources/NTFSToolCore/NTFSToolEventLog.swift)
BINS=()
for A in "${ARCHS[@]}"; do
  mkdir -p "$OUT/$A"
  OBJS=()
  for c in "${CSRC[@]}"; do
    o="$OUT/$A/$(basename "${c%.c}").o"
    # Rebuild an object only when its source (or the bridge header) is newer.
    if [ ! -f "$o" ] || [ "$c" -nt "$o" ] || [ Sources/NTFSToolBridge/include/ntfstool_bridge.h -nt "$o" ]; then
      xcrun clang -c -O2 -w -arch "$A" -mmacosx-version-min=$MIN -DHAVE_CONFIG_H \
        -ISources/NTFSToolBridge/include -I$NTFSLIB/Headers "$c" -o "$o"
    fi
    OBJS+=("$o")
  done
  # The Swift sources import the C API as module NTFSToolBridge (include/module.modulemap).
  xcrun swiftc -O -parse-as-library -module-name NTFSToolFS -target "$A-apple-macos$MIN" -sdk "$SDK" \
    -I Sources/NTFSToolBridge/include "${SWSRC[@]}" "${OBJS[@]}" "$NTFSLIB/libntfstool-driver.a" -framework CoreFoundation \
    -Xlinker -e -Xlinker _NSExtensionMain -o "$OUT/$A/NTFSToolFS"
  BINS+=("$OUT/$A/NTFSToolFS")
done
X="$APP/Contents/Extensions/NTFSToolFS.appex"
rm -rf "$X"
mkdir -p "$X/Contents/MacOS"
# Single architecture: copy as is (a one-architecture fat file can be damaged by later vtool rewrites).
if [ ${#BINS[@]} -eq 1 ]; then cp "${BINS[0]}" "$X/Contents/MacOS/NTFSToolFS"; else lipo -create "${BINS[@]}" -output "$X/Contents/MacOS/NTFSToolFS"; fi
# The extension's version must match the host app (App Store validation); build-app.sh passes it.
VERSION="${FSKIT_VERSION:-$(cat version)}"
BUILD="${FSKIT_BUILD:-$VERSION}"
python3 - "$X/Contents/Info.plist" "$VERSION" "$FSKIT_ID" "$FSKIT_SHORT" "$FSKIT_AUTOMOUNT" "$MIN" "$FSKIT_DISPLAY" "$BUILD" "$FSKIT_LOG_GROUP" <<'PY'
import plistlib, sys
out, version, bid, short, automount, minos, display, build, loggroup = sys.argv[1:]
d = {
  "CFBundleExecutable": "NTFSToolFS", "CFBundleIdentifier": bid, "CFBundleName": "NTFSToolFS",
  "CFBundleDisplayName": display, "CFBundlePackageType": "XPC!", "CFBundleShortVersionString": version,
  "CFBundleVersion": build, "CFBundleInfoDictionaryVersion": "6.0", "LSMinimumSystemVersion": minos,
  "NSHumanReadableCopyright": f"{display} driver (FSKit).",
  "EXAppExtensionAttributes": {
    "EXExtensionPointIdentifier": "com.apple.fskit.fsmodule",
    # Class instantiated by FSKit (@objc name in NTFSToolFS.swift).
    "EXExtensionPrincipalClass": "NTFSToolFileSystem",
    "FSShortName": short,
    "FSSupportsBlockResources": True,
    "FSSupportsServerURLs": False,
    "FSPersonalities": {"NTFSTool": {"FSName": f"NTFS ({display})"}},
    # Required: `mount -F` uses it to decide that the module supports mounting
    # (without it mount fails with "does not support operation mount").
    "FSActivateOptionSyntax": {"shortOptions": "ro:"},
  },
}
if loggroup:
  d["NTFSToolLogGroup"] = loggroup
if automount == "1":
  a = d["EXAppExtensionAttributes"]
  m = lambda hint: {"FSMediaProperties": {"Content Hint": hint, "Leaf": True}, "FSProbeOrder": 100}
  # Probe order 100 runs before Apple's built-in read-only NTFS (1000);
  # partitionless whole disks use 200.
  a["FSMediaTypes"] = {"Windows_NTFS": m("Windows_NTFS"), "EBD0A0A2-B9E5-4433-87C0-68B6B72699C7": m("EBD0A0A2-B9E5-4433-87C0-68B6B72699C7"),
                       "Partitionless": {"FSMediaProperties": {"Leaf": True, "Whole": True}, "FSProbeOrder": 200}}
  # fsck options (-n verify, -q quick, -y repair) and newfs options; see NTFSToolFSKit.CheckMode.
  a["FSCheckOptionSyntax"] = {"shortOptions": "nqy"}
  a["FSFormatOptionSyntax"] = {"shortOptions": "v"}
  a["FSVolumeAlwaysIgnoreOwnership"] = True
  a["FSPersonalities"]["NTFSTool"].update({"FSSubType": 0, "FSfileObjectsAreCaseSensitive": False})
plistlib.dump(d, open(out, "wb"))
PY
echo "Built $X ($(lipo -archs "$X/Contents/MacOS/NTFSToolFS"))"
