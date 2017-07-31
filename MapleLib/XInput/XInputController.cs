using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using Dynamitey;
using MapleLib.Collections;
using MapleLib.Common;
using MapleLib.Structs;
using SharpDX.XInput;

namespace MapleLib.XInput
{
    public class XInputController
    {
        private readonly MController[] _controllers =
        {
            new MController(UserIndex.One),
            new MController(UserIndex.Two),
            new MController(UserIndex.Three),
            new MController(UserIndex.Four)
        };

        private Title SelectedEntry {
            get { return Config?.SelectedItem; }
            set { Dynamic.InvokeSet(Config, "SelectedItem", value); }
        }

        private Task LibraryWorker { get; set; }

        private Task PollWorker { get; set; }

        private static GamepadButtonFlags LaunchButton => GamepadButtonFlags.A;

        private MapleDictionary LibraryEntries { get; set; }

        private dynamic Config { get; set; }

        internal State PreviousState { get; set; }

        public void Start(dynamic config)
        {
            Config = config;

            LibraryWorker = Task.Run(async () => {
                while (!Process.GetCurrentProcess().HasExited) {
                    LibraryEntries = Database.GetLibrary();
                    await Task.Delay(1000 * 30);
                }
            });

            PollWorker = Task.Run(async () => {
                while (!Process.GetCurrentProcess().HasExited) {
                    var controller = (from selectControler in _controllers
                        where selectControler.IsConnected
                        select selectControler).FirstOrDefault();

                    controller?.Poll(this);
                    await Task.Delay(1);
                }
            });
        }

        private static bool _cemuRunning()
        {
            if (Settings.CemuDirectory.IsNullOrEmpty()) return false;

            var cemuPath = Path.Combine(Settings.CemuDirectory, "cemu.exe");
            var fileName = Path.GetFileName(cemuPath).Replace(".exe", "");
            return Process.GetProcessesByName(fileName).Length > 0;
        }

        internal void DPadButtonPress(Gamepad gamePad)
        {
            if (gamePad.Buttons == 0) return;
            if (LibraryEntries == null) return;

            var idx = LibraryEntries.IndexOf(SelectedEntry);

            if (gamePad.Buttons.HasFlag(GamepadButtonFlags.DPadDown))
                if (idx < LibraryEntries.Count - 1)
                    SelectedEntry = LibraryEntries[idx + 1];

            if (gamePad.Buttons.HasFlag(GamepadButtonFlags.DPadUp))
                if (idx > 0)
                    SelectedEntry = LibraryEntries[idx - 1];

            Config?.RaisePropertyChangedEvent("SelectedItem");
        }

        internal void FaceButtonPress(Gamepad gamePad)
        {
            if (gamePad.Buttons == 0) return;

            if (gamePad.Buttons == LaunchButton && !_cemuRunning())
                SelectedEntry.PlayTitle();

            if (!gamePad.Buttons.HasFlag(GamepadButtonFlags.Start) ||
                !gamePad.Buttons.HasFlag(GamepadButtonFlags.Back) ||
                !_cemuRunning()) return;

            var cemuPath = Path.Combine(Settings.CemuDirectory, "cemu.exe");
            var fileName = Path.GetFileName(cemuPath).Replace(".exe", "");
            foreach (var cemuProcess in Process.GetProcessesByName(fileName)) cemuProcess.Kill();
        }
    }
}