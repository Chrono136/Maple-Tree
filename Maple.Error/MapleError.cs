// Created: 2017/11/19 10:53 AM
// Updated: 2017/11/19 8:48 PM
// 
// Project: Maple.Error
// Filename: MapleError.cs
// Created By: Jared T

using System;
using System.Windows;
using Octokit;

namespace Maple.Error
{
    public static class MapleError
    {
        private static string SessionId => Guid.NewGuid().ToString().Replace("-", "");

        public static void Initialize()
        {
            AppDomain.CurrentDomain.FirstChanceException += (sender, eventArgs) => { CreateBug(eventArgs.Exception); };
            //System.Windows.Forms.Application.SetUnhandledExceptionMode(UnhandledExceptionMode.ThrowException);
            //System.Windows.Forms.Application.ThreadException += EventHandlers.ThreadException;
            //AppDomain.CurrentDomain.UnhandledException += EventHandlers.UnhandledException;
            //System.Windows.Application.Current.DispatcherUnhandledException += EventHandlers.DispatcherUnhandledException;
        }

        public static async void CreateBug(Exception ex)
        {
            const string message = "{0}\n\nWould you like to submit this error to the developer?\nYour issue ID will be copied to your clipboard.";
            var mbResult = MessageBox.Show(string.Format(message, ex.Message), "Error Occurred", MessageBoxButton.YesNo);
            if (mbResult != MessageBoxResult.Yes)
                return;

            var github = new GitHubClient(new ProductHeaderValue("MapleSeed"));
            var tokenAuth = new Credentials("315f0d4a929da095bcf9de7b198c07dd65b397c8");
            github.Credentials = tokenAuth;

            var dateTime = new DateTimeWithZone(DateTime.Now, TimeZoneInfo.Utc);

            var createIssue = new NewIssue($"Error Report {SessionId}")
            {
                Body = $"**Time Stamp:** {dateTime.UniversalTime}\n" +
                       $"**Issue ID:** {SessionId}\n" +
                       "\n" +
                       $">**Source:** {ex.Source}\n" +
                       $"**Message:** {ex.Message}\n" +
                       $"**Stack Trace:** {ex.StackTrace}\n"
            };
            var issue = await github.Issue.Create("Tsume", "Maple-Tree", createIssue);
        }
    }
}