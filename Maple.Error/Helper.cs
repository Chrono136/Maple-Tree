// Created: 2017/11/24 7:20 PM
// Updated: 2017/11/24 7:31 PM
// 
// Project: Maple.Error
// Filename: Helper.cs
// Created By: Jared T

using System;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;

namespace Maple.Error
{
    public static class Helper
    {
        public static object InvokeOnCurrentDispatcher(Action action)
        {
            return Application.Current?.Dispatcher?.Invoke(DispatcherPriority.Normal, action);
        }

        public static string ToBase64(this string text)
        {
            var bytes = Encoding.UTF8.GetBytes(text);
            return Convert.ToBase64String(bytes);
        }

        public static string FromBase64(this string base64)
        {
            var bytes = Convert.FromBase64String(base64);
            return Encoding.UTF8.GetString(bytes);
        }
    }

    public static class TaskExtensions
    {
        public static Task ObserveExceptions(this Task task)
        {
            return task.ContinueWith(t =>
            {
                ThreadPool.QueueUserWorkItem(w =>
                {
                    if (t.Exception == null) return;
                    foreach (var ex in t.Exception.InnerExceptions)
                        throw ex;
                });
            }, TaskContinuationOptions.OnlyOnFaulted | TaskContinuationOptions.PreferFairness);
        }
    }
}