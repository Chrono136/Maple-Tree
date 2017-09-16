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

        public void Dispose()
        {
            DatabaseState = null;

        }
    }
}
