#########################################################
#
# PI Python samples version 1.0.0
#
#
#########################################################
#                  IMPORTANT NOTICE                     #
#########################################################
#
# Please initialize your controller and your positioner
# before running any sample.
# For the initialization use PIMikroMove on Windows and
# PITerminal on Linux. See corresponding manual for details.
#
#########################################################
#                 HOW TO USE THE SAMPLES                #
#########################################################

There are two sources for the python samples. Typical samples can be found in the samples folder in the PIPython.zip file. There you find samples for different features provided by PI controllers.
Please note that there are prerequisites for using some of the samples.
Also there are folders for specific controllers. They contain samples for controller-specific features or for features which have to be used differently for this controller i.e. E-872, E-135.

1. Install:
        PIPython
        > Unzip the file PIPython.zip
        > Open a command entry (Linux Console or Windows CMD window) and run:
            python setup.py install

        PISoftwareSuite
        > Call PISoftwareSuite.exe from your product CD. This installs all important PI software to your PC. This is recommended.
        > If you do not want to perform the complete installation, you must run the three installers on the product CD listed below as a minimum:
            PIBaseComponents.exe
            PIAPIComponents.exe
            PIUSBDriversWin32/64.exe

2. Run:
        > For further information open the *index.html* file in your browser and see the samples in the general samples in the
        > PIPython *samples* installation folder in your python distribution. Or see the samples provided in this sample folder.