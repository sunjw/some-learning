using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// https://go.microsoft.com/fwlink/?LinkId=234238 上介绍了“空白页”项模板

namespace TestUWP1
{
    /// <summary>
    /// 可用于自身或导航至 Frame 内部的空白页。
    /// </summary>
    public sealed partial class About : UserControl
    {
        public About()
        {
            this.InitializeComponent();
        }

        private MainPage GetMainPage()
        {
            Frame frame = (Frame)Window.Current.Content;

            if (frame != null)
            {
                MainPage mainPage = (MainPage)frame.Content;
                return mainPage;
            }

            return null;
        }

        private void ButtonOK_Click(object sender, RoutedEventArgs e)
        {
            MainPage mainPage = GetMainPage();
            mainPage?.HidePopupAbout();
        }
    }
}
