// Created: 2017/03/27 11:20 AM
// Updated: 2017/09/29 1:56 AM
// 
// Project: MapleLib
// Filename: MapleList.cs
// Created By: Jared T

using System;
using System.Collections.Generic;
using System.ComponentModel;
using Newtonsoft.Json.Utilities;

namespace MapleLib.Collections
{
    [Serializable]
    public class MapleList<T> : BindingList<T>
    {
        public MapleList() : base(new List<T>())
        {
            AddItemEvent += OnAddItemEvent;
        }

        public MapleList(IEnumerable<T> list) : base(new List<T>())
        {
            this.AddRange(list);
            AddItemEvent += OnAddItemEvent;
        }

        public event EventHandler<AddItemEventArgs<T>> AddItemEvent;

        public new void Add(T item)
        {
            AddItemEvent?.Invoke(this, new AddItemEventArgs<T>(this, item));
        }

        private void OnAddItemEvent(object sender, AddItemEventArgs<T> e)
        {
            if (!Contains(e.Item))
                base.Add(e.Item);
        }
    }

    public class AddItemEventArgs<T> : EventArgs
    {
        public readonly T Item;
        private object _sender;

        public AddItemEventArgs(object sender, T item)
        {
            _sender = sender;
            Item = item;
        }
    }
}