
# General controller samples

## General notice

Please refer to the general readme file located here: ..\..\ReadMe.txt.
It describes general topics about how to work with the samples.

## Provided samples for this controller type

### AutoZeroSample
    This sample connects to a controller (i.e. E-727) via connection dialog
    and which supports auto zero. The controller will then do a auto zero of
    the stage. The controller and the stage has to be initialized with
    PIMikroMove before starting this sample.

### DaisyChainSample
    This sample shows how to use the C#-Wrapper for the PI_GCS2_DLL.dll and its
    methods, connect to a controller using connection dialog and read IDN.

### DataRecorderSample
    This sample connects to a controller using the PI Connection Dialog.
    The connection type can be easily changed. Then the data recorder is
    configured and samples of a step are captured. They are written to console
    and also to a file, names "test.dat". The stage has to be referenced
    before starting this sample.

### MoveSample
    This sample connects a controller using the connection dialog. It shows how
    to determine min/max position limits, also how to do absolute and relative
    motions. The stage has to be referenced before starting this sample.

### ReferenceSample
    This sample connects to a controller using the connection dialog and
    references the stage. The controller and the stage has to be initialized
    with PIMikroMove before starting this sample.

### ReferenceWithEAXSample
    This sample connects to a controller which supports the command EAX (i.e.
    C-891) using the connection dialog and references the stage. The controller
    and the stage has to be initialized with PIMikroMove before starting this
    sample.

### StaticLoadSample
    This sample shows how to use the C#-Wrapper for the PI_GCS2_DLL.dll and its
    methods, connect to a controller using connection dialog and read IDN.

### TrajectorySample
    This sample connects to a controller using the PI Connection Dialog. After
    that, a trajectory is setup, transferred and started. The stage has to be
    initialized and referenced before starting this sample.

### TwoAxesSample
    This sample connects to a controller with at least 2 axes and demonstrate
    how to use 2 axis. Command a absolute movement with 2 axes with one command
    and afterwards demonstrate a movement with 2 axes sequentially. Both axes
    have to be initialized and referenced.

### WaveGeneratorSample
    This sample connects to a controller using the PI Connection Dialog. The
    connection type can be easily changed. Then the wave generator is configured
    and started. The stage has to be initialized and referenced before starting
    this sample.
