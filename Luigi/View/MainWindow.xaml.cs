using System;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace LuigiWPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {

        }
        private void MouseRightButtonEvent(object sender, MouseButtonEventArgs e)
        {
            
            ViewModel.MainViewModel.FillContextMenu();
            try {
                var ColName = (((e.OriginalSource as TextBlock).Parent as DataGridCell)).Column.Header.ToString();
                var RawData = (e.OriginalSource as TextBlock).Text;
                var FileName = ((e.OriginalSource as TextBlock).DataContext as Model.PipeData).PipeFileNameWithFullPath;
                (DataContext as ViewModel.MainViewModel).FillContextMenu(ColName, RawData, FileName);
                DGMainContextMenu.Visibility = Visibility.Visible;
            } 
            catch
            {
                DGMainContextMenu.Visibility = Visibility.Hidden;
                ViewModel.MainViewModel.FillContextMenu();
                
            }
        }
    }
}
