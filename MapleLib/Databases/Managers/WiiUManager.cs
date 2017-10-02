// Created: 2017/09/18 11:55 AM
// Updated: 2017/09/29 1:51 AM
// 
// Project: MapleLib
// Filename: WiiUManager.cs
// Created By: Jared T

using System;
using System.Windows.Forms;
using MapleLib.Collections;
using MapleLib.Structs;
using MapleLib.UserInterface;

namespace MapleLib.Databases.Managers
{
    public sealed class WiiUManager : IDisposable
    {
        private readonly BindingSource _bindingSource;

        private readonly MapleList<Title> _databaseState;

        private EditorForm _form;

        private WiiUManager()
        {
            //_databaseState = new MapleList<Title>(Database.GetTitles());

            _bindingSource = new BindingSource {DataSource = _databaseState};

            InitializeForm();
        }

        public static WiiUManager Instance { get; } = new WiiUManager();

        private void InitializeForm()
        {
            _form = new EditorForm
            {
                dataGridView1 = {DataSource = _bindingSource},
                bindingNavigator1 = {BindingSource = _bindingSource}
            };
        }

        public void ToggleUi()
        {
            MessageBox.Show(@"Featured disabled");
            return; //TODO: complete
            if (_form.Disposing || _form.IsDisposed)
                InitializeForm();

            if (_form.Visible)
                _form.Hide();
            else
                _form.Show();
        }

        public void AddEntry(Title title)
        {
            if (!_databaseState.Contains(title))
                _databaseState.Add(title);
        }

        public bool RemoveEntry(Title title)
        {
            return _databaseState.Remove(title);
        }

        #region IDisposable Support

        private bool _disposedValue;

        private void Dispose(bool disposing)
        {
            if (!_disposedValue)
            {
                if (disposing)
                {
                    _form.Hide();
                    _form.Dispose();
                    _form = null;
                }

                // TODO: free unmanaged resources (unmanaged objects) and override a finalizer below.
                // TODO: set large fields to null.

                _disposedValue = true;
            }
        }

        public void Dispose()
        {
            Dispose(true);
        }

        #endregion
    }
}