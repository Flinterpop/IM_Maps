# IM_Maps
Based on private repo: IM_RadarSites

ImGui version of FMX Radar Sites
Branched from immaps

14 Apr 2025 Version
slippy map tile background with a single SRTM elevation grid shown.

![image](https://github.com/user-attachments/assets/bfeccbae-7596-40ee-a252-722d2c8ad309)




## Requirements

- OpenGL
- C++17 compiler
- Executable needs the Microsoft VC Runtime:
- https://aka.ms/vs/17/release/vc_redist.x64.exe

**Note**: Compiled and tested with MSVC 2022
It was developed with ImGui v1.91.9 (Feb 2025) Docking branch. If you use a later version with breaking changes then you may need to update ImMaps to accomodate those changes.

## Build Instructions
1. Clone this repository, [Im_RadarSites](https://github.com/flinterpop/IM_RadarSites), and [ImGui](https://github.com/ocornut/imgui) into a parent directory (name doesn't matter):

```shell
mkdir root
cd root
git clone https://github.com/flinterpop/IM_RadarSites
git clone https://github.com/ocornut/imgui --branch=docking
```

or if the current version of ImGui has relevant breaking changes clone ImGui with this branch number:
```
git clone https://github.com/ocornut/imgui --branch v1.91.6
```

- `root/`
    - `imgui/`
    - `IM_RadarSites/`
2. Build with CMake, e.g.:
```shell
cd IM_RadarSites
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Demos

ezpent's 'maps.cpp`is an OpenStreetMap world map viewer. Downloads and displays zoomable tile maps in a plot.
maps](https://github.com/epezent/implot_demos/blob/master/screenshots/maps.png)


### 19 Jan 2025 Version:
![image](https://github.com/user-attachments/assets/0d117c05-403f-4f9c-abfa-0d54a78a4670)

