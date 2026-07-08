#!/usr/bin/env python3
"""One-shot converter: Conan's Tiled TMX -> Storm Engine v2 editor .map format
plus a JRPG-style colliders file. Run from the Conan repo root."""
import base64, zlib, struct, xml.etree.ElementTree as ET

TMX = "data/gfx/map1.tmx"
OUT_TILES = "data/conan.map"
OUT_COLL  = "data/conan_colliders.map"

TILE = 32
SPACING, MARGIN, COLS = 2, 2, 18  # both tilesets: 18 cols x 11 rows of 32px

tree = ET.parse(TMX)
root = tree.getroot()
W = int(root.get("width")); H = int(root.get("height"))

tilesets = []  # (firstgid, name)
for ts in root.findall("tileset"):
    tilesets.append((int(ts.get("firstgid")), ts.get("name")))
tilesets.sort(reverse=True)  # match highest firstgid first

def decode(layer):
    data = layer.find("data")
    raw = zlib.decompress(base64.b64decode(data.text.strip()))
    gids = struct.unpack("<%dI" % (len(raw)//4), raw)
    return gids

def src_for(gid):
    for firstgid, name in tilesets:
        if gid >= firstgid:
            local = gid - firstgid
            col, row = local % COLS, local // COLS
            return name, MARGIN + col*(TILE+SPACING), MARGIN + row*(TILE+SPACING)
    raise ValueError(gid)

layers = {l.get("name"): decode(l) for l in root.findall("layer")}

# ── Visual layers -> editor .map ────────────────────────────────────────────
# Collision is the visible level geometry in this map (750 of ~870 tiles) —
# it is drawn AND solid, book-style. Bottom sits behind it.
Z = {"Bottom": 0, "Collision": 1, "Overlay": 2, "Overlay2": 3}
count = 0
with open(OUT_TILES, "w") as f:
    for lname, z in Z.items():
        gids = layers[lname]
        for i, gid in enumerate(gids):
            if gid == 0: continue
            x, y = (i % W) * TILE, (i // W) * TILE
            asset, sx, sy = src_for(gid)
            f.write(f"tiles {asset} {TILE} {TILE} {sx} {sy} {z} "
                    f"{x} {y} 1 1 0 0\n")
            count += 1
print(f"tiles written: {count}")

# ── Collision layer -> colliders file (horizontal runs merged) ──────────────
coll = layers["Collision"]
runs = 0
with open(OUT_COLL, "w") as f:
    for row in range(H):
        col = 0
        while col < W:
            if coll[row*W + col] == 0:
                col += 1; continue
            start = col
            while col < W and coll[row*W + col] != 0:
                col += 1
            wpx = (col - start) * TILE
            f.write(f"collider {start*TILE} {row*TILE} 1 1 {wpx} {TILE} 0 0\n")
            runs += 1
print(f"collider runs written: {runs}")

# ── Object spawns (for hardcoding in PlayState) ─────────────────────────────
for og in root.findall("objectgroup"):
    for o in og.findall("object"):
        print("spawn:", o.get("type"), o.get("x"), o.get("y"))
