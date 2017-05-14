// Project: MapleLib
// File: IBaseCollection.cs
// Updated By: Jared
// 

using System.Threading.Tasks;
using LiteDB;
using MapleLib.Collections;

namespace MapleLib.Interfaces
{
    public interface IDatabase<T>
    {
        LiteDatabase LiteDatabase { get; }

        string CollectionName { get; }

        int Count { get; }

        void Load();

        void InitDatabase();

        MapleList<T> Find(string id);
    }
}