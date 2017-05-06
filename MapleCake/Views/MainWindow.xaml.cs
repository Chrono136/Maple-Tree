using System;
using System.Windows;
using System.Windows.Threading;
using MapleCake.Models;
using MapleCake.ViewModels;

namespace MapleCake.Views
{
    /// <summary>
    ///     Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        public MainWindow()
        {
            InitializeComponent();
            return;
            var timer = new DispatcherTimer {Interval = new TimeSpan(0, 0, 0, 0, 50)};
            timer.Tick += (sender, e) => {
                _contentCtrl.Height += 10;
                var count = Math.Abs(_scrollViewer.VerticalOffset - _scrollViewer.ScrollableHeight);
                if (count > 0) {
                    _scrollViewer.ScrollToEnd();
                }
            };
            timer.Start();
        }

        private void OnContextMenuClosing(object sender, RoutedEventArgs e)
        {
            ((MainWindowViewModel) DataContext)?.Config.RaisePropertyChangedEvent("ContextItems");
        }

        private void MetroWindow_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            ((MainWindowViewModel) DataContext)?.Config.SaveState();
        }

        private void MetroWindow_Initialized(object sender, EventArgs e)
        {
            ((MainWindowViewModel)DataContext)?.Config.LoadState();
        }
    }
}