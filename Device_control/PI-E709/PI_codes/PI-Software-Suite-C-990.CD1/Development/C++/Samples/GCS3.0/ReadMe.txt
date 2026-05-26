
# GCS 3.0 controller samples

## General notice

Please refer to the general readme file located here: ..\..\ReadMe.txt.
It describes general topics about how to work with the samples.

## Provided samples for this controller type

### DataRecorderSample
    This sample connects to a controller using a connection dialog. Then the
    data recorder is configured and samples of a step response are captured. The
    recorded data is written to std::cout and also to a file, named "test.dat".
    The positioner has to be referenced before starting this sample.

### MoveSample
    This sample connects to a controller using a connection dialog. It shows how
    to determine min/max position limits, and how to do absolute and relative
    moves. The positioner has to be referenced before starting this sample.

### ParameterSample
    This sample connects a controller using the connection dialog. The value of
    the parameter for the maximum velocity is read and the value is modified and
    set as velocity of the axis. The stage has to be referenced before starting
    this sample.

### ReferenceSample
    This sample connects to a controller using a connection dialog and
    references the positioner.

### TwoAxesSample
    This sample connects to a two-axis controller and references both axes.
    Afterwards, it moves the two axes forward and backward repeatedly.
