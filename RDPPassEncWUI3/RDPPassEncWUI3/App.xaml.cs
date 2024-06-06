using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml;
using Microsoft.Windows.AppLifecycle;
using Windows.ApplicationModel.Activation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace RDPPassEncWUI3
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    public partial class App : Application
    {
        private Window mainWindow;

        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App()
        {
            InitializeComponent();
        }

        /// <summary>
        /// Invoked when the application is launched.
        /// </summary>
        /// <param name="args">Details about the launch request and process.</param>
        protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
        {
            mainWindow = new MainWindow();
            mainWindow.Activate();
        }

        public void OnRedirected(AppActivationArguments args)
        {
            if (mainWindow != null)
            {
                string appActivateArgs = "";
                if (args.Kind == ExtendedActivationKind.Launch)
                {
                    ILaunchActivatedEventArgs launchActivatedEventArgs = args.Data as ILaunchActivatedEventArgs;
                    appActivateArgs = launchActivatedEventArgs?.Arguments;
                }
                mainWindow.DispatcherQueue.TryEnqueue(DispatcherQueuePriority.Normal, () =>
                {
                    (mainWindow as MainWindow).OnRedirected(appActivateArgs);
                });
            }
        }
    }
}
