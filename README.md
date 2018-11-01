Adwaita themed wayland decorations for qt 
=========================================

## How to Build and Install

If some private qt headers can not be found on your system,
you have to compile [qtwayland](https://github.com/qt/qtwayland) 
yourself and specify its location.

```
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr \
 -DPRIVATE_QT_HEADER_DIR="path_to_qtwayland_builtdir/include/QtWaylandClient/5.11.2" ../
make install
```
## Usage
Make sure Qt uses its wayland backend. If it doesn't, set the 
QT_QPA_PLATFORM environment variable to wayland or wayland-egl.
To enable adwaita decorations set QT_WAYLAND_DECORATION to adwaita. 
If fonts appear to large set QT_WAYLAND_FORCE_DPI to 96 or 96 multiplied 
with your font scaling factor.

## Run the Test Application
You can run the test application without installing it or the decorations
directly from the build directory.
```
export QT_QPA_PLATFORM=wayland
export QT_WAYLAND_FORCE_DPI=96
build/bin/testapp
```

