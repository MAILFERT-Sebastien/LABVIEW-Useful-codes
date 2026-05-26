using System;


namespace ReferenceSample
{
    public class Program
    {
        [STAThread()]
        private static void Main()
        {
            var sample = new ReferenceSample();
            sample.Run();
        }
    }
}
