# LBEAST Unreal Plugin Restructure Plan

## Current Problem

The repository structure mixes plugin code (`Plugins/LBEAST/`) with project code (`Source/LBEAST_Unreal/`, `.uproject`, etc.). This makes it impossible to clone directly into a user's project without copying extra files.

## Goal

Restructure so cloning the repository gives you a clean plugin folder that can be directly copied to `YourProject/Plugins/LBEAST/`.

## Proposed Solution

**Make the repository root BE the plugin folder structure:**

```
LBEAST_Unreal/  (repo root)
├── LBEAST.uplugin          ← Plugin definition (moved from Plugins/LBEAST/)
├── Source/                  ← Plugin source (moved from Plugins/LBEAST/Source/)
│   ├── LBEASTCore/
│   ├── AIFacemask/
│   ├── LargeHaptics/
│   ├── EmbeddedSystems/
│   ├── LBEASTExperiences/
│   ├── ProAudio/
│   └── Examples/           ← NEW: Example code (moved from Source/LBEAST_Unreal/)
│       └── ServerManager/  ← Example Server Manager implementation
├── Content/                 ← Plugin content (from Plugins/LBEAST/Content/)
├── Resources/               ← Plugin resources (from Plugins/LBEAST/Resources/)
├── README.md
└── LICENSE
```

## Migration Steps

1. **Move plugin files to root:**
   - Move `Plugins/LBEAST/LBEAST.uplugin` → `LBEAST.uplugin`
   - Move `Plugins/LBEAST/Source/*` → `Source/*`
   - Move `Plugins/LBEAST/Content/` → `Content/`
   - Move `Plugins/LBEAST/Resources/` → `Resources/`

2. **Move example code into plugin:**
   - Move `Source/LBEAST_Unreal/ServerManager/` → `Source/Examples/ServerManager/`
   - Create `Source/Examples/Examples.Build.cs` module (optional)
   - Update ServerManager to use proper plugin module references

3. **Handle project files:**
   - Option A: Move to `DemoProject/` subfolder (keep for testing)
   - Option B: Remove from distribution (use separate repo for demo)
   - Option C: Keep minimal `.uproject` for plugin development/testing

4. **Update paths:**
   - Update any hardcoded paths in code
   - Update build files if needed
   - Update documentation

## Installation After Restructure

**Simple one-command install:**
```bash
cd YourProject/Plugins
git clone https://github.com/ajcampbell1333/LBEAST_Unreal.git LBEAST
```

That's it! The plugin is ready to use.

## Benefits

✅ **Clean installation** - Clone = Install, no copying needed
✅ **Standard Unreal pattern** - Matches how most Unreal plugins are distributed
✅ **Version control friendly** - Users can update with `git pull`
✅ **No project pollution** - Only plugin files in the repo

## Alternative: Keep Current Structure but Add Install Script

If restructuring is too disruptive, we could:
- Keep current structure
- Add `install-plugin.sh` / `install-plugin.bat` scripts
- Scripts extract just the plugin folder to user's Plugins directory
- But this is less clean than proper restructure

