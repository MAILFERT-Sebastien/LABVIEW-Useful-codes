#########################################################
#
# PI MATLAB AutoZeroSample Version 1.5.0
#
# Creation timestamp: 2020-01-20 13:00
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
#                   MOTION WARNING                      #
#########################################################
#
# Description:
#
# This sample connects to a controller which supports auto zero (e.g. E-727) with a positioner connected
# and runs an auto zero procedure to set the zero position of the axis.
#
#########################################################
#
# Hardware requirements:
#
# - controller which supports auto zero (e.g. E-727)
# - compatible actuator
#
#########################################################
#
# Software requirements:
#
# - MATLAB Driver and DLL 
#
#########################################################