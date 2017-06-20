// Project: MapleCake
// File: SeperatorCommandItem.cs
// Updated By: Jared
// 

using System.Windows.Input;
using MapleCake.Models.Interfaces;

namespace MapleCake.Models.ContextMenu
{
    public class SeparatorCommandItem : ICommandItem
    {
        #region ICommandItem Members

        string ICommandItem.Text { get; set; } = string.Empty;

        string ICommandItem.ToolTip { get; set; }

        ICommand ICommandItem.Command { get; set; }

        #endregion
    }
}