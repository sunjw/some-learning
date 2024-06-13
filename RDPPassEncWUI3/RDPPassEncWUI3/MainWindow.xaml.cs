using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using Microsoft.Windows.AppLifecycle;
using System;
using WinRT.Interop;
using Windows.Win32;
using Windows.Win32.Foundation;

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
            double scale = WinUIHelper.GetScaleFactor(m_hWnd);
            AppWindow.Resize(new(WinUIHelper.GetScaledPixel(appInitWidth, scale), WinUIHelper.GetScaledPixel(appInitHeight, scale)));
        }

        private void MainFrame_Loaded(object sender, RoutedEventArgs e)
        {
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
    }
}
