using System;

namespace ReadCommandSample
{
    class Program
    {
        [STAThread()]
        static void Main()
        {
            ReadCommandSample sample = new ReadCommandSample ();
            sample.Run();
        }
    }
}
