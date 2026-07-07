A Video App in C++
==================


Notes:

1. In WSL2:
```bash
export WAYLAND_DISPLAY=""
```

2. For WebCam:

2.1. In Widows PowerShell:
```bash
usbipd list
```

See the BUSID for the WebCam. Then type
```bash
usbipd attach --wsl --busid <BUSID>
```

2.2. In WSL2:
```bash
sudo ffmpeg -framerate 10 -video_size 640x480 -input_format mjpeg -i /dev/video0 -c:v libx264 -pix_fmt yuv420p video.mkv
```

3. Build FFmpeg with debug information inplace:
```bash
./configure --enable-shared --prefix=<PATH>/install --enable-debug=1 --cc=/usr/bin/gcc-13 --cxx=/usr/bin/g++-13 --shlibdir=<PATH>/lib --disable-x86asm
```

Run ffmpeg in sudo like this:
```bash
sudo env LD_LIBRARY_PATH=<PATH>/install/lib:${LD_LIBRARY_PATH} <PATH>/install/bin/ffmpeg -framerate 30 -video_size 640x480 -input_format mjpeg -i /dev/video0  -pix_fmt yuv420p video.mkv
```