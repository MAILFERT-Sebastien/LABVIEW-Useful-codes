
# PISoftwareSuite C++ and C# samples

## Motion warning

Most samples will cause motion. Moving parts of the stage can cause crushing
or shearing injuries. All personnel must remain clear of any moving parts.

## Important notice

Please initialize your controller and your positioner before running any sample.
For the initialization use PIMikroMove on Windows and PITerminal on Linux.
See corresponding manual for details.

## Hard- and software requirements

Unless otherwise mentioned in the readme of the corresponding sample
you need a controller that supports the corresponding feature and a compatible
positioner. Please refer to your controller manual for further details.
Concerning software you need linked libaries, DLLs and headers from the
corresponding "API" folder.

## How to use the samples

The samples are provided in different folders. Typical samples can be found in
the folder "General Controller". There you find samples for different features
PI controllers provide.

Please note that there are prerequisites for using some of the samples.
Also there are folders for specific controllers. They contain samples for
controller-specific features or for features which have to be used differently
for this controller.

1. Install the samples

   Run PISoftwareSuite.exe from your installation media (product CD, USB flash
   drive or mounted ISO file image). This installs all required PI software
   to your PC.

   You can find the samples by opening the Windows start menu
   Physik Instrumente (PI) \ Development. This opens the folder with all samples
   in the Windows file explorer. (Typically "C:\Program Files (x86)\
   Physik Instrumente (PI)\Software Suite\Development")

   Depending of the used programming language copy the corresponding folder
   (e.g. C#) to an appropriate location on your hard disk. Note to choose
   a location with write permission.

2. Open a Visual Studio solution

   Depending on the used controller open a corresponding Visual Studio solution
   (e.g. "General Controller\Samples.sln"). Build "StaticLoadSample" or
   "DynamicLoadSample" to see if all dependencies are set correctly. This is
   recommended but not required.

3. Connect to the controller

   When running any sample, make sure that you have selected the connection
   type you want to use. Available connection settings are RS232, USB, TCP/IP.
   By default the InterfaceSetupDialog is used when a connection is established
   and the dialog is available.

4. Reference the stage

   Depending on the controller you use, specific samples have to be executed to
   have the correct controller state. Check which sample is available in your
   <ControllerType> folder and run it. Corresponding samples are typically
   "AutoZeroSample" or "ReferenceSample". This is required to set the zero
   position of the axis or referencing the stage.

5. Evaluate the samples

   Now any samples can be performed. Please note that the samples from the
   folder "General Controller" are valid for many controllers but not for all.
   If your sample is available in your <ControllerType> folder and
   "General Controller" folder, 	please select the sample from your
   <ControllerType> folder.
