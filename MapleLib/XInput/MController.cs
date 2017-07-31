using SharpDX.XInput;

namespace MapleLib.XInput
{
    public class MController : Controller
    {
        public MController(UserIndex userIndex) : base(userIndex) {}

        public void Poll(XInputController xInputController)
        {
            var state = GetState();

            state.Gamepad.LeftThumbX = 0;
            state.Gamepad.LeftThumbY = 0;
            state.Gamepad.RightThumbX = 0;
            state.Gamepad.RightThumbY = 0;
            state.Gamepad.LeftTrigger = 0;
            state.Gamepad.RightTrigger = 0;

            if (!xInputController.PreviousState.Gamepad.Equals(state.Gamepad)) {
                if (state.Gamepad.Buttons != 0) {
                    xInputController.DPadButtonPress(state.Gamepad);
                    xInputController.FaceButtonPress(state.Gamepad);
                }
            }

            xInputController.PreviousState = state;
        }
    }
}