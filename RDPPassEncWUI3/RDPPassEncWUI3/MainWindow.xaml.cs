using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using Microsoft.Windows.AppLifecycle;
using System;
using WinRT.Interop;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Input;

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

        private IntPtr m_hWnd = 0;
        private Page m_pageCurrent = null;

        public static MainWindow CurrentWindow { get; private set; }

        public MainWindow()
        {
            InitializeComponent();

            m_hWnd = WindowNative.GetWindowHandle(this);

            InitWindowSize();

            ExtendsContentIntoTitleBar = true;
            SetTitleBar(AppTitleBar);
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

        public IntPtr GetHWNDHandle()
        {
            return m_hWnd;
        }

        private void InitWindowSize()
        {
            double scale = Win32Helper.GetScaleFactor(m_hWnd);
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

        private void GridRoot_PointerMoved(object sender, PointerRoutedEventArgs e)
        {
            PointerPoint pp = e.GetCurrentPoint(GridRoot);
            string strDebug = string.Format("{0:0.00} : {1:0.00}", pp.Position.X, pp.Position.Y);
            if (IsAboutPageCurrent())
            {
                (m_pageCurrent as AboutPage).UpdateDebugString(strDebug);
            }
        }
    }
}
