#!/bin/bash
# Copyright © 2019-2026 NTFSTool.
# SPDX-License-Identifier: MIT
# Build NTFSTool.app (UI + FSKit driver module) into build/.
#
#   make app                     ad-hoc signed, native arch, no Apple Developer account needed
#   make app UNIVERSAL=1         arm64 + x86_64
#
# Signing with your own Apple Developer account (needed for macOS to load the FSKit driver):
#   TEAM_ID=ABCDE12345 \
#   SIGN_IDENTITY="Apple Development: Your Name (XXXXXXXXXX)" \
#   BUNDLE_ID=com.yourname.ntfs \
#   FSKIT_PROFILE=/path/to/fskit.provisionprofile \
#   APP_PROFILE=/path/to/app.provisionprofile \
#   make app
#
# Environment (all optional):
#   BUNDLE_ID          app bundle id (default com.ntfstool.opensource); the module is "$BUNDLE_ID.fskit"
#   FSKIT_SHORT        FSKit short name shown by `mount` (default ntfstooloss; must differ from Apple's "ntfs"
#                      and from other NTFS modules installed on the same Mac)
#   TEAM_ID            your Team ID; enables application-identifier / App Group entitlements
#   APP_GROUP          shared log container (default "$TEAM_ID.$BUNDLE_ID" when TEAM_ID is set)
#   SIGN_IDENTITY      codesign identity (default "-" = ad-hoc)
#   APP_PROFILE        provisioning profile embedded in the app
#   FSKIT_PROFILE      provisioning profile embedded in the module (must include the FSKit Module capability)
#   UNIVERSAL=1        build arm64 + x86_64
set -euo pipefail
cd "$(dirname "$0")/.."

NAME="NTFSTool"
BUNDLE_ID="${BUNDLE_ID:-com.ntfstool.opensource}"
FSKIT_SHORT="${FSKIT_SHORT:-ntfstooloss}"
TEAM_ID="${TEAM_ID:-}"
APP_GROUP="${APP_GROUP:-${TEAM_ID:+$TEAM_ID.$BUNDLE_ID}}"
SIGN_IDENTITY="${SIGN_IDENTITY:--}"
APP_PROFILE="${APP_PROFILE:-}"
FSKIT_PROFILE="${FSKIT_PROFILE:-}"
VERSION=$(cat version)
BUILD="${BUILD_NUMBER:-$VERSION}"
MIN=15.6

ARCH_FLAGS=(); FSKIT_FLAG=()
if [ "${UNIVERSAL:-0}" = "1" ]; then ARCH_FLAGS=(--arch arm64 --arch x86_64); FSKIT_FLAG=(--universal); fi

echo "▶ swift build (release)"
swift build -c release --product NTFSTool ${ARCH_FLAGS[@]+"${ARCH_FLAGS[@]}"}
BIN=$(swift build -c release --show-bin-path ${ARCH_FLAGS[@]+"${ARCH_FLAGS[@]}"})

OUT=build
APP="$OUT/$NAME.app"
rm -rf "$APP"; mkdir -p "$APP/Contents/MacOS" "$APP/Contents/Resources"
cp "$BIN/NTFSTool" "$APP/Contents/MacOS/$NAME"
SDKV=$(xcrun --sdk macosx --show-sdk-version)
for A in $(lipo -archs "$APP/Contents/MacOS/$NAME"); do
  vtool -arch "$A" -set-build-version macos $MIN "$SDKV" -replace -output "$APP/Contents/MacOS/$NAME" "$APP/Contents/MacOS/$NAME"
