using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using LuigiWPF.Model.DAL;
using System.Windows.Input;
using System.ComponentModel;
using System.Collections.ObjectModel;
using LuigiWPF.ViewModel.Commands;

namespace LuigiWPF.Model
{
    public class ViewContextMenu
    {
        public string Name { get; set; }
        public RelayCommand<object> MyContextMenuCommand { get; set; }

    }
}
