using System;
using System.ComponentModel;
using System.Windows;
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
        }

        private void OnContextMenuClosing(object sender, RoutedEventArgs e)
        {
            ((MainWindowViewModel) DataContext)?.Config.RaisePropertyChangedEvent("ContextItems");
        }

        private void MetroWindow_Closing(object sender, CancelEventArgs e)
        {
            ((MainWindowViewModel) DataContext)?.Config.SaveState();
        }

        private void MetroWindow_Initialized(object sender, EventArgs e)
        {
            ((MainWindowViewModel) DataContext)?.Config.LoadState();
        }
    }
}