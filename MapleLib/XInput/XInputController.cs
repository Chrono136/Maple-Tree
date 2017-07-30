using System.Diagnostics;
using System.IO;
using System.Threading.Tasks;
using Dynamitey;
using MapleLib.Common;
using MapleLib.Structs;
using SharpDX.XInput;

namespace MapleLib.XInput
{
    public class XInputController
    {
        private static GamepadButtonFlags LaunchButton => GamepadButtonFlags.A;

        private dynamic Config { get; set; }

        private Title SelectedEntry {
            get { return Config?.SelectedItem; }
            set { Dynamic.InvokeSet(Config, "SelectedItem", value); }
        }

        public void Start(dynamic config)
        {
            Config = config;

            Task.Run(async () => {
                var controllers = new[]
                {
                    new Controller(UserIndex.One),
                    new Controller(UserIndex.Two),
                    new Controller(UserIndex.Three),
                    new Controller(UserIndex.Four)
                };

                Controller controller = null;
                foreach (var selectControler in controllers)
                    if (selectControler.IsConnected) {
                        controller = selectControler;
                        break;
                    }

                if (controller != null) {
                    var previousState = controller.GetState();
                    while (!Process.GetCurrentProcess().HasExited) {
                        var state = controller.GetState();
                        state.Gamepad.LeftThumbX = 0;
                        state.Gamepad.LeftThumbY = 0;
                        state.Gamepad.RightThumbX = 0;
                        state.Gamepad.RightThumbY = 0;
                        state.Gamepad.LeftTrigger = 0;
                        state.Gamepad.RightTrigger = 0;

                        if (!previousState.Gamepad.Equals(state.Gamepad))
                            if (state.Gamepad.Buttons != 0) {
                                DPadButtonPress(state.Gamepad);
                                FaceButtonPress(state.Gamepad);
                            }

                        await Task.Delay(1);
                        previousState = state;
                    }
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

        private void DPadButtonPress(Gamepad gamePad)
        {
            if (gamePad.Buttons == 0) return;

            var entries = Database.GetLibrary();
            if (entries == null) return;

            var idx = entries.IndexOf(SelectedEntry);

            if (gamePad.Buttons.HasFlag(GamepadButtonFlags.DPadDown))
                if (idx < entries.Count - 1)
                    SelectedEntry = entries[idx + 1];

            if (gamePad.Buttons.HasFlag(GamepadButtonFlags.DPadUp))
                if (idx > 0)
                    SelectedEntry = entries[idx - 1];

            Config?.RaisePropertyChangedEvent("SelectedItem");
        }

        private void FaceButtonPress(Gamepad gamePad)
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