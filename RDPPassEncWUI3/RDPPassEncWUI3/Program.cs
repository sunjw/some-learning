using System;
using System.Threading;
using System.Threading.Tasks;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml;
using Microsoft.Windows.AppLifecycle;

namespace RDPPassEncWUI3
{
    public static class Program
    {
        private static string[] programArgs = null;

        [STAThread]
        static void Main(string[] args)
        {
            WinRT.ComWrappersSupport.InitializeComWrappers();
            programArgs = args;

            bool isRedirect = DecideRedirection();
            if (!isRedirect)
            {
                Application.Start((p) =>
                {
                    var context = new DispatcherQueueSynchronizationContext(
                        DispatcherQueue.GetForCurrentThread());
                    SynchronizationContext.SetSynchronizationContext(context);
                    new App();
                });
            }

            return;
        }

        private static bool DecideRedirection()
        {
            bool isRedirect = false;
            AppActivationArguments args = WinUIHelper.GetCurrentActivatedEventArgs();
            AppInstance keyInstance = AppInstance.FindOrRegisterForKey("RDPPassEncWUI3");

            if (keyInstance.IsCurrent)
            {
                keyInstance.Activated += OnActivated;
            }
            else
            {
                isRedirect = true;
                RedirectActivationTo(keyInstance, args);
            }
            return isRedirect;
        }

        public static void RedirectActivationTo(AppInstance keyInstance, AppActivationArguments args)
        {
            var redirectSemaphore = new Semaphore(0, 1);
            Task.Run(() =>
            {
                keyInstance.RedirectActivationToAsync(args).AsTask().Wait();
                redirectSemaphore.Release();
            });
            redirectSemaphore.WaitOne();
        }

        private static void OnActivated(object sender, AppActivationArguments args)
        {
            // ExtendedActivationKind kind = args.Kind;
            App appCurrent = Application.Current as App;
            if (appCurrent != null)
            {
                appCurrent.OnRedirected(args);
            }
        }

        public static string[] GetProgramArgs()
        {
           return programArgs;
        }
    }
}
