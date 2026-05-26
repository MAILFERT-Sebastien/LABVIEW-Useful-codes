namespace DataRecorderSample
{

    public readonly struct Parameter
    {
        internal string ContainerUnitId { get; }
        internal string FunctionUnitId { get; }
        internal string ParameterId { get; }

        public Parameter (string containerUnitId, string functionUnitId, string parameterId)
        {
            ContainerUnitId = containerUnitId; // Format <classificationId>_<instanceId>, e.g. "AXIS_1" or "CNTR_1"
            FunctionUnitId  = functionUnitId;
            ParameterId     = parameterId;
        }
    }

    public struct TriggerSettings
    {
        internal string Source;  // trigger source
        internal string Option1; // first trigger option, only used for certain trigger sources (see manual for details)
        internal string Option2; // second trigger option, only used for certain trigger sources (see manual for details)

        public TriggerSettings (string source, string option1, string option2)
        {
            Source  = source;
            Option1 = option1;
            Option2 = option2;
        }
    };

    public struct DataRecorderSettings
    {
        public const string TargetPositionParameterId = "0x102";
        public const string ActualPositionParameterId = "0x103";

        public readonly string Axis;          // axisAlias identifier, e.g. "AXIS_1"
        public readonly string RecorderId; // data recorder (REC_1 .. REC_n, n=count od data recorder)
        public readonly int[] TraceIds;       // traceId of a data recorder
        public readonly Parameter[] Parameters;
        public readonly int RecordRate;       // record table rate (equal to output rate of wave generator)
        public readonly TriggerSettings Trigger;
        public readonly string RecorderFormat;
        public int NoOfSamplesToRead;  // number of samples to read

        public DataRecorderSettings ( string axis )
        {
            Axis       = axis;
            RecorderId = "REC_1";        // recorder id to be used
            TraceIds   = new[] {1, 2};   // record traces to be used
            Parameters = new[]           // parameters to be recorded
                {
                    new Parameter (axis, "-", ActualPositionParameterId),
                    new Parameter (axis, "-", TargetPositionParameterId)

                };
            RecordRate        = 2;       // record table rate (equal to output rate of wave generator)
            Trigger           = new TriggerSettings ("MOV", axis, "");
            RecorderFormat    = "ASCII"; // format of data to be read
            NoOfSamplesToRead = 200;     // number of samples to read
        }

    }
}
