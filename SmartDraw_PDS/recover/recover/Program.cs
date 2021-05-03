using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace recover
{
    class Program
    {
        static void Main(string[] args)
        {
            if (1 == args.Length)
            {
                DrvFile oDrvFile = new DrvFile();
                if(true == oDrvFile.Read(args[0]))
                {
                    oDrvFile.ToCSV(args[0] + ".csv");
                }
            }
        }
    }
}
