using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using LuigiWPF.Model.DAL;

namespace LuigiWPF.Model
{
    public class PipeData
    {
        private int iCommunicationId;
        public string CommunicationId { get { return iCommunicationId.ToString(); } }

        public uint iProcessId { get; set; }
        public string ProcessId { get { return iProcessId.ToString(); } }

        public ulong PerformanceCounter { get; set; }

        public string PipeName { get; set; }


        public byte[] PipeDataByteArray { get; set; }

        public long Size { get; set; }
        public string SizeHumanReadable
        {
            get
            {
                return Util.BytesToString(Size);

            }
        }

        private static int uiPipeCommunicationsIndex;
        public string PipeFileNameWithFullPath { get; set; }

        public static PipeData ParsePipeData(String sFilePath)
        {
            FileStream fs = new FileStream(sFilePath, FileMode.Open, FileAccess.Read);
            using (BinaryReader br = new BinaryReader(fs))
            {
                long numBytes = new FileInfo(sFilePath).Length;

                string sPipeFileNameWithFullPath = sFilePath;

                uint uiProcessId = br.ReadUInt32();
                UInt64 uiPerformanceCounter = br.ReadUInt64();
                byte[] abFileName = new byte[1024];

                StringBuilder sb = new StringBuilder();
                int count = 0;
                do
                {
                    abFileName[count] = br.ReadByte();
                    abFileName[count + 1] = br.ReadByte();
                    if ((abFileName[count] == 0) && (abFileName[count + 1] == 0))
                        break;
                    count += 2;
                } while (count < 1024);

                byte[] abPipeData = br.ReadBytes((int)numBytes - 12 - count);

                String sPipeName = System.Text.Encoding.Unicode.GetString(abFileName, 0, count);

                PipeData p = new PipeData
                {
                    iCommunicationId = uiPipeCommunicationsIndex++,
                    iProcessId = uiProcessId,
                    PerformanceCounter = uiPerformanceCounter,
                    PipeName = sPipeName,
                    PipeFileNameWithFullPath = sPipeFileNameWithFullPath,
                    PipeDataByteArray = abPipeData,
                    Size = numBytes,
                };
                return p;
            }
        }
       
    }
}