done
cp -R "$BIN/NTFSTool_NTFSToolApp.bundle" "$APP/Contents/Resources/"
cp Resources/Icon/AppIcon.icns "$APP/Contents/Resources/AppIcon.icns"
# Per-language .lproj in the main bundle: macOS uses them to pick the UI language and direction (RTL for ar/fa)
LOCS=""
for d in Sources/NTFSToolApp/Resources/*.lproj; do
  l=$(basename "$d"); mkdir -p "$APP/Contents/Resources/$l"
  printf '"CFBundleDisplayName" = "%s";\n' "$NAME" > "$APP/Contents/Resources/$l/InfoPlist.strings"
  LOCS="$LOCS ${l%.lproj}"
done
python3 - "$APP/Contents/Info.plist" "$NAME" "$BUNDLE_ID" "$VERSION" "$BUILD" "$(date +%Y)" "$MIN" "$APP_GROUP" $LOCS <<'PY'
import plistlib, sys
out, name, bid, ver, build, year, minos, group, *locs = sys.argv[1:]
d = {
  "CFBundleExecutable": name, "CFBundleIdentifier": bid, "CFBundleName": name, "CFBundleDisplayName": name,
  "CFBundlePackageType": "APPL", "CFBundleShortVersionString": ver, "CFBundleVersion": build, "CFBundleInfoDictionaryVersion": "6.0",
  "CFBundleIconFile": "AppIcon", "CFBundleDevelopmentRegion": "en", "CFBundleLocalizations": locs, "CFBundleSupportedPlatforms": ["MacOSX"],
  "LSMinimumSystemVersion": minos, "LSApplicationCategoryType": "public.app-category.utilities", "NSPrincipalClass": "NSApplication",
  "NSHighResolutionCapable": True, "NSSupportsAutomaticTermination": False, "ITSAppUsesNonExemptEncryption": False,
  "NSHumanReadableCopyright": f"© 2019-{year} NTFSTool",
}
if group: d["NTFSToolAppGroup"] = group
plistlib.dump(d, open(out, "wb"))
PY

echo "▶ FSKit driver module"
FSKIT_LOG_GROUP="$APP_GROUP" FSKIT_ID="$BUNDLE_ID.fskit" FSKIT_SHORT=$FSKIT_SHORT FSKIT_AUTOMOUNT=1 FSKIT_MIN=$MIN \
  FSKIT_DISPLAY=$NAME FSKIT_VERSION=$VERSION FSKIT_BUILD=$BUILD \
  scripts/build-fskit.sh "$APP" ${FSKIT_FLAG[@]+"${FSKIT_FLAG[@]}"}
FSX="$APP/Contents/Extensions/NTFSToolFS.appex"
for A in $(lipo -archs "$FSX/Contents/MacOS/NTFSToolFS"); do
  vtool -arch "$A" -set-build-version macos $MIN "$SDKV" -replace -output "$FSX/Contents/MacOS/NTFSToolFS" "$FSX/Contents/MacOS/NTFSToolFS"
done
xattr -cr "$APP"

echo "▶ Entitlements"
ENT=.build/entitlements; mkdir -p "$ENT"
make_ent() {  # make_ent <template> <out> <application-identifier suffix>
  cp "$1" "$2"
  if [ -n "$TEAM_ID" ]; then
    /usr/libexec/PlistBuddy -c "Add :com.apple.application-identifier string $TEAM_ID.$3" \
                            -c "Add :com.apple.developer.team-identifier string $TEAM_ID" "$2"
  fi
  if [ -n "$APP_GROUP" ]; then
    /usr/libexec/PlistBuddy -c "Add :com.apple.security.application-groups array" \
                            -c "Add :com.apple.security.application-groups:0 string $APP_GROUP" "$2"
  fi
}
make_ent Resources/Entitlements/app.entitlements "$ENT/app.entitlements" "$BUNDLE_ID"
make_ent Resources/Entitlements/fskit.entitlements "$ENT/fskit.entitlements" "$BUNDLE_ID.fskit"

echo "▶ Sign ($([ "$SIGN_IDENTITY" = "-" ] && echo ad-hoc || echo "$SIGN_IDENTITY"))"
[ -n "$FSKIT_PROFILE" ] && cp "$FSKIT_PROFILE" "$FSX/Contents/embedded.provisionprofile"
[ -n "$APP_PROFILE" ] && cp "$APP_PROFILE" "$APP/Contents/embedded.provisionprofile"
RUNTIME=(); [ "$SIGN_IDENTITY" != "-" ] && RUNTIME=(--options runtime --timestamp)
codesign --force ${RUNTIME[@]+"${RUNTIME[@]}"} --entitlements "$ENT/fskit.entitlements" -s "$SIGN_IDENTITY" "$FSX"
codesign --force ${RUNTIME[@]+"${RUNTIME[@]}"} --entitlements "$ENT/app.entitlements" -s "$SIGN_IDENTITY" "$APP"
codesign --verify --deep --strict "$APP"
codesign -d --entitlements - "$APP" 2>/dev/null | grep -q app-sandbox || { echo "❌ app is not sandboxed" >&2; exit 1; }
codesign -d --entitlements - "$FSX" 2>/dev/null | grep -q fskit.fsmodule || { echo "❌ module lacks the FSKit entitlement" >&2; exit 1; }

echo "✅ $APP  $VERSION ($BUILD)  $(du -sh "$APP" | cut -f1)"
if [ "$SIGN_IDENTITY" = "-" ]; then
  echo "   Ad-hoc build: the app runs, but macOS will not load the FSKit driver without your own"
  echo "   provisioning profile (see README → Running the driver)."
fi
