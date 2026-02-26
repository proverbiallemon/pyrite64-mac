#!/bin/bash
# Builds Pyrite64 and packages it into the .app bundle.
set -euo pipefail

echo "Building Pyrite64..."
cmake --build --preset macos

echo "Updating .app bundle..."
mkdir -p Pyrite64.app/Contents/MacOS
mkdir -p Pyrite64.app/Contents/Resources
cp pyrite64 Pyrite64.app/Contents/MacOS/pyrite64
rsync -a --delete data/ Pyrite64.app/Contents/Resources/data/
rsync -a --delete n64/ Pyrite64.app/Contents/Resources/n64/

echo "Signing .app bundle..."
codesign --force --deep --sign - Pyrite64.app

echo "Done: Pyrite64.app is ready."
