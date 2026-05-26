#########################################################
#
# PI E-816 MATLAB ConnectToE816Sample Version 1.5.0
#
# Creation timestamp: 2020-01-17 16:00
#
#########################################################
#                  IMPORTANT NOTICE                     #
#########################################################
#
# This sample only works with a PI_MATLAB_Driver version 1.5.0 and newer.
# 
# You must always initialize the system first. The steps necessary depend on the type of mechanics (e.g., positioner, actuator)
# and electronics (e.g., controller, drive electronics). 
# It may be necessary to load positioner parameters from a database and/or perform a homing procedure (e.g., auto zero procedure, reference move). 
# Refer to the manuals of your mechanics and electronics for details.
#
# For the initialization use PIMikroMove on Windows or PITerminal on Linux.
#
# - Load positioner parameters via "Start Up Controller" in PIMikroMove 
# or via the CST command in PITerminal. 
#
# - Execute the homing procedure in PIMikroMove via "Start Up Controller" -> "Start up axes".
# Alternatively, use the corresponding sample (AutoZeroSample, ReferenceSample, ReferenceWithEAXSample). 
#
# To check if the PI_MATLAB_Driver works properly, run the sample 
# BeginWithThisSampleForTestingMatlabDriver.m
#
#########################################################
#
# Description:
#
# This sample demonstrates
# - Loading PI MATLAB Driver GCS2
# - Connecting to the E-816 controller
# - Retrieving controller name
# - Closing the connection to the controller
#
#########################################################
#
# Hardware requirements:
#
# - E-816
# - compatible actuator
#
#########################################################
#
# Software requirements:
#
# - MATLAB Driver and DLL 
#
#########################################################