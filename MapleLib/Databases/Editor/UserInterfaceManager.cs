using MapleLib.Databases.Editor.UI;
using System;

namespace MapleLib.Databases.Editor
{
    public class UserInterfaceManager : IDisposable
    {
        private EditorForm Form;

        public UserInterfaceManager()
        {
            if (Form == null)
                Form = new EditorForm();
        }

        public void Activate()
        {
            Form.Activate();
            Form.Show();
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
