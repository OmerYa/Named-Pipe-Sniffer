using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LuigiWPF.Model.DAL
{
    public static class Util
    {
     
        
        public static String BytesToString(long byteCount)
        {
            string[] suffix = { "B", "KB", "MB", "GB", "TB", "PB", "EB" }; //Longs run out around EB
            if (byteCount == 0)
            {
                return "0" + suffix[0];
            }
            int pos = 0;
            long num_count;
            for (num_count = byteCount; num_count > 1024; num_count /= 1024)
            {
                pos++;
            }
            return num_count.ToString() + suffix[pos];
        }


     
    }
}
