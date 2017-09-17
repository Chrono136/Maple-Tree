using MapleLib.Collections;
using MapleLib.Structs;
using MapleLib.Databases.Editor;

namespace MapleLib.Databases
{
    public class DatabaseEditor
    {
        private MapleList<Title> DatabaseState;
        private UserInterfaceManager UIManager;

        public DatabaseEditor()
        {
            DatabaseState = new MapleList<Title>(Database.GetLibrary());

            UIManager.Activate();
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

        public void Dispose()
        {
            DatabaseState = null;
        }
    }
}
