#!/usr/bin/env python3
"""Makes the Windows executable and window icon, port/windows/forerunner.ico:

    python tools/windows_icon.py

The 16–48 pixel frames use a compact, high-contrast version of the emblem.
The 64–256 pixel frames use port/assets/icon/forerunner.png. Needs Pillow.
The resource script, port/windows/halo.rc, embeds all frames in the executable.
"""

import struct
from io import BytesIO
from pathlib import Path

from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parent.parent
ARTWORK = ROOT / "port/assets/icon/forerunner.png"
ICON = ROOT / "port/windows/forerunner.ico"

SIZES = (16, 20, 24, 32, 40, 48, 64, 128, 256)


def compact_emblem(size: int) -> Image.Image:
    """Draw the ring and central crest with enough weight for a title bar."""
    canvas = Image.new("RGBA", (128, 128))
    draw = ImageDraw.Draw(canvas)
    ring_color = "#82e7ff"
    ring_bounds = (10, 10, 117, 117)
    draw.arc(ring_bounds, 106, 234, fill=ring_color, width=15)
    draw.arc(ring_bounds, 306, 434, fill=ring_color, width=15)
    draw.polygon(
        [(64, 16), (90, 43), (82, 69), (72, 80), (72, 119),
         (56, 119), (56, 80), (46, 69), (38, 43)],
        fill="#eef9ff",
    )
    return canvas.resize((size, size), Image.Resampling.LANCZOS)


def render(size: int) -> bytes:
    """The size-appropriate emblem as a PNG."""
    output = BytesIO()
    if size <= 48:
        icon = compact_emblem(size)
    else:
        with Image.open(ARTWORK) as artwork:
            icon = artwork.resize((size, size), Image.Resampling.LANCZOS)
    icon.save(output, format="PNG")
    return output.getvalue()


def main() -> None:
    images = [render(size) for size in SIZES]
    # the icon's header, then an entry for each image (a width or height of
    # 256 is written 0), then the images
    data = struct.pack("<HHH", 0, 1, len(images))
    offset = 6 + 16 * len(images)
    for size, image in zip(SIZES, images):
        edge = size % 256
        data += struct.pack("<BBBBHHII", edge, edge, 0, 0, 1, 32, len(image), offset)
        offset += len(image)
    ICON.write_bytes(data + b"".join(images))
    print(f"wrote {ICON.relative_to(ROOT)} ({len(SIZES)} sizes, {len(data) + sum(map(len, images))} bytes)")


if __name__ == "__main__":
    main()
