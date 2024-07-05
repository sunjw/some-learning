using System;
using Microsoft.UI.Input;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Navigation;
using Microsoft.Windows.AppLifecycle;
using Windows.Graphics;
using WinRT.Interop;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace RDPPassEncWUI3
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        private const int appInitWidth = 400;
        private const int appInitHeight = 420;

        private Page m_pageCurrent = null;
        private bool m_confirmExit = false;

        public static MainWindow CurrentWindow { get; private set; } = null;

        public IntPtr HWNDHandle { get; private set; } = 0;
        public Windows.Foundation.Point PointCursor { get; private set; } = new Windows.Foundation.Point(0, 0);

        public MainWindow()
        {
            InitializeComponent();

            HWNDHandle = WindowNative.GetWindowHandle(this);

            InitWindowSize();

            ExtendsContentIntoTitleBar = true;
            SetTitleBar(AppTitleBar);

            Closed += MainWindow_Closed;
        }

        private bool IsMainPageCurrent()
        {
            return m_pageCurrent is MainPage;
        }

        private bool IsAboutPageCurrent()
        {
            return m_pageCurrent is AboutPage;
        }

        public void OnRedirected(AppActivationArguments args)
        {
            if (IsMainPageCurrent())
            {
                string appActivateArgs = WinUIHelper.GetLaunchActivatedEventArgs(args);
                (m_pageCurrent as MainPage).OnRedirected(appActivateArgs);
            }
        }

        public System.Drawing.Point GetCursorRelativePoint()
        {
            System.Drawing.Point pointRelative = new System.Drawing.Point(0, 0);

            System.Drawing.Point pointPointer = Win32Helper.GetPointerPoint();

            if (AppWindow != null)
            {
                PointInt32 pointAppWindow = AppWindow.Position;
                pointRelative.X = pointPointer.X - pointAppWindow.X;
                pointRelative.Y = pointPointer.Y - pointAppWindow.Y;
            }

            return pointRelative;
        }

        private void InitWindowSize()
        {
            double scale = Win32Helper.GetScaleFactor(HWNDHandle);
            AppWindow.Resize(new(Win32Helper.GetScaledPixel(appInitWidth, scale), Win32Helper.GetScaledPixel(appInitHeight, scale)));
        }

        private void MainFrame_Loaded(object sender, RoutedEventArgs e)
        {
            CurrentWindow = this;
            MainFrame.Navigate(typeof(MainPage));
        }

        private void MainFrame_Navigated(object sender, NavigationEventArgs e)
        {
            bool isAppStart = false;
            if (m_pageCurrent == null)
            {
                isAppStart = true;
            }
            m_pageCurrent = e.Content as Page;
            if (isAppStart && IsMainPageCurrent())
            {
                AppActivationArguments args = WinUIHelper.GetCurrentActivatedEventArgs();
                string appActivateArgs = WinUIHelper.GetLaunchActivatedEventArgs(args);
                (m_pageCurrent as MainPage).OnRedirected(appActivateArgs);
            }
        }

        private /*async*/ void MainWindow_Closed(object sender, WindowEventArgs args)
        {
            //if (m_confirmExit)
            //{
            //    return;
            //}

            //args.Handled = true;
            //ContentDialog dialog = new()
            //{
            //    XamlRoot = Content.XamlRoot,
            //    Title = "Exit?",
            //    PrimaryButtonText = "Yes",
            //    CloseButtonText = "No",
            //    DefaultButton = ContentDialogButton.Close
            //};
            //dialog.PrimaryButtonClick += (s, e) =>
            //{
            //    m_confirmExit = true;
            //    DispatcherQueue.TryEnqueue(Close);
            //};
            //await dialog.ShowAsync();

            if (IsAboutPageCurrent())
            {
                AboutPage aboutPageCur = m_pageCurrent as AboutPage;
                if (aboutPageCur.ThreadRunning)
                {
                    args.Handled = true;
                    aboutPageCur.StopThread(() => DispatcherQueue.TryEnqueue(Close));
                }
            }
        }

        private void GridRoot_PointerMoved(object sender, PointerRoutedEventArgs e)
        {
            PointerPoint pp = e.GetCurrentPoint(GridRoot);
            PointCursor = pp.Position;

            string strDebug = string.Format("{0:0.00} : {1:0.00}", PointCursor.X, PointCursor.Y);
            if (IsAboutPageCurrent())
            {
                //(m_pageCurrent as AboutPage).UpdateDebugString(strDebug);
            }
        }

        private void GridRoot_Tapped(object sender, TappedRoutedEventArgs e)
        {
            Windows.Foundation.Point pointTapped = e.GetPosition(GridRoot);
            string strDebug = string.Format("{0:0.00} : {1:0.00}", pointTapped.X, pointTapped.Y);
            if (IsAboutPageCurrent())
            {
                //(m_pageCurrent as AboutPage).UpdateDebugString(strDebug);
            }
        }
    }
}
