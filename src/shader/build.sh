#!/usr/bin/env bash
set -e
cd "$(dirname "$0")"

OUT_DIR="../../data/shader"

mkdir -p "$OUT_DIR"

for shader in *.vert.glsl *.frag.glsl; do
    # Skip if glob didn’t match anything
    [ -e "$shader" ] || continue

    base="${shader%.glsl}"          # n64.vert or n64.frag
    name="${base%.*}"               # n64
    stage="${base##*.}"              # vert or frag

    spv="$OUT_DIR/$base.spv"
    hlsl="$OUT_DIR/$base.hlsl"
    dxil="$OUT_DIR/$base.dxil"

    echo "=== Compiling $shader ($stage) ==="

    # GLSL -> SPIR-V
    glslc -fshader-stage="$stage" "$shader" -o "$spv"

    # SPIR-V -> HLSL
    spirv-cross \
        --hlsl \
        --shader-model 50 \
        --entry main \
        --stage "$stage" \
        "$spv" > "$hlsl"

    # HLSL -> DXIL
    if [ "$stage" = "vert" ]; then
        dxc -T vs_6_0 -E main "$hlsl" -Fo "$dxil"
    else
        dxc -T ps_6_0 -E main "$hlsl" -Fo "$dxil"
    fi

    rm "$hlsl"
done