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

        private IntPtr hWnd = 0;
        private Page currentPage = null;

        public MainWindow()
        {
            InitializeComponent();

            hWnd = WindowNative.GetWindowHandle(this);

            InitWindowSize();

            ExtendsContentIntoTitleBar = true;
            SetTitleBar(AppTitleBar);
        }

        private bool IsCurrentMainPage()
        {
            return currentPage is MainPage;
        }

        public void OnRedirected(AppActivationArguments args)
        {
            if (IsCurrentMainPage())
            {
                string appActivateArgs = WinUIHelper.GetLaunchActivatedEventArgs(args);
                (currentPage as MainPage).OnRedirected(appActivateArgs);
            }
        }

        public IntPtr GetHWNDHandle()
        {
            return hWnd;
        }

        public double GetScaleFactor()
        {
            double dpi = PInvoke.GetDpiForWindow(new HWND(hWnd));
            return dpi / 96.0;
        }

        private void InitWindowSize()
        {
            double scale = GetScaleFactor();
            AppWindow.Resize(new((int)(appInitWidth * scale), (int)(appInitHeight * scale)));
        }

        private void MainFrame_Loaded(object sender, RoutedEventArgs e)
        {
            MainFrame.Navigate(typeof(MainPage));
        }

        private void MainFrame_Navigated(object sender, NavigationEventArgs e)
        {
            bool isAppStart = false;
            if (currentPage == null)
            {
                isAppStart = true;
            }
            currentPage = e.Content as Page;
            if (isAppStart && IsCurrentMainPage())
            {
                AppActivationArguments args = WinUIHelper.GetCurrentActivatedEventArgs();
                string appActivateArgs = WinUIHelper.GetLaunchActivatedEventArgs(args);
                (currentPage as MainPage).OnRedirected(appActivateArgs);
            }
        }
    }
}
