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
    public class MapleList<T> : BindingList<T>
    {
        public event EventHandler<AddItemEventArgs<T>> AddItemEvent;

        public MapleList() : base(new List<T>())
        {
            AddItemEvent += OnAddItemEvent;
        }

        public MapleList(IEnumerable<T> list) : base(new List<T>())
        {
            this.AddRange(list);
            AddItemEvent += OnAddItemEvent;
        }

        /// <inheritdoc />
        public new void Add(T item)
        {
            AddItemEvent?.Invoke(this, new AddItemEventArgs<T>(this, item));
        }

        private void OnAddItemEvent(object sender, AddItemEventArgs<T> e)
        {
            if (!Contains(e.item))
                base.Add(e.item);
        }
    }

    public class AddItemEventArgs<T> : EventArgs
    {
        public object sender;
        public readonly T item;

        public AddItemEventArgs(object sender, T item)
        {
            this.sender = sender;
            this.item = item;
        }
    }
}