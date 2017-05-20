// Project: MapleLib
// File: MapleList.cs
// Updated By: Jared
// 

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

        /// <inheritdoc />
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