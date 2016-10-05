using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Threading;

namespace LuigiWPF.ViewModel
{
    public class BaseViewModel : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        public bool IsWorking { get; set; }
        public void OnPropertyChanged(params string[] props)
        {
            Application.Current.Dispatcher.Invoke(new Action(() =>
            {
                if (props == null && PropertyChanged != null)
                {
                    PropertyChanged(this, new PropertyChangedEventArgs(null));
                    return;
                }

                if (props != null)
                    foreach (var p in props)
                        if (PropertyChanged != null)
                            PropertyChanged(this, new PropertyChangedEventArgs(p));
            }));
        }

        protected void Disp(Action x)
        {
            System.Windows.Application.Current.Dispatcher.Invoke(new Action(() => { x(); OnPropertyChanged(null); }));
        }

        protected void DispAsync(Action x)
        {
            System.Windows.Application.Current.Dispatcher.BeginInvoke(new Action(() => { x(); OnPropertyChanged(null); }));
        }

        public void Working(bool x)
        {
            Disp(() =>
            {
                IsWorking = x;
                OnPropertyChanged("", null);
            });
        }
    }
}
