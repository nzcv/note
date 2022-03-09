## windows 出包

https://zhuanlan.zhihu.com/p/339673291

https://www.msys2.org/wiki/MSYS2-reinstallation/

注意:
如果以前有使用vcpkg. 删除vcpkg环境变量 CMAKE_TOOLCHAIN_FILE D:\vcpkg

## Android原理解析

![20220222094301](https://cdn.jsdelivr.net/gh/nzcv/picgo/20220222094301.png)

1. 权限

```xml
<uses-permission android:name="android.permission.MANAGE_EXTERNAL_STORAGE"/>
<uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
<uses-permission android:name="android.permission.INTERNET"/>
```
2. opengles3.0

```xml
<uses-feature android:glEsVersion="0x00030000" android:required="true"/>
```

3. Loader.java
```java
// 保持屏幕常亮
getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

// 权限获取与跳转应用信息界面
Intent viewIntent = new Intent( "android.settings.MANAGE_ALL_FILES_ACCESS_PERMISSION"  /*android.provider.Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION*/);
startActivity(viewIntent);

// 加载renderdoccmd
System.loadLibrary("renderdoccmd"); // this will load VkLayer_GLES_RenderDoc as well
```

4. 关于VKLayer

https://developer.android.com/ndk/guides/graphics
