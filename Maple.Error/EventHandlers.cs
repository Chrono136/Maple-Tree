// Created: 2017/11/19 4:23 PM
// Updated: 2017/11/19 5:04 PM
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
            MapleError.CreateBug(e.ExceptionObject as Exception);
        }

        public static void DispatcherUnhandledException(object sender, DispatcherUnhandledExceptionEventArgs e)
        {
            MapleError.CreateBug(e.Exception);
        }

        public static void ThreadException(object sender, ThreadExceptionEventArgs e)
        {
            MapleError.CreateBug(e.Exception);
        }
    }
}