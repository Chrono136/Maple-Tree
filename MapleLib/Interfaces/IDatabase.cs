// Created: 2017/05/14 5:25 AM
// Updated: 2017/09/29 2:05 AM
// 
// Project: MapleLib
// Filename: IDatabase.cs
// Created By: Jared T

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