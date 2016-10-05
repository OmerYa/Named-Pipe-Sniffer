using LuigiWPF.ViewModel.Commands;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;


namespace LuigiWPF.ViewModel
{
    public class UCHexViewerViewModel : BaseViewModel
    {
        public class HexViewData
        {
            public string RowNumbers { get; set; }
            public string HexData { get; set; }
            public string StringData { get; set; }

        }


        public List<HexViewData> Data { get; set; }

        public  UCHexViewerViewModel(byte[] BinaryData)
        {

            Working(true);

            List<HexViewData> tmp = new List<HexViewData>();

            new Thread(() =>
             {
                 Data = new List<HexViewData>();
                 List<byte> bd = new List<byte>(BinaryData);
                 
                 for (int i = 0; i < BinaryData.Length; i += 16)
                 {
                     var SingleData = new HexViewData();
                     SingleData.RowNumbers = (i).ToString("X8");
                     int readLength = BinaryData.Length - 16 < i ? BinaryData.Length - i : 16;
                     SingleData.HexData = (ByteArrayToString(bd.GetRange(i, readLength).ToArray()));
                     var tmpData = Encoding.ASCII.GetString(BinaryData, i, readLength);
                     SingleData.StringData = Regex.Replace(tmpData, @"[^\u0020-\u007F]", ".");
                     tmp.Add(SingleData);
                 }

                 DispAsync(() => { Data = tmp;
                     OnPropertyChanged("", null); });

                 Working(false);
             })
            { IsBackground = true }.Start();
           
           


        }
        
        public static string ByteArrayToString(byte[] ba)
        {
            StringBuilder hex = new StringBuilder(ba.Length * 2);
            foreach (byte b in ba)
                hex.AppendFormat("{0:x2} ", b);
            return hex.ToString();
        }


    

    }
}
