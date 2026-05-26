#########################################################
#
# PI C-887 EtherCAT TwinCAT sample version 1.0.0
#
# Creation timestamp: 2020-01-16 10:31
#
#########################################################
#                   MOTION WARNING                      #
#########################################################
#
# Description:
#
This sample connects to a C-887 controller via a TwinCAT master and references the hexapod.
It then executes the G-code sequence from TestIt.nc. 

#
# Software requirements:
#
The file TestIt.nc should be copied to C:\TwinCAT\Mc\Nci\ or the path in variable g_strProgram (Global Variables) should be modified.