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
        public MainPage()
        {
            this.InitializeComponent();

            ApplicationView.PreferredLaunchViewSize = new Size(640, 400);
            ApplicationView.PreferredLaunchWindowingMode = ApplicationViewWindowingMode.PreferredLaunchViewSize;

            CoreApplicationViewTitleBar coreTitleBar = CoreApplication.GetCurrentView().TitleBar;
            coreTitleBar.ExtendViewIntoTitleBar = true;
            
            ApplicationViewTitleBar appTitleBar = ApplicationView.GetForCurrentView().TitleBar;
            Color bgColor = Colors.Transparent;
            Color fgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlPageTextBaseHighBrush"]).Color;
            Color inactivefgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlForegroundChromeDisabledLowBrush"]).Color;
            Color hoverbgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlBackgroundListLowBrush"]).Color;
            Color hoverfgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlForegroundBaseHighBrush"]).Color;
            Color pressedbgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlBackgroundListMediumBrush"]).Color;
            Color pressedfgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlForegroundBaseHighBrush"]).Color;
            appTitleBar.ButtonBackgroundColor = bgColor;
            appTitleBar.ButtonForegroundColor = fgColor;
            appTitleBar.ButtonInactiveBackgroundColor = bgColor;
            appTitleBar.ButtonInactiveForegroundColor = inactivefgColor;
            appTitleBar.ButtonHoverBackgroundColor = hoverbgColor;
            appTitleBar.ButtonHoverForegroundColor = hoverfgColor;
            appTitleBar.ButtonPressedBackgroundColor = pressedbgColor;
            appTitleBar.ButtonPressedForegroundColor = pressedfgColor;
        }

        private void ButtonTest_Click(object sender, RoutedEventArgs e)
        {
            if (ProgressMain.Value < 100)
            {
                ProgressMain.Value += 10;
            }
        }

        private void ButtonAbout_Click(object sender, RoutedEventArgs e)
        {
            if (PopupAboutContent == null)
            {
                FindName("PopupAboutContent");
            }

            var windowBounds = Window.Current.Bounds;
            PopupAboutContent.Width = windowBounds.Width;
            PopupAboutContent.Height = windowBounds.Height;

            if (!PopupAbout.IsOpen)
            {
                PopupAbout.IsOpen = true;
            }
        }
    }
}
