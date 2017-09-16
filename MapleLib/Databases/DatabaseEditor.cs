using MapleLib.Collections;
using MapleLib.Structs;

namespace MapleLib.Databases
{
    public class DatabaseEditor
    {
        private MapleList<Title> DatabaseState;

        public DatabaseEditor()
        {
            DatabaseState = new MapleList<Title>(Database.GetLibrary());
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
