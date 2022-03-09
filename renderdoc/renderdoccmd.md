renderdoccmd使用安卓启动后,第一个加载的so文件. 这里面是跨平台实现的.

## 需要阅读的源码有:

```shell
renderdoccmd.cpp
renderdoccmd.h
3rdparty/cmdline/cmdline.h

renderdoccmd_android.cpp
android_native_app_glue.c //这个再ndk目录下
```

## 使用CMake编译进行apk编译, 具体实现为在RenderDocCmd的CMakeLists.txt里面:

```shell
1. 使用java8以上
2. 检查是否需要剔除符号
3. 查找最新的buildTools
4. 查找最新的ANDROID_PLATFORM
5. 生成证书
6. APK_VERSION_CODE/APK_VERSION_NAME用的git的commit hash
7. 打包成APK, 其中重要的依赖renderdoc/renderdoccmd
8. 签名生成APK
```
## 使用了原生Android开发

https://developer.android.com/ndk/samples/sample_na

![20220222110201](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220222110201.png)


## 重点关注cmdthread

```shell
Core 17612 11:21:38 android_tools.cpp(321) Log COMMAND: D:/Android/sdk/platform-tools/adb '-s e06bd82c shell am start -n org.renderdoc.renderdoccmd.arm64/.Loader -e renderdoccmd remoteserver'

Core 17612 11:44:47 android_tools.cpp(321) Log COMMAND: D:/Android/sdk/platform-tools/adb '-s e06bd82c push "C:\Users\Administrator\AppData\Roaming\renderdoc\renderdoc.conf" /sdcard/Android/data/org.renderdoc.renderdoccmd.arm64/files/renderdoc.conf'
```

```c
void *cmdthread(void *)
{
  std::vector<std::string> args = getRenderdoccmdArgs();
  if(args.size())
  {    
    ANDROID_LOG("Entering cmd thread, args:");
    for(auto &e: args) {
      ANDROID_LOG("%s", e.c_str()); //renderdoccmd,remoteserver 
    }    
    m_CmdLock.lock();
    GlobalEnvironment env;
    renderdoccmd(env, args); //命令解析
    m_CmdLock.unlock();
    ANDROID_LOG("Exiting cmd thread");
  }

  // activity is done and should be closed
  ANativeActivity_finish(android_state->activity);

  return NULL;
}
```

### vulkanlayer命令

### RemoteServerCommand

Start up a server listening as a host for remote replays.

判断renderdoc在哪个平台以何种方式启动

```c++
//RenderDoc/core.cpp
RDCLOG("RenderDoc v%s %s %s %s (%s) %s", MAJOR_MINOR_VERSION_STRING, platform,
         sizeof(uintptr_t) == sizeof(uint64_t) ? "64-bit" : "32-bit",
         ENABLED(RDOC_RELEASE) ? "Release" : "Development", GitVersionHash,
         IsReplayApp() ? "loaded in replay application" : "capturing application");
```

默认情况以replay模式启动

```shell
RenderDoc v1.19 Android 64-bit Development (6bd8bcee606dc697c39c57ee3ecaded22eea9ccf) loaded in replay application
```

当你使用launchApplication方式启动时

```shell
RenderDoc v1.19 Android 64-bit Development (6bd8bcee606dc697c39c57ee3ecaded22eea9ccf) capturing application
```

针对Android10, 带有native GPU layering.

```c++
if(Android::SupportsNativeLayers(m_deviceID))
    {
      RDCLOG("Using Android 10 native GPU layering");

      // if we have Android 10 native layering, don't use JDWP hooking
      hookWithJDWP = false;
```

如何使用native debug layering的?

```c++
D:/Android/sdk/platform-tools/adb '-s e06bd82c shell settings put global enable_gpu_debug_layers 1'
D:/Android/sdk/platform-tools/adb '-s e06bd82c shell settings put global gpu_debug_app com.YourCompany.Demo'
D:/Android/sdk/platform-tools/adb '-s e06bd82c shell settings put global gpu_debug_layer_app org.renderdoc.renderdoccmd.arm64'
D:/Android/sdk/platform-tools/adb '-s e06bd82c shell settings put global gpu_debug_layers VK_LAYER_RENDERDOC_Capture'
D:/Android/sdk/platform-tools/adb '-s e06bd82c shell settings put global gpu_debug_layers_gles libVkLayer_GLES_RenderDoc.so'
```

https://community.arm.com/cn/b/blog/posts/gl_5f00_khr_5f00_debug-arm-mali-gpu-opengl-es

https://hub.vive.com/storage/docs/zh-cn/UnrealPlugin/RenderDoc/Unreal_RenderDoc.html


## 关于Android10 使用GLES图层问题

https://developer.android.google.cn/ndk/guides/rootless-debug-gles?hl=zh-cn

重点关注 AndroidGLESLayer_Initialize

eglCreateContext_renderdoc_hooked





