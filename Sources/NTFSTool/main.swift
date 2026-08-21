// Copyright © 2019-2026 NTFSTool.
// SPDX-License-Identifier: MIT
// main.swift
//
// Entry point of the NTFSTool executable. All the real code lives in the
// NTFSToolApp library (AppKit shell, SwiftUI views and the model) and in
// NTFSToolCore (pure logic); keeping `main` this small lets the test target
// import and exercise everything without launching an app.
//
// NTFSTool is a sandboxed app: it never mounts NTFS itself. macOS mounts NTFS
// drives read-write through the app's FSKit module once the user has turned
// the file system extension on in System Settings.

import NTFSToolApp

NTFSToolAppMain.run()
