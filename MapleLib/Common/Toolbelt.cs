// Created: 2017/03/27 11:20 AM
// Updated: 2017/10/14 2:54 PM
// 
// Project: MapleLib
// Filename: Toolbelt.cs
// Created By: Jared T

#region usings

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using libWiiSharp;
using MapleLib.Properties;
using MapleLib.WiiU;
using NUS_Downloader;

#endregion

namespace MapleLib.Common
{
    public static class Toolbelt
    {
        public static bool LaunchCemu(string game, GraphicPack pack)
        {
            try
            {
                var cemuPath = Path.Combine(Settings.CemuDirectory, "cemu.exe");

                if (game.IsNullOrEmpty() && File.Exists(cemuPath))
                {
                    RunCemu(Path.Combine(Settings.CemuDirectory, "cemu.exe"), string.Empty);
                }
                else
                {
                    string rpx = null;
                    var dir = Path.GetDirectoryName(Path.GetDirectoryName(game));

                    if (string.IsNullOrEmpty(dir))
                        return false;

                    var files = Directory.GetFiles(dir, "*.rpx", SearchOption.AllDirectories);
                    if (files.Any())
                        rpx = files.First();

                    if (File.Exists(cemuPath) && File.Exists(rpx))
                    {
                        pack?.Apply();
                        RunCemu(cemuPath, rpx);
                        pack?.Remove();
                    }
                    else
                    {
                        SetStatus("Could not find a valid .rpx");
                    }
                }
            }
            catch (Exception e)
            {
                AppendLog($"{e.Message}\n{e.StackTrace}", Color.DarkRed);
                return false;
            }

            return true;
        }

        private static void RunCemu(string cemuPath, string rpx)
        {
            try
            {
                var workingDir = Path.GetDirectoryName(cemuPath);
                if (workingDir == null) return;

                var o1 = Settings.FullScreenMode ? "-f" : "";
                using (TextWriter writer = File.CreateText(Path.Combine(Settings.ConfigDirectory, "cemu.log")))
                {
                    StartProcess(cemuPath, $"{o1} -g \"{rpx}\"", workingDir, null, true, false, writer);
                }
            }
            catch (Exception ex)
            {
                TextLog.MesgLog.WriteError("Error!\r\n" + ex.Message);
            }
        }

        public static string Ric(string str)
        {
            return RemoveInvalidCharacters(str);
        }

        private static string RemoveInvalidCharacters(string str)
        {
            if (string.IsNullOrEmpty(str))
                return string.Empty;

            str = str.Replace("™", "");
            str = str.Replace("®", "");

            return
                Path.GetInvalidPathChars()
                    .Aggregate(str, (current, c) => current.Replace(c.ToString(), " "))
                    .Replace(':', ' ').Replace("(", "").Replace(")", "");
        }

        public static void AppendLog(string msg, Color color = default(Color))
        {
            TextLog.MesgLog.WriteLog(msg, color);
        }

        public static void SetStatus(string msg, Color color = default(Color))
        {
            TextLog.StatusLog.WriteLog(msg, color);
        }

        public static string SizeSuffix(long bytes)
        {
            const int scale = 1024;
            string[] orders = {"GB", "MB", "KB", "Bytes"};
            var max = (long) Math.Pow(scale, orders.Length - 1);

            foreach (var order in orders)
            {
                if (bytes > max)
                    return $"{decimal.Divide(bytes, max):##.##} {order}";

                max /= scale;
            }
            return "0 Bytes";
        }

        public static bool IsValid(TMD_Content content, string contentFile)
        {
            if (!File.Exists(contentFile)) return false;

            return (ulong) new FileInfo(contentFile).Length == content.Size;
        }

        public static int CDecrypt(string workingDir)
        {
            try
            {
                var cdecrypt = Path.Combine(workingDir, "CDecrypt.exe");

                foreach (var process in Process.GetProcessesByName("CDecrypt"))
                {
                    var fullpath = Path.GetFullPath(process.MainModule.FileName);
                    if (fullpath == Path.GetFullPath(cdecrypt))
                        process.Kill();
                }

                if (!GZip.Decompress(Resources.CDecrypt_exe_gz, cdecrypt))
                    AppendLog("Error decrypting contents!\r\n       Could not extract CDecrypt.");

                using (TextWriter writer = File.CreateText(Path.Combine(workingDir, "CDecrypt.log")))
                {
                    StartProcess(cdecrypt, "tmd cetk", workingDir, null, true, false, writer).Wait();
                    return 0;
                }
            }
            catch (TaskCanceledException)
            {
                TextLog.MesgLog.WriteError(@"Process Timed Out!");
            }
            catch (Exception ex)
            {
                AppendLog("Error decrypting contents!\r\n" + ex.Message);
            }
            return 1;
        }

