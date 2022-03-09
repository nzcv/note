
## RenderDoc编译

```shell

pacman -S mingw-w64-x86_64-gcc

mkdir build-android
cd build-android
cmake -DBUILD_ANDROID=On -DANDROID_ABI=armeabi-v7a -G "MinGW Makefiles" ..
mingw32-make.exe -j8

mkdir build-android64
cd build-android64
cmake -DBUILD_ANDROID=On -DANDROID_ABI=armeabi-v7a -G "MinGW Makefiles" ..
mingw32-make.exe -j8
```

```shell

$ cd build-android/
$ cmake -DBUILD_ANDROID=On -DANDROID_ABI=armeabi-v7a -G "MinGW Makefiles" ..
$ mingw32-make -j8
$ pacman -S mingw-w64-x86_64-gcc

$ mkdir build-android64
$ cd build-android64
$ cmake -DBUILD_ANDROID=On -DANDROID_ABI=arm64-v8a -G "MinGW Makefiles" ..
$ mingw32-make -j8
```

```
Android  PID  30661: [17:33:50]                   DEBUG(0) - Fatal   - #01 pc 0000000000326328  /vendor/lib64/egl/libGLESv2_adreno.so (BuildId: 8f58bda1c70fa129f4e013d6faea0796)
Android  PID  30661: [17:33:50]                   DEBUG(0) - Fatal   - #02 pc 00000000002fcf84  /data/app/~~I24wdy6z_mDknwbevQIPLQ==/org.renderdoc.renderdoccmd.arm64-LXyx77_s43GH2ByH4_QeIg==/lib/arm64/libVkLayer_GLES_RenderDoc.so (WrappedOpenGL::glDeleteBuffers(int, unsigned int const*)+428) (BuildId: 1d2ed789c210126bd808bb12076cd7f66ac64c3e)
```