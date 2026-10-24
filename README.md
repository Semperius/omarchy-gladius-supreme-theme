# Warhammer 40,000 Gladius: Relics of War Supreme Theme for Omarchy

An all-in-one grimdark battlefield theme for [Omarchy](https://omarchy.org/) featuring:
- **Gladius Ultramarine Mouse Pointer**: Authentic tactical chevron cursor geometry from *Warhammer 40,000 Gladius: Relics of War* rendered with Macragge Ultramarine ceramite blue armor plating and a high-contrast Imperial Crimson outline. Configured at large 48px size.
- **Volumetric Smoke Trail with Wake Spotlight**: A native, hyper-optimized GTK4 layer-shell daemon (`gladius-smoke-trail`) producing atmospheric battlefield smoke puffs trailing dynamically behind the pointer whenever moved, featuring adaptive socket polling (down to ~28 Hz idle backoff), a 60 FPS capped rendering engine, and an automatic radial wake-burst halo that instantly spotlights the cursor after being stationary.
- **Official Gladius Wallpapers & Palette**: 11 official 1080p game wallpapers, paired with Space Marine ceramite blue (`#3B82F6`), grimdark slate backgrounds (`#0F141D`), purity gold accents (`#E5A93C`), and dual-tone 45° active window borders.
- **Battlefield Screensaver with Password Lock**: Multi-monitor terminal screensaver with rotating ASCII art banners and looping *Gladius: Relics of War* soundtrack audio, automatically securing the machine with password lock upon dismissal or idle timeout.

---

## Quick Install (Omarchy)

```bash
omarchy theme install https://github.com/Semperius/omarchy-gladius-supreme-theme
```

Or via Omarchy menu:
`Install -> Style -> Themes` and enter the repository URL.

To apply immediately:
```bash
omarchy theme set gladius-supreme
```

---

## Manual Installation

1. Copy the theme directory into your Omarchy themes folder:
   ```bash
   cp -r . ~/.config/omarchy/themes/gladius-supreme
   ```

2. Install the Gladius Ultramarine cursor theme:
   ```bash
   mkdir -p ~/.local/share/icons/gladius-ultramarine
   cp -r cursors/xcursor ~/.local/share/icons/gladius-ultramarine/cursors
   cp -r cursors/hyprcursors ~/.local/share/icons/gladius-ultramarine/hyprcursors
   cp cursors/manifest.hl cursors/index.theme ~/.local/share/icons/gladius-ultramarine/
   ```

3. Build and install the smoke trail daemon:
   ```bash
   # Dependencies: gtk4, gtk4-layer-shell, cairo
   cd smoke-trail && make install && cd ..
   ```

4. Install the screensaver scripts and systemd timer:
   ```bash
   cp screensaver/gladius-screensaver-runner screensaver/gladius-screensaver-lock screensaver/gladius-rotation-manager ~/.local/bin/
   mkdir -p ~/.config/omarchy/branding/gladius_screensavers
   cp screensaver/art/* ~/.config/omarchy/branding/gladius_screensavers/
   mkdir -p ~/.config/systemd/user
   cp systemd/* ~/.config/systemd/user/
   systemctl --user daemon-reload
   systemctl --user enable --now gladius-rotation.timer
   ```

5. Install the theme automation hook:
   ```bash
   omarchy hook install theme-set hooks/gladius-supreme.sh
   ```

6. Apply the theme:
   ```bash
   omarchy theme set gladius-supreme
   ```

---

## Submission Details for omarchy.org/themes/

- **Site Thumbnail**: `gladius-supreme.webp` (1200x675)
- **JSON Entry** for `src/data/themes.json` on `omacom/omarchy-site`:
  ```json
  {
    "repo": "https://github.com/Semperius/omarchy-gladius-supreme-theme",
    "image": "/assets/themes/gladius-supreme.webp",
    "name": "Gladius Supreme"
  }
  ```
