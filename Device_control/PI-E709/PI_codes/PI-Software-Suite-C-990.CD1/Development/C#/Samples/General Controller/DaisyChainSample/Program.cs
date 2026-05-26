using System;

namespace DaisyChainSample
{
    public class Program
    {
        [STAThread()]
        private static void Main()
        {
            var sample = new DaisyChainSample();
            sample.Run();
        }
    }
}
