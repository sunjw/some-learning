using System;
using System.Runtime.InteropServices;
using Microsoft.UI;
using Microsoft.UI.Input;
using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Navigation;
using Microsoft.Windows.AppLifecycle;
using Windows.Graphics;
using Windows.UI;
using Windows.UI.ViewManagement;
using Windows.Win32;
using Windows.Win32.Foundation;
using Windows.Win32.UI.Shell;
using Windows.Win32.UI.WindowsAndMessaging;
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
        private const int appMinWidth = 400;
        private const int appMinHeight = 420;
        private const nuint subclassId = 18;

        private UISettings m_uiSettings;
        private Page m_pageCurrent = null;
        private SUBCLASSPROC m_subclassProc = null;
        //private bool m_confirmExit = false;

        public static MainWindow CurrentWindow { get; private set; } = null;

        public bool IsAppPackaged { get; private set; } = false;
        public IntPtr HWNDHandle { get; private set; } = 0;
        public double Scale { get; private set; } = 1.0;

        public Windows.Foundation.Point PointCursor { get; private set; } = new Windows.Foundation.Point(0, 0);

        public MainWindow()
        {
            InitializeComponent();

            IsAppPackaged = Win32Helper.IsAppPackaged();
            HWNDHandle = WindowNative.GetWindowHandle(this);
            Scale = Win32Helper.GetScaleFactor(HWNDHandle);
            m_uiSettings = new UISettings();

            ExtendsContentIntoTitleBar = true;
            SetTitleBar(AppTitleBar);

            Closed += MainWindow_Closed;
            m_uiSettings.ColorValuesChanged += UISettings_ColorValuesChanged;

            InitWindowSize();
            InitWindowSubclass();
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

        private void UpdateTitleBarColor()
        {
            AppWindowTitleBar curTitleBar = AppWindow.TitleBar;
            if (curTitleBar == null)
                return;

            ElementTheme curElementTheme = MainFrame.ActualTheme;
            Color fgColor = Colors.Black;
            if (curElementTheme == ElementTheme.Dark)
                fgColor = Colors.White;

            Application.Current.Resources["WindowCaptionForeground"] = fgColor;
            AppWindow.TitleBar.ButtonForegroundColor = fgColor;

            //Color bgColor = Colors.Transparent;
            //Color fgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlPageTextBaseHighBrush"]).Color;
            //Color inactivefgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlForegroundChromeDisabledLowBrush"]).Color;
            //Color hoverbgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlBackgroundListLowBrush"]).Color;
            //Color hoverfgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlForegroundBaseHighBrush"]).Color;
            //Color pressedbgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlBackgroundListMediumBrush"]).Color;
            //Color pressedfgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlForegroundBaseHighBrush"]).Color;

            //AppWindow.TitleBar.ButtonBackgroundColor = bgColor;
            //AppWindow.TitleBar.ButtonForegroundColor = fgColor;
            //AppWindow.TitleBar.ButtonInactiveBackgroundColor = bgColor;
            //AppWindow.TitleBar.ButtonInactiveForegroundColor = inactivefgColor;
            //AppWindow.TitleBar.ButtonHoverBackgroundColor = hoverbgColor;
            //AppWindow.TitleBar.ButtonHoverForegroundColor = hoverfgColor;
            //AppWindow.TitleBar.ButtonPressedBackgroundColor = pressedbgColor;
            //AppWindow.TitleBar.ButtonPressedForegroundColor = pressedfgColor;
        }

        private void InitWindowSize()
        {
            AppWindow.Resize(new(Win32Helper.GetScaledPixel(appMinWidth, Scale), Win32Helper.GetScaledPixel(appMinWidth, Scale)));
        }

        private void InitWindowSubclass()
        {
            m_subclassProc = new SUBCLASSPROC(WndSubProc);
            PInvoke.SetWindowSubclass(new HWND(HWNDHandle), m_subclassProc, subclassId, 0);
        }

        private LRESULT WndSubProc(HWND hWnd, uint uMsg, WPARAM wParam, LPARAM lParam, nuint uIdSubclass, nuint dwRefData)
        {
            if (uMsg == PInvoke.WM_GETMINMAXINFO)
            {
                var minMaxInfo = Marshal.PtrToStructure<MINMAXINFO>(lParam);
                minMaxInfo.ptMinTrackSize.X = Win32Helper.GetScaledPixel(appMinWidth, Scale);
                minMaxInfo.ptMinTrackSize.Y = Win32Helper.GetScaledPixel(appMinWidth, Scale);
                Marshal.StructureToPtr(minMaxInfo, lParam, false);
            }

            return PInvoke.DefSubclassProc(hWnd, uMsg, wParam, lParam);
        }

        private void MainFrame_Loaded(object sender, RoutedEventArgs e)
        {
            CurrentWindow = this;
            UpdateTitleBarColor();
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

        private void UISettings_ColorValuesChanged(UISettings sender, object args)
        {
            DispatcherQueue.TryEnqueue(UpdateTitleBarColor);
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
