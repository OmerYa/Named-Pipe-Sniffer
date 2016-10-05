using Microsoft.WindowsAPICodePack.Dialogs;
using LuigiWPF.Model;
using LuigiWPF.ViewModel.Commands;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Threading;
using LuigiWPF.View;
using System.Threading;

namespace LuigiWPF.ViewModel
{
    public class MainViewModel : BaseViewModel
    {
        public  ObservableCollection<PipeData> PipeDataCollection { get; set; }
    
        private static ReadOnlyObservableCollection<PipeData> PipeDataCollectionOriginal;

        public UCHexViewer CurrentViewer { get; private set; }

        private string _SelectedFolder;
        public string SelectedFolder
        {
            get { return _SelectedFolder; }
            set
            {
                // todo: handle
                _SelectedFolder = value;
            }
        }

        private PipeData _SelectedPipe;
        public PipeData SelectedPipe
        {
            get
            {
                return _SelectedPipe;
            }
            set
            {
                _SelectedPipe = value;
                if (_SelectedPipe == null)
                {
                    return;
                }
                Dispatcher.CurrentDispatcher.BeginInvoke(new Action(() =>
                {
                    Disp(() =>
                    {
                        UCHexViewer hx = new UCHexViewer() { DataContext = new UCHexViewerViewModel(_SelectedPipe.PipeDataByteArray) };
                        CurrentViewer = hx;
                        OnPropertyChanged("CurrentViewer");

                    });
                }));
            }
        }

        public ObservableCollection<PipeData> FiltOut(String sToFilter, String sFilterType, ObservableCollection<PipeData> curPipeData)
        {
            ObservableCollection<PipeData> newPipeData = curPipeData;

            if (sFilterType == "PipeName")
            {
                foreach (PipeData curPipeItem in newPipeData) {
                    if (curPipeItem.PipeName==sToFilter)
                    {
                        newPipeData.Remove(curPipeItem);
                    }
                }
            }
            return newPipeData;
        }



        public RelayCommand<object> ExitCommand
        {
            get
            {
                return new RelayCommand<object>(x =>
                {
                    Environment.Exit(0);
                });
            }
        }

        public RelayCommand<object> AboutCommand
        {
            get
            {
                return new RelayCommand<object>(x =>
                {
                    MessageBox.Show("Luigi - written by:\n\tOmer Yair \n\tOr Safran \nSpecial thanks to:\n\tOmer Agmon", "About");
                });
            }
        }
        public RelayCommand<object> ClearFilters
        {
            get
            {
                return new RelayCommand<object>(x =>
                {
                    if (PipeDataCollectionOriginal == null)
                    {
                        return;
                    }
                    Disp(() =>
                    {
                        PipeDataCollection = new ObservableCollection<PipeData>(PipeDataCollectionOriginal);
                        OnPropertyChanged("PipeDataCollection");
                    });
                   
                });
            }
        }




        public RelayCommand<object> OpenFolderCommand
        {
            get
            {
                return new RelayCommand<object>(x =>
                {
                  
                    var dlg = new CommonOpenFileDialog();
                    dlg.InitialDirectory = @"c:\";
                    dlg.Title = "Please select binary files folder";
                    dlg.IsFolderPicker = true;

                    dlg.AddToMostRecentlyUsedList = false;
                    dlg.AllowNonFileSystemItems = false;

                    dlg.EnsureFileExists = true;
                    dlg.EnsurePathExists = true;
                    dlg.EnsureReadOnly = false;
                    dlg.EnsureValidNames = true;
                    dlg.Multiselect = false;
                    dlg.ShowPlacesList = true;

                    if (dlg.ShowDialog() != CommonFileDialogResult.Ok)
                    {
                        return;
                    }

                    SelectedFolder = dlg.FileName;


                    if (!Directory.Exists(SelectedFolder))
                    {
                        MessageBox.Show("Folder " + SelectedFolder + " does not exist", "Error - no such folder", MessageBoxButton.OK
                            , MessageBoxImage.Error, MessageBoxResult.OK);
                        return;
                    }

                    LoadFolderData();
                });
            }
        }


