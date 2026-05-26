using System;

namespace MoveSample
{
    public class Program
    {
        [STAThread()]
        private static void Main()
        {
            var sample = new MoveSample();
            sample.Run();
        }
    }
}
