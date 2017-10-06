// Created: 2017/07/29 12:15 PM
// Updated: 2017/09/29 1:54 AM
// 
// Project: MapleLib
// Filename: ViewModelBase.cs
// Created By: Jared T

using System;
using System.ComponentModel;
using System.Windows.Input;

namespace MapleLib.UserInterface
{
    public abstract class ViewModelBase : INotifyPropertyChanged
    {
        #region INotifyPropertyChanged Members

        public event PropertyChangedEventHandler PropertyChanged;

        #endregion

        public void RaisePropertyChangedEvent(string propertyName)
        {
            var handler = PropertyChanged;
            handler?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }

    public class CommandHandler : ICommand
    {
        private readonly Action _action;
        private readonly bool _canExecute;

        public CommandHandler(Action action, bool canExecute = true)
        {
            _action = action;
            _canExecute = canExecute;
        }

        #region ICommand Members

        public bool CanExecute(object parameter)
        {
            return _canExecute;
        }

        public void Execute(object parameter)
        {
            _action();
        }

        public event EventHandler CanExecuteChanged;

        #endregion
    }
}