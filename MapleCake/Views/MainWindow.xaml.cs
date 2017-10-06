// Created: 2017/03/27 11:20 AM
// Updated: 2017/10/06 12:20 PM
// 
// Project: MapleCake
// Filename: MainWindow.xaml.cs
// Created By: Jared T

using System;
using System.ComponentModel;
using System.Windows;
using System.Windows.Input;
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

        private void UIElement_OnPreviewMouseRightButtonDown(object sender, MouseEventArgs e)
        {
            var dataContext = (MainWindowViewModel) DataContext;

            if (dataContext != null)
                dataContext.Config.ContextItems = MapleContext.CreateMenu();
        }
    }
}