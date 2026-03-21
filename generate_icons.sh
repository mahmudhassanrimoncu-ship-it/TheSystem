#!/bin/bash
# Run this ONCE on your Fedora machine to generate all icon formats.
# After running, commit the icons/ folder to your repo.
# You never need to run this again unless you change the logo.
#
# Requirements:
#   sudo dnf install inkscape ImageMagick

set -e
mkdir -p icons

SVG="restem_logo.svg"

if [ ! -f "$SVG" ]; then
    echo "ERROR: $SVG not found. Put it in the same folder as this script."
    exit 1
fi

echo "Generating PNGs from SVG..."
for SIZE in 16 32 48 64 128 256 512 1024; do
    inkscape "$SVG" \
        --export-type=png \
        --export-width=$SIZE \
        --export-height=$SIZE \
        --export-filename="icons/restem_${SIZE}.png"
    echo "  -> icons/restem_${SIZE}.png"
done

# ── Windows .ico (multi-size bundle) ───────────────────
echo "Building Windows .ico..."
convert \
    icons/restem_16.png \
    icons/restem_32.png \
    icons/restem_48.png \
    icons/restem_64.png \
    icons/restem_128.png \
    icons/restem_256.png \
    icons/reSTEM.ico
echo "  -> icons/reSTEM.ico"

# ── macOS .icns ─────────────────────────────────────────
# macOS iconutil is only available on macOS — so the macOS
# GitHub Actions workflow generates the .icns itself.
# We just ship the 1024px PNG and let the workflow do the rest.
cp icons/restem_1024.png icons/restem_mac_1024.png
echo "  -> icons/restem_mac_1024.png (macOS workflow uses this)"

# ── Linux app icon (XDG standard) ───────────────────────
cp icons/restem_256.png icons/restem.png
echo "  -> icons/restem.png (Linux XDG icon)"

echo ""
echo "Done! Now commit the icons/ folder:"
echo "  git add icons/"
echo "  git commit -m 'add: app icons for all platforms'"
echo "  git push"
