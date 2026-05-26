using System;

namespace ParameterSample
{
    class Program
    {
        [STAThread()]
        private static void Main()
        {
            var sample = new ParameterSample();
            sample.Run();
        }
    }
}