        public static ObservableCollection<ViewContextMenu> MainContextMenu { get; set; }
        public MainViewModel()
        {
            PipeDataCollection = new ObservableCollection<PipeData>();
            MainContextMenu = new ObservableCollection<ViewContextMenu>();
            FillContextMenu();
        }

        public static void FillContextMenu()
        {
            MainContextMenu.Clear();
        }
        private enum FilterType
        {
            File = 0,
            Name = 1,
            PID = 2,
        }
      
        private void RunFile (string FilePath)
        {
            System.Diagnostics.Process proc = new System.Diagnostics.Process();
            proc.StartInfo.FileName = FilePath;
            proc.Start();
        }
        private void AddItem (FilterType Type, bool Include, String Data)
        {
            ViewContextMenu MenuItem = new ViewContextMenu();
            MenuItem.MyContextMenuCommand = new RelayCommand<object>(x =>
            {
                switch (Type)
                {
                    case FilterType.Name:
                        FilterPipeNameData(Include, Data);
                        break;
                    case FilterType.PID:
                        FilterPipePIDData(Include, Data);
                        break;
                    case FilterType.File:
                        RunFile(Data);
                        break;
                    default:
                        break;
                }
            });
            if (Type != FilterType.File) { 
                if (Include)
                    MenuItem.Name = "Include only ";
                else
                    MenuItem.Name = "Exclude ";
            }
            switch (Type)
            {
                case FilterType.Name:
                    MenuItem.Name += "pipe name: " + Data;
                    break;
                case FilterType.PID:
                    MenuItem.Name += "PID: " + Data;
                    break;
                case FilterType.File:
                    MenuItem.Name += "Open raw data file: " + Data;
                    break;
                default:
                    break;
            }
            MainContextMenu.Add(MenuItem);
        }
        public  void FillContextMenu(String Header, String Data, String FileWithFullPath)
        {
            MainContextMenu.Clear();
            AddItem(FilterType.File, false, FileWithFullPath);
            switch (Header)
            {
                case "Process ID":
                    AddItem(FilterType.PID, false, Data);
                    AddItem(FilterType.PID, true, Data);
                    break;
                case "Pipe Name":
                    AddItem(FilterType.Name, false, Data);
                    AddItem(FilterType.Name, true, Data);
                    break;
                default:
                    return;
            }
        }

        private void LoadFolderData()
        {
            Working(true);

            //new Thread(() =>
            //{

                foreach (String file in Directory.GetFiles(SelectedFolder, "MAR*.tmp"))
                {
                    PipeData p = PipeData.ParsePipeData(file);
                    Disp(() =>
                    {
                        PipeDataCollection.Add(p);
                        OnPropertyChanged("PipeDataCollection");
                    });
                }

                Working(false);

           // });
            
            PipeDataCollectionOriginal = new ReadOnlyObservableCollection<PipeData>(PipeDataCollection);
        }

        public  void FilterPipeNameData(bool Include ,string pName)
        { 
            Disp(() =>
            {
                if (Include) { 
                    PipeDataCollection = new ObservableCollection<PipeData>(
                    PipeDataCollection.Where(x => x.PipeName == pName).ToList()
                    );
                 }
                if (!Include)
                {
                    PipeDataCollection = new ObservableCollection<PipeData>(
                    PipeDataCollection.Where(x => x.PipeName != pName).ToList()
                    );
                }
                OnPropertyChanged("PipeDataCollection");
            });
        }
        public  void FilterPipePIDData(bool Include, string PID)
        {
            Disp(() =>
            {
                if (Include)
                {
                    PipeDataCollection = new ObservableCollection<PipeData>(
                    PipeDataCollection.Where(x => x.ProcessId == PID).ToList()
                    );
                }
                if (!Include)
                {
                    PipeDataCollection = new ObservableCollection<PipeData>(
                    PipeDataCollection.Where(x => x.ProcessId != PID).ToList()
                    );
                }
                OnPropertyChanged("PipeDataCollection");
            });
        }
    }


}
