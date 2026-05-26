using System;

namespace WaveGeneratorSample
{
    public class Program
    {
        [STAThread]
        private static void Main()
        {
            var sample = new WaveGeneratorSample();
            sample.Run();
        }
    }
}
