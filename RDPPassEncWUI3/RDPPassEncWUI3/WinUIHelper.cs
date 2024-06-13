using System;
using Microsoft.UI.Xaml.Documents;
using Microsoft.Windows.AppLifecycle;
using Windows.ApplicationModel.Activation;
using Windows.Win32;
using Windows.Win32.Foundation;

namespace RDPPassEncWUI3
{
    public class WinUIHelper
    {
        public static AppActivationArguments GetCurrentActivatedEventArgs()
        {
            return AppInstance.GetCurrent().GetActivatedEventArgs();
        }

        public static string GetLaunchActivatedEventArgs(AppActivationArguments appActivationArgs)
        {
            string launchArgs = null;
            if (appActivationArgs.Kind == ExtendedActivationKind.Launch)
            {
                ILaunchActivatedEventArgs launchActivatedEventArgs = appActivationArgs.Data as ILaunchActivatedEventArgs;
                launchArgs = launchActivatedEventArgs?.Arguments;
            }
            return launchArgs;
        }

        public static double GetScaleFactor(IntPtr hWnd)
        {
            double dpi = PInvoke.GetDpiForWindow(new HWND(hWnd));
            return dpi / 96.0;
        }

        public static int GetScaledPixel(int pixel, double scale)
        {
            return (int)(pixel * scale);
        }

        public static Run GenRunFromString(string strContent)
        {
            Run run = new Run();
            run.Text = strContent;
            return run;
        }
    }
}
