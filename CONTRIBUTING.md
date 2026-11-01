# Contributing

Thanks for helping! A few rules keep the project healthy:

1. **Sign off every commit** (`git commit -s`). The `Signed-off-by:` line certifies the
   [Developer Certificate of Origin 1.1](https://developercertificate.org/): you wrote the change, or have the
   right to submit it under this project's license (MIT).
2. Run `swift test` and `make app` before opening a pull request. `make scan` must pass — never commit
   provisioning profiles, certificates, Team IDs or other signing material.
3. User-facing text goes through `L("key")`. Add the key to `Sources/NTFSToolApp/Resources/en.lproj/Localizable.strings`
   (the fallback for missing translations) and, if you can, to the other `*.lproj` files.
4. Security issues: please email service@ntfstool.com instead of opening a public issue.
