using System.Collections.Generic;

namespace DataRecorderSample
{
    public static class DataRecorderConfigurations
    {
        public static Dictionary<string, DataRecorderSettings> Properties { get; } =
            new Dictionary<string, DataRecorderSettings>
            {
                {
                    "Common"
                    , new DataRecorderSettings ( "1" )
                },
                {
                    "C-887"
                    , new DataRecorderSettings ( "X" )
                }
            };
    }

    public struct DataRecorderSettings
    {
        public readonly string Axis; // axis identifier
        public readonly int[] TableIds; // record table to be used
        public readonly int[] Option; // record options (1 = commanded position, 2 = measured position)
        public readonly int TableRate; // record table rate (equal to output rate of wave generator)

        public readonly int[]
            TriggerSource; // 0 - default, data recording is triggered with IMP; 1 - any command changing target position or open-loop control value

        public readonly string
            TriggerOption; // trigger option is set to a dummy value, only used for certain trigger sources (see manual for details)

        public readonly int ReadChannels; // number of channels to read
        public int NoOfSamplesToRead; // number of samples to read

        public DataRecorderSettings( string axis )
        {
            Axis = axis;
            TableIds = new[] {1, 2};
            Option = new[] {1, 2};
            TableRate = 1;
            TriggerSource = new[] {1};
            TriggerOption = "0";
            ReadChannels = 2;
            NoOfSamplesToRead = 200;
        }
    }
}
