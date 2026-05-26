using System.Collections.Generic;

namespace WaveGeneratorSample
{
    public static class WaveGeneratorConfigurations
    {
        public static Dictionary<string, WaveGeneratorSettings> Properties { get; } =
            new Dictionary<string, WaveGeneratorSettings>
            {
                {
                    "Common",
                    new WaveGeneratorSettings(
                        "1", // axis identifier
                        new[] {2} // table rate
                    )
                },
                {
                    "C-887",
                    new WaveGeneratorSettings(
                        "X", // axis identifier
                        new[] {10} // table rate only multiple of 10 are allowed for the Hexapod controller
                    )
                },
            };
    }

    public struct WaveGeneratorSettings
    {
        public readonly string Axis;                          // Axis Identifier
        public readonly int NumberOfAxis;                     // number of axis used
        public readonly int[] WaveGeneratorId;                // waveGeneratorId of wave generator
        public readonly int[] WaveTableId;                    // waveGeneratorId of wave table
        public readonly int[] StartMode;                      // start mode = 1 (start wave generator output synchronized by servo cycle)
        public readonly int[] NumberOfCycles;                 // number of wave generator cycles
        public readonly double[] OffsetOfWave;                  // offset of wave
        public readonly double AmplitudeOfWave;                 // amplitude of wave
        public readonly double Frequency;                   // frequency of wave
        public readonly int[] TableRate;                      // number of servo cycles per point in wave table
        public readonly int[] InterpolationType;              // interpolation between points, used if piTableRate > 1. 1 = linear interpolation
        public readonly int OffsetOfFirstPointInWaveTable;  // index of starting point of curve in segment
        public readonly int AppendWave;                     // 0=clear wave table (1=add wave table values, 2=append to existing wave table contents)

        public WaveGeneratorSettings(
            string axis
            , int[] tableRate
            )
        {
            Axis = axis;
            NumberOfAxis = 1;
            WaveGeneratorId = new[] {1};
            WaveTableId = new[] {1};
            StartMode = new[] {1};
            NumberOfCycles = new[] {5};
            OffsetOfWave = new[] {0.0};
            AmplitudeOfWave = 1;
            Frequency = 2;
            TableRate = tableRate;
            InterpolationType = new[] {1};
            OffsetOfFirstPointInWaveTable = 0;
            AppendWave = 0;
        }
    }

}