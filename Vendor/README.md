# Vendor/NTFSToolNTFS.xcframework

Prebuilt NTFS engine used by the FSKit driver: **libntfstool-driver** (NTFSTool's own NTFS engine), taken as a
prebuilt binary from `ntfstool-driver/build/release/` (`lib/` + `include/ntfstool-driver/`).

| | |
| --- | --- |
| Library | `macos-arm64_x86_64/libntfstool-driver.a` (static, arm64 + x86_64) |
| Headers | `macos-arm64_x86_64/Headers/` (public headers and the `config.h` the library was built with) |
| Used by | `Sources/NTFSToolBridge` via `Package.swift` (`NTFSToolNTFS` binary target) and `scripts/build-fskit.sh` |
| SHA-256 of the library | `096ac724705d416e44a939e310ab7c8df923f1889082662887f9973e2543b2f7` |

The library is linked into the driver as is; nothing in this repository needs to compile it. Exported symbols
use the `ntfsdrv_` prefix. To update it, rebuild `ntfstool-driver` (`./build.sh`) and re-create the xcframework:

```sh
R=../ntfstool-driver/build/release
xcodebuild -create-xcframework -library $R/lib/libntfstool-driver.a \
  -headers $R/include/ntfstool-driver -output Vendor/NTFSToolNTFS.xcframework
```
