// Created: 2017/11/19 4:23 PM
// Updated: 2017/11/24 8:18 PM
// 
// Project: Maple.Error
// Filename: EventHandlers.cs
// Created By: Jared T

using System;
using System.Threading;
using System.Windows.Threading;

namespace Maple.Error
{
    public static class EventHandlers
    {
        public static void UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            MapleError.SendIssue(MapleError.CreateIssue(e.ExceptionObject as Exception));
        }

        public static void DispatcherUnhandledException(object sender, DispatcherUnhandledExceptionEventArgs e)
        {
            e.Handled = true;
            MapleError.SendIssue(MapleError.CreateIssue(e.Exception));
        }

        public static void ThreadException(object sender, ThreadExceptionEventArgs e)
        {
            MapleError.SendIssue(MapleError.CreateIssue(e.Exception));
        }
    }
}