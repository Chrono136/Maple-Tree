using System.Collections.Generic;
using System.Linq;

namespace MapleLib.Structs
{
    public class Versions : List<int>
    {
        public Versions() { }

        public Versions(int[] collection) : base(collection) { }
        
        public string GetString()
        {
            return this.Aggregate(string.Empty, (current, update) => current + $"{update} ");
        }

        public new string ToString()
        {
            return GetString();
        }

        public static implicit operator Versions(int[] value)
        {
            return new Versions(value);
        }

        public static implicit operator Versions(string value)
        {
            var strList = value.Split(' ');
            var intList = new List<int>();
            foreach (var s in strList)
            {
                var i = 0;
                if (int.TryParse(s, out i))
                {
                    intList.Add(i);
                }
            }
            return new Versions(intList.ToArray());
        }

        public static explicit operator string(Versions f)
        {
            return f.GetString();
        }

        public static explicit operator int[] (Versions f)
        {
            return f.ToArray();
        }
    }
}
