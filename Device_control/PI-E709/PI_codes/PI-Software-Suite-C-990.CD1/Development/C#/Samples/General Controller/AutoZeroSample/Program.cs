using System;

namespace AutoZeroSample
{
    public class Program
    {
        [STAThread()]
        private static void Main()
        {
            var sample = new AutoZeroSample();
            sample.Run();
        }
    }
}
