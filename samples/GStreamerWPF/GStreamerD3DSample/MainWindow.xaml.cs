using System;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Media;
using D3D11Scene;
using Gst;

namespace GStreamerD3D.Samples.WPF.D3D11
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private D3DImageEx _d3DImageEx;
        private D3D11TestScene _D3D11Scene;
        private Playback _playback;

        private const bool _enableOverlay = true;
        private const bool _enableSoftwareFallback = true;

        public MainWindow()
        {
            InitializeComponent();
            Utilities.DetectGstPath();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            if (_enableOverlay)
            {
                _d3DImageEx = new D3DImageEx();
                d3dScene.Source = _d3DImageEx;
                _D3D11Scene = new D3D11TestScene(1920, 1080);

                /* Set the backbuffer, which is a ID3D11Texture2D pointer */
                var renderTarget = _D3D11Scene.GetRenderTarget();
                var backBuffer = _d3DImageEx.CreateBackBuffer(D3DResourceTypeEx.ID3D11Texture2D, renderTarget);

                _d3DImageEx.Lock();
                _d3DImageEx.SetBackBuffer(D3DResourceType.IDirect3DSurface9, backBuffer, _enableSoftwareFallback);
                _d3DImageEx.Unlock();

                _playback = new Playback(renderTarget, _enableOverlay);
                _playback.OnDrawSignalReceived += VideoSink_OnBeginDraw;

                CompositionTarget.Rendering += CompositionTarget_Rendering;
            }
            else
            {
                var windowHandle = new WindowInteropHelper(System.Windows.Application.Current.MainWindow).Handle;
                _playback = new Playback(windowHandle, _enableOverlay);
            }
        }
        private void VideoSink_OnBeginDraw(Element sink, GLib.SignalArgs args)
        {        
            var sharedHandle = _D3D11Scene.GetSharedHandle();
            _ = sink.Emit("draw", sharedHandle, (UInt32)2, (UInt64)0, (UInt64)0);
        }
        private void CompositionTarget_Rendering(object sender, EventArgs e)
        {
            InvalidateD3DImage();
        }
        private void InvalidateD3DImage()
        {
            _d3DImageEx.Lock();
            _d3DImageEx.AddDirtyRect(new Int32Rect()
            {
                X = 0,
                Y = 0,
                Height = _d3DImageEx.PixelHeight,
                Width = _d3DImageEx.PixelWidth
            });
            _d3DImageEx.Unlock();
        }
        protected override void OnClosed(EventArgs e)
        {
            base.OnClosed(e);
            _playback.Cleanup();
            System.Windows.Application.Current.Shutdown();
        }
    }
}
