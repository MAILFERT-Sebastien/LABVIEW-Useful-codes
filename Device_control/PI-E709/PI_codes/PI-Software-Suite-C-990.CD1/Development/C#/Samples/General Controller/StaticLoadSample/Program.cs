using System;

namespace StaticLoadSample
{
    public class Program
    {
        [STAThread()]
        private static void Main()
        {
            var sample = new StaticLoadSample();
            sample.Run();
        }
    }
}
