using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using System;
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
        private const int appInitWidth = 480;
        private const int appInitHeight = 480;

        IntPtr hWnd = 0;
        private Page currentPage = null;

        public MainWindow()
        {
            InitializeComponent();

            hWnd = WindowNative.GetWindowHandle(this);

            AppWindow.Resize(new(appInitWidth, appInitHeight));

            ExtendsContentIntoTitleBar = true;
            SetTitleBar(AppTitleBar);
        }

        private bool IsCurrentMainPage()
        {
            return currentPage is MainPage;
        }

        public void OnRedirected(string someArgs)
        {
            if (IsCurrentMainPage())
            {
                (currentPage as MainPage).OnRedirected(someArgs);
            }
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
                string someArgs = "";
                string[] programeArgs = Program.GetProgramArgs();
                if (programeArgs != null)
                {
                    someArgs = string.Join(",", programeArgs);
                }
                (currentPage as MainPage).OnRedirected(someArgs);
            }
        }
    }
}
