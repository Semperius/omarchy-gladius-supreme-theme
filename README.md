# Warhammer 40,000 Gladius: Relics of War Supreme Theme for Omarchy

An official grimdark battlefield theme for [Omarchy](https://omarchy.org/) inspired by *Warhammer 40,000 Gladius: Relics of War*.

## Theme Features

- **37 Full-HD Gladius Wallpapers & Palette**:
  - **12 Quad-Panel Faction & Imperial Archive Illustrations** (`08-quadtych-01-adepta-sororitas.jpg` through `19-quadtych-12-imperial-archive.jpg`) combining all 44 *Faction*, *Quest*, *Research*, and *Victory* paintings.
  - **11 Faction Cinematic Snapshots** (`20-snapshot-01-adepta-sororitas.jpg` through `30-snapshot-11-tyranids.jpg`) featuring authentic sub-faction heraldry (`Captain Corezon – Ultramarines, 2nd Company`, `Order of Our Martyred Lady`, `Cohort of Mars`, `8th Cadian Regiment`, `Host of Biel-Tan`, `The Crimson Slaughter`, `Kabal of the Poisoned Tongue`, `Sautekh Dynasty`, `Goff Clan`, `Commander Shadowsun – Vior'la Sept`, and `Hive Fleet Leviathan`).
  - **14 Battlefield & Warp Storm Wallpapers** including the Ultramarine standing beneath the swirling Warp Storm (`05-space-marine-warp-storm.jpg`), paired with Space Marine ceramite blue (`#3B82F6`), grimdark slate backgrounds (`#0F141D`), and purity gold accents (`#E5A93C`).
- **Astartes-Pattern Power Sword Cursor (`Model: Castrum-9`)**:
  - Forged to **Tech-Priest Omega-92's Technical Data Package (`032.M42`)** featuring a mirror-polished liquid-chrome Adamantium blade, crackling high-voltage cyan-and-white disruption lightning arcs across 32 animation frames, exposed copper disruption coils, an Imperialis winged-skull crossguard, a 5-ribbed ceramite-weave grip, and a pulsing MK.III micro-fusion core pommel (`24px`–`144px` for both XCursor and Hyprcursor, inheriting `Adwaita`).
- **Pure Declarative Theme Package**: Contains zero executable code or scripts in full compliance with the Omarchy theme gallery specification.

---

## Installation

### Standard Install (Omarchy)

```bash
omarchy theme install https://github.com/Semperius/omarchy-gladius-supreme-theme
```

Or via the Omarchy application menu:
`Install -> Style -> Themes` and enter the repository URL.

To apply immediately:
```bash
omarchy theme set gladius-supreme
```

### Manual Install

1. Copy the theme directory into your Omarchy themes folder:
   ```bash
   cp -r . ~/.config/omarchy/themes/gladius-supreme
   ```

2. Install the Castrum-9 Astartes Power Sword cursor theme:
   ```bash
   mkdir -p ~/.local/share/icons/astartes-powersword
   cp -r cursors/xcursor ~/.local/share/icons/astartes-powersword/cursors
   cp -r cursors/hyprcursors ~/.local/share/icons/astartes-powersword/hyprcursors
   cp cursors/manifest.hl cursors/index.theme ~/.local/share/icons/astartes-powersword/
   hyprctl setcursor astartes-powersword 144
   ```

3. Apply the theme:
   ```bash
   omarchy theme set gladius-supreme
   ```

---

## Companion Tools & Advanced Enhancements

Looking for the optional battlefield daemons, 11-faction cinematic screensaver suite, and Canoness Penelope neural voice pack?
Check out the dedicated companion suite: **[omarchy-gladius-tools](https://github.com/Semperius/omarchy-gladius-tools)**.
