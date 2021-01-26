# gstreamer-d3d11-overlay
An RTSP WPF client using GStreamer with Direct3D11/Direct3D9 interop layer

[GStreamer](https://gstreamer.freedesktop.org/documentation/?gi-language=c) is a framework used for creating streaming media applications. In this project we are creating a player which can play RTSP streams in WPF. 

There is no direct support in WPF to display UI elements over a D3D component due to the airspace issue. This project will tackle this issue by implementing a Direct3D11/Direct3D9 interop layer and by using the D3DImageEx class. 

The D3DImageEx class will create a D3D11Texture which is created with the `D3D11_RESOURCE_MISC_SHARED`. This texture will be used for the SetBackBuffer method and later used for the d3d11videosink element in the gstreamer code. 

Building the C++ projects
------------------------------
**What You Need**

 - [Visual Studio 2015](https://www.visualstudio.com/features/wpf-vs)
 - [DirectX SDK](http://www.microsoft.com/en-us/download/details.aspx?id=6812)
 - [Windows SDK](https://dev.windows.com/en-us/downloads/windows-10-sdk)

Credits
-------------------
The Direct3D11/Direct3D9 interop layer is based on the project http://jmorrill.hjtcentral.com/Home/tabid/428/EntryId/437/Direct3D-10-11-Direct2D-in-WPF.aspx by Jeremiah Morrill. 
