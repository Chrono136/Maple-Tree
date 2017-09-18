using MapleLib.Collections;
using MapleLib.Structs;
using MapleLib.UserInterface;
using System;
using System.Windows.Forms;

namespace MapleLib.Databases.Managers
{
    public class WiiUManager : IDisposable
    {
        public static WiiUManager Instance { get; } = new WiiUManager();

        public EditorForm Form;

        private MapleList<Title> DatabaseState;
        private BindingSource BindingSource;

        public WiiUManager()
        {
            DatabaseState = new MapleList<Title>(Database.GetTitles());
            
            BindingSource = new BindingSource();
            BindingSource.DataSource = DatabaseState;

            InitializeForm();
        }

        private void InitializeForm()
        {
            Form = new EditorForm();
            Form.dataGridView1.DataSource = BindingSource;
            Form.bindingNavigator1.BindingSource = BindingSource;
        }

        public void ToggleUI()
        {
            if (Form.Disposing || Form.IsDisposed)
                InitializeForm();

            if (Form.Visible)
                Form.Hide();
            else
                Form.Show();
        }

        public void AddEntry(Title title)
        {
            if (!DatabaseState.Contains(title))
                DatabaseState.Add(title);
        }

        public bool RemoveEntry(Title title)
        {
            return DatabaseState.Remove(title);
        }

        #region IDisposable Support
        private bool disposedValue = false; // To detect redundant calls

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    Form.Hide();
                    Form.Dispose();
                    Form = null;
                }

                // TODO: free unmanaged resources (unmanaged objects) and override a finalizer below.
                // TODO: set large fields to null.

                disposedValue = true;
            }
        }

        public void Dispose()
        {
            Dispose(true);
        }
        #endregion
    }
}
