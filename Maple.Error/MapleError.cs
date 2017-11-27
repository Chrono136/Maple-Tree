// Created: 2017/11/19 10:53 AM
// Updated: 2017/11/24 8:40 PM
// 
// Project: Maple.Error
// Filename: MapleError.cs
// Created By: Jared T

using System;
using System.Diagnostics;
using System.Windows;
using System.Windows.Forms;
using Octokit;
using Application = System.Windows.Forms.Application;
using MessageBox = System.Windows.MessageBox;

namespace Maple.Error
{
    public static class MapleError
    {
        private static GitHubClient GitHub { get; set; }

        private static string Token => "OWZlZTM2N2RlN2Y3NWEzZGZjNTRjZDU4M2VlODk0NDEwYTE2MTUxZQ==";

        private static string SessionId => Guid.NewGuid().ToString().Replace("-", "");

        private static bool Initialized { get; set; }

        private static string Version { get; set; }

        public static void Initialize(string version)
        {
            if (Initialized) return;
            Initialized = true;

            Version = version;

            GitHub = new GitHubClient(new ProductHeaderValue("MapleSeed")) {Credentials = new Credentials(Token.FromBase64())};

            Application.SetUnhandledExceptionMode(UnhandledExceptionMode.ThrowException);
            Application.ThreadException += EventHandlers.ThreadException;

            AppDomain.CurrentDomain.UnhandledException += EventHandlers.UnhandledException;
            System.Windows.Application.Current.DispatcherUnhandledException += EventHandlers.DispatcherUnhandledException;
            //AppDomain.CurrentDomain.FirstChanceException += (sender, eventArgs) => { CreateBug(eventArgs.Exception); };
        }

        public static NewIssue CreateIssue(Exception ex)
        {
            var result = MessageBoxResult.Cancel;
            Helper.InvokeOnCurrentDispatcher(() =>
            {
                const string message = "{0}\n\nWould you like to submit this error to the developer?" +
                                       "\nYour issue ID will be copied to your clipboard.";

                Window owner;
                if ((owner = System.Windows.Application.Current.MainWindow) != null)
                    result = MessageBox.Show(owner, string.Format(message, ex.Message), "Error Occurred", MessageBoxButton.YesNo);
            });

            if (result != MessageBoxResult.Yes && GitHub != null)
                return null;

            return new NewIssue($"Bug Report - {SessionId}")
            {
                Body =
                    $"**Version:** {Version}\n" +
                    $"**Time Stamp:** {DateTimeWithZone.UniversalTime}\n" +
                    $"**Issue ID:** {SessionId}\n" +
                    "\n" +
                    $">**Source:** {ex.Source}\n" +
                    $"**Message:** {ex.Message}\n" +
                    $"**Stack Trace:** {ex.StackTrace}\n"
            };
        }

        public static async void SendIssue(NewIssue newIssue)
        {
            if (newIssue == null) return;
            var issue = await GitHub.Issue.Create("Tsume", "Maple-Tree", newIssue);
            issue.ToUpdate().State = ItemState.Closed;

            Helper.InvokeOnCurrentDispatcher(() =>
            {
                MessageBox.Show($"Navigating to bug report. \n{issue.HtmlUrl}", "Bug Report Submitted");
                Process.Start(issue.HtmlUrl);
            });
        }
    }
}