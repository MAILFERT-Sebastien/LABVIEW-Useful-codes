#########################################################
#
# PI MATLAB ReferenceSample Version 1.5.0
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
# This sample connects to a controller using TCP/IP, USB or RS232 and references the actuator.
#
# Please note that the referencing process for the E-712 controller can only be performed if
# an actuator is connected in which no absolute sensor is installed. Other actuators don't support the FRF command.
#
#########################################################
#
# Hardware requirements:
#
# - controller
# - compatible actuator
#
#########################################################
#
# Software requirements:
#
# - MATLAB Driver and DLL 
#
#########################################################
#
# Unsupported controllers:
#
# - E-727
# - C-891
#
#########################################################