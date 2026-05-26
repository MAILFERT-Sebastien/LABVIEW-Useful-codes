using System;

namespace DataRecorderSample
{
    class Program
    {
        [STAThread()]
        static void Main()
        {
            DataRecorderSample sample = new DataRecorderSample ();
            sample.Run();
        }
    }
}
