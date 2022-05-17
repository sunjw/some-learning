using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.ApplicationModel.Core;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x804 上介绍了“空白页”项模板

namespace TestUWP1
{
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class MainPage : Page
    {
        private CoreApplicationViewTitleBar m_coreTitleBar;
        private ApplicationViewTitleBar m_appTitleBar;

        public MainPage()
        {
            this.InitializeComponent();

            m_coreTitleBar = CoreApplication.GetCurrentView().TitleBar;
            m_appTitleBar = ApplicationView.GetForCurrentView().TitleBar;

            ApplicationView.PreferredLaunchViewSize = new Size(640, 400);
            ApplicationView.PreferredLaunchWindowingMode = ApplicationViewWindowingMode.PreferredLaunchViewSize;

            Window.Current.SizeChanged += WindowSizeChanged;

            InitCustomTitleBar();
        }

        private void InitCustomTitleBar()
        {
            m_coreTitleBar.ExtendViewIntoTitleBar = true;
            Color bgColor = Colors.Transparent;
            Color fgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlPageTextBaseHighBrush"]).Color;
            Color inactivefgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlForegroundChromeDisabledLowBrush"]).Color;
            Color hoverbgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlBackgroundListLowBrush"]).Color;
            Color hoverfgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlForegroundBaseHighBrush"]).Color;
            Color pressedbgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlBackgroundListMediumBrush"]).Color;
            Color pressedfgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlForegroundBaseHighBrush"]).Color;
            m_appTitleBar.ButtonBackgroundColor = bgColor;
            m_appTitleBar.ButtonForegroundColor = fgColor;
            m_appTitleBar.ButtonInactiveBackgroundColor = bgColor;
            m_appTitleBar.ButtonInactiveForegroundColor = inactivefgColor;
            m_appTitleBar.ButtonHoverBackgroundColor = hoverbgColor;
            m_appTitleBar.ButtonHoverForegroundColor = hoverfgColor;
            m_appTitleBar.ButtonPressedBackgroundColor = pressedbgColor;
            m_appTitleBar.ButtonPressedForegroundColor = pressedfgColor;
        }

        private void WindowSizeChanged(object sender, Windows.UI.Core.WindowSizeChangedEventArgs e)
        {
            UpdatePopupAboutSize();
        }

        private void ShowPopupAbout()
        {
            UpdatePopupAboutSize();

            if (!PopupAbout.IsOpen)
            {
                GridMain.Visibility = Visibility.Collapsed;
                PopupAbout.IsOpen = true;
            }
        }

        public void HidePopupAbout()
        {
            if (PopupAbout.IsOpen)
            {
                PopupAbout.IsOpen = false;
                GridMain.Visibility = Visibility.Visible;
            }
        }

        private void UpdatePopupAboutSize()
        {
            if (PopupAboutContent == null)
            {
                FindName("PopupAboutContent");
            }

            Rect windowBounds = Window.Current.Bounds;
            double titleBarHeight = m_coreTitleBar.Height;
            PopupAbout.VerticalOffset = titleBarHeight;
            PopupAboutContent.Width = windowBounds.Width;
            PopupAboutContent.Height = windowBounds.Height - titleBarHeight;
        }

        private void ButtonTest_Click(object sender, RoutedEventArgs e)
        {
            if (ProgressBarMain.Value < 100)
            {
                ProgressBarMain.Value += 10;
            }
        }

        private void ButtonAbout_Click(object sender, RoutedEventArgs e)
        {
            ShowPopupAbout();
        }
    }
}
