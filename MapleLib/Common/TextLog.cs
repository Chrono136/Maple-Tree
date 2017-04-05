// Project: MapleLib
// File: TextLog.cs
// Updated By: Jared
// 

using System;
using System.Drawing;
using System.Threading.Tasks;
using MapleLib.Collections;

namespace MapleLib.Common
{
    public sealed class TextLog
    {
        public static TextLog MesgLog { get; } = new TextLog();
        public static TextLog ChatLog { get; } = new TextLog();
        public static TextLog StatusLog { get; } = new TextLog();

        private MapleList<string> LogHistory { get; } = new MapleList<string>();

        private int index { get; set; }
        
        public event EventHandler<NewLogEntryEvent> NewLogEntryEventHandler;

        public void AddHistory(string text, Color color = default(Color))
        {
            index = LogHistory.Count;
            LogHistory.Add(text);
        }

        private Task NewLine(string text, Color color = default(Color))
        {
            AddHistory(text, color);
            NewLogEntryEventHandler?.Invoke(this, new NewLogEntryEvent(text, color));
            return null;
        }

        public void WriteLog(string text, Color color = default(Color))
        {
            NewLine(text + Environment.NewLine, color);
        }

        public void WriteError(string text)
        {
            var color = Color.DarkRed;
            NewLine(text + Environment.NewLine, color);
        }

        public static void Write(string text)
        {
            MesgLog?.WriteLog(text);
        }
    }

    public class NewLogEntryEvent : EventArgs
    {
        public NewLogEntryEvent(string entry, Color color = default(Color))
        {
            Entry = $"[{DateTime.Now.TimeStamp()}] {entry}";
            Entry = $"{entry}";
            Color = color;
        }

        public string Entry { get; private set; }

        public Color Color { get; private set; }
    }
}