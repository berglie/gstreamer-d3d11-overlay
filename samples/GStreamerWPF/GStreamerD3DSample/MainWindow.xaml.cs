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
                _D3D11Scene = new D3D11TestScene();

                /* Set the backbuffer, which is a ID3D11Texture2D pointer */
                var renderTarget = _D3D11Scene.GetRenderTarget();
                _d3DImageEx.SetBackBufferEx(D3DResourceTypeEx.ID3D11Texture2D, renderTarget);

                _playback = new Playback(renderTarget, _enableOverlay);
                _playback.OnDrawSignalReceived += VideoSink_OnBeginDraw;
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

            Dispatcher.BeginInvoke( System.Windows.Threading.DispatcherPriority.Background, (Action)(() => InvalidateD3DImage()));
        }
        /// <summary>
        /// Invalidates entire D3DImage area
        /// </summary>
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
