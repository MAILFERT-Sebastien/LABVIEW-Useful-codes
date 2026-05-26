
# General controller samples

## General notice

Please refer to the general readme file located here: ..\..\ReadMe.txt.
It describes general topics about how to work with the samples.

## Provided samples for this controller type

### DataRecorderSample
    This sample connects to a controller using a connection dialog.
    Then the data recorder is configured and samples of a step response are
    captured. The recorded data is written to std::cout and also to a file,
    named "test.dat". The positioner has to be referenced before starting this
    sample. Please note that C-891 and E-872 are not supported.

### DynamicLoadSample
    This sample shows how to load the PI_GCS2_DLL and its methods dynamically,
    connect to a controller using a connection dialog and read the device
    identification.

### FastAlignmentSample
    Demonstrates various features how to use fast alignment.

### MoveSample
    This sample connects to a controller using a connection dialog.
    It shows how to determine min/max position limits, and how to do absolute
    and relative moves. The positioner has to be referenced before starting
    this sample. Please note that C-887 and E-872 are not supported.

### ReferenceSample
    This sample connects to a controller using a connection dialog and
    references the positioner. The controller and positioner have to be
    initialized with PIMikroMove before starting this sample.
    Please note that C-891, E-712, E-727 and E-872 are not supported.

### StaticLoadSample
    This sample shows how to load the PI_GCS2_DLL and its methods statically,
    connect to a controller using a connection dialog and read the device
    identification.

### TrajectorySample
    This sample connects to a controller using a connection dialog. Then a
    trajectory is set up, transferred to the controller and started. The
    positioner has to be intialized and referenced before starting this sample.
    The used controller requires supporting trajectories for motion patterns,
    e.g. C-867.2U2. Please note that E-872 is not supported.

### TwoAxesSample
    This sample connects to a two-axis controller and references both axes.
    Afterwards, it moves the two axes forward and backward repeatedly.
    Requirements are a two axis controller and two positioners or two axis
    positioner. Please note that E-872 is not supported.

### WaveGeneratorSample
    This sample connects to a controller using a connection dialog. Then the
    wave generator is configured and started. The positioner has to be
    referenced before starting this sample.
    Requirements are a controller with wave generator support, e.g. C-891
    For C-887 and E-727 use specific configuration defined in header file.
