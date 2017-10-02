// Created: 2017/09/17 10:25 PM
// Updated: 2017/10/02 10:25 AM
// 
// Project: MapleLib
// Filename: Versions.cs
// Created By: Jared T

using System;
using System.Collections.Generic;
using System.Linq;

namespace MapleLib.Structs
{
    public class Versions : List<int>
    {
        public Versions() { }

        private Versions(IEnumerable<int> collection) : base(collection) { }

        private static string GetString(Versions versions)
        {
            return versions.Aggregate(string.Empty, (current, update) => current + $"{update} ");
        }

        public new string ToString()
        {
            return GetString(this);
        }

        public static implicit operator Versions(int[] value)
        {
            return new Versions(value);
        }

        public static implicit operator Versions(string value)
        {
            var chars = new[] {' ', ','};
            var strList = value.Split(chars);
            var intList = new List<int>();
            foreach (var s in strList)
            {
                int i;
                if (int.TryParse(s, out i))
                    intList.Add(i);
            }
            return new Versions(intList.ToArray());
        }

        public static explicit operator string(Versions f)
        {
            return GetString(f);
        }

        public static explicit operator int[](Versions f)
        {
            return f.ToArray();
        }
    }
}