
# C-887 controller samples

## General notice

Please refer to the general readme file located here: ..\..\ReadMe.txt.
It describes general topics about how to work with the samples.

## Provided samples for this controller type

### DataRecorderSample
    This sample connects to a C-887 controller using a connection dialog.
    Then the data recorder is configured and samples of a step are captured.
    They are written to std::cout and also to a file, named "test.dat".
    The positioner has to be referenced before starting this sample.

### MoveSample
    This sample connects a C-887 controller using a connection dialog.
    It shows how to determine min/max position limits, and how to do absolute
    and relative moves. The positioner has to be referenced before starting
    this sample.

### ReferenceSample
    This sample connects to a C-887 controller using the connection dialog and
    references the stage. The controller and the stage has to be initialized
    with PIMikroMove before starting this sample.
