
# GCS2.1 controller samples

## General notice

Please refer to the general readme file located here: ..\..\ReadMe.txt.
It describes general topics about how to work with the samples.

## Provided samples for this controller type

### DataRecorderSample
    This sample connects to a controller using the PI Connection Dialog. The
    connection type can be easily changed. Then the data recorder is configured
    and samples of a step are captured. They are written to console and also to
    a file named "test.dat". The stage has to be referenced before starting this
    sample.

### MoveSample
    This sample connects a controller using the connection dialog. It shows how
    to determine min/max position limits, also how to do absolute and relative
    motions. The stage has to be referenced before starting this sample.

### ReferenceSample
    This sample connects to a controller using the connection dialog.
    Then it starts up the connected stage and references it.

### ParameterSample
    This sample connects a controller using the connection dialog. The value of
    the parameter for the maximum velocity is read and the value is modified and
    set as velocity of the axis. The stage has to be referenced before starting
    this sample.

### TwoAxesSample
    This sample connects to a controller with at least 2 axes and demonstrate
    how to use 2 axis. Command a absolute movement with 2 axes with one command
    and afterwards demonstrate a movement with 2 axes sequentially. Both axes
    have to be initialized and referenced.
