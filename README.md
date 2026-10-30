# Warhammer 40,000 Gladius: Relics of War Supreme Theme for Omarchy

An official grimdark battlefield theme for [Omarchy](https://omarchy.org/) inspired by *Warhammer 40,000 Gladius: Relics of War*.

## Theme Features

- **Official Gladius Wallpapers & Palette**: 11 official 1080p game wallpapers paired with Space Marine ceramite blue (`#3B82F6`), grimdark slate backgrounds (`#0F141D`), purity gold accents (`#E5A93C`), and dual-tone 45° active window borders.
- **Gladius Ultramarine Mouse Pointer**: Authentic tactical chevron cursor geometry from *Warhammer 40,000 Gladius: Relics of War* rendered with Macragge Ultramarine ceramite blue armor plating and a high-contrast Imperial Crimson outline. Configured at large 48px size for both XCursor and Hyprcursor.
- **Full App Dressing**: Tailored configurations for Alacritty, Foot, Ghostty, Kitty, btop, Neovim, and Omarchy status bar.

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

2. Install the Gladius Ultramarine cursor theme:
   ```bash
   mkdir -p ~/.local/share/icons/gladius-ultramarine
   cp -r cursors/xcursor ~/.local/share/icons/gladius-ultramarine/cursors
   cp -r cursors/hyprcursors ~/.local/share/icons/gladius-ultramarine/hyprcursors
   cp cursors/manifest.hl cursors/index.theme ~/.local/share/icons/gladius-ultramarine/
   ```

3. Apply the theme:
   ```bash
   omarchy theme set gladius-supreme
   ```

---

## Companion Tools & Advanced Enhancements

Looking for the optional battlefield daemons, cinematic screensaver, and voice packs?
Check out the dedicated companion suite: **[omarchy-gladius-tools](https://github.com/Semperius/omarchy-gladius-tools)**.

The companion suite provides:
- **Volumetric Smoke Trail Daemon (`gladius-smoke-trail`)**: Dynamic GTK4 layer-shell battlefield smoke trailing behind the pointer with wake-burst spotlight halos.
- **Cinematic Multi-Screen Screensaver (`gladius-screensaver-lock`)**: 1080p 60 FPS hardware-accelerated battle cinematics with focused-monitor audio routing.
- **Zero-Exposure Privacy Shield Lock (`gladius-screensaver-dismiss`)**: Mutex-protected cryptographic handoff ensuring zero desktop leakage upon wake.
- **Adeptus Mechanicus Audio Authentication (`gladius-auth-sound`)**: Cybernetic Tech-Priest voice cues praising the Omnissiah with binaric liturgies.