        private static async Task<int> StartProcess(string filename, string arguments, string workingDirectory,
            int? timeout = null, bool createNoWindow = true, bool shellEx = false, TextWriter outputTextWriter = null,
            TextWriter errorTextWriter = null)
        {
            using (var process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    CreateNoWindow = createNoWindow,
                    Arguments = arguments,
                    FileName = filename,
                    RedirectStandardOutput = outputTextWriter != null,
                    RedirectStandardError = errorTextWriter != null,
                    UseShellExecute = shellEx,
                    WorkingDirectory = workingDirectory
                }
            })
            {
                process.Start();

                var cancellationTokenSource = timeout.HasValue
                    ? new CancellationTokenSource(timeout.Value)
                    : new CancellationTokenSource();
                
                var tasks = new List<Task>(2) {process.WaitForExitAsync(cancellationTokenSource.Token)};
                if (outputTextWriter != null)
                    tasks.Add(ReadAsync(
                        x =>
                        {
                            process.OutputDataReceived += x;
                            process.BeginOutputReadLine();
                        },
                        x => process.OutputDataReceived -= x,
                        outputTextWriter,
                        cancellationTokenSource.Token));

                if (errorTextWriter != null)
                    tasks.Add(ReadAsync(
                        x =>
                        {
                            process.ErrorDataReceived += x;
                            process.BeginErrorReadLine();
                        },
                        x => process.ErrorDataReceived -= x,
                        errorTextWriter,
                        cancellationTokenSource.Token));

                await Task.WhenAll(tasks);

                var log = TextLog.MesgLog;
                if (process.ExitCode == 0) log.WriteLog($@"[{Path.GetFileName(filename)}] Exited Successfully!");
                else log.WriteError($@"[{Path.GetFileName(filename)}] Exited with Code {process.ExitCode}!");

                return process.ExitCode;
            }
        }

        /// <summary>
        ///     Waits asynchronously for the process to exit.
        /// </summary>
        /// <param name="process">The process to wait for cancellation.</param>
        /// <param name="cancellationToken">
        ///     A cancellation token. If invoked, the task will return
        ///     immediately as canceled.
        /// </param>
        /// <returns>A Task representing waiting for the process to end.</returns>
        private static Task WaitForExitAsync(this Process process,
            CancellationToken cancellationToken = default(CancellationToken))
        {
            process.EnableRaisingEvents = true;

            var taskCompletionSource = new TaskCompletionSource<object>();

            EventHandler handler = null;
            handler = (sender, args) =>
            {
                process.Exited -= handler;
                taskCompletionSource.TrySetResult(null);
            };
            process.Exited += handler;

            if (cancellationToken != default(CancellationToken))
                cancellationToken.Register(
                    () =>
                    {
                        process.Exited -= handler;
                        taskCompletionSource.TrySetCanceled();
                    });

            return taskCompletionSource.Task;
        }

        /// <summary>
        ///     Reads the data from the specified data received event and writes it to the
        ///     <paramref name="textWriter" />.
        /// </summary>
        /// <param name="addHandler">Adds the event handler.</param>
        /// <param name="removeHandler">Removes the event handler.</param>
        /// <param name="textWriter">The text writer.</param>
        /// <param name="cancellationToken">The cancellation token.</param>
        /// <returns>A task representing the asynchronous operation.</returns>
        private static Task ReadAsync(this Action<DataReceivedEventHandler> addHandler,
            Action<DataReceivedEventHandler> removeHandler, TextWriter textWriter,
            CancellationToken cancellationToken = default(CancellationToken))
        {
            var taskCompletionSource = new TaskCompletionSource<object>();

            DataReceivedEventHandler handler = null;
            handler = (sender, e) =>
            {
                if (e.Data == null)
                {
                    removeHandler(handler);
                    taskCompletionSource.TrySetResult(null);
                }
                else
                {
                    textWriter.WriteLine(e.Data);
                    //TextLog.MesgLog.WriteLog(e.Data, Color.DarkSlateBlue);
                }
            };

            addHandler(handler);

            if (cancellationToken != default(CancellationToken))
                cancellationToken.Register(
                    () =>
                    {
                        removeHandler(handler);
                        taskCompletionSource.TrySetCanceled();
                    });

            return taskCompletionSource.Task;
        }
    }
}