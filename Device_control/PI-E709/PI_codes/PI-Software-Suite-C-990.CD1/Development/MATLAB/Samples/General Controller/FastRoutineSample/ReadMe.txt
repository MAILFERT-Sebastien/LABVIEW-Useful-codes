#########################################################
#
# PI MATLAB FastRoutineSample Version 1.5.0
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
#                   MOTION WARNING                      #
#########################################################
#
# Description:
#
# The E-712 systems provide routines for fast alignment of one or more senders and receivers. 
# The objective of the routines is to align each sender and receiver 
# so that the maximum intensity of the emitted signal is measured on the receiver side.
# The controller and the actuator have to be initialized with PIMikroMove (start up axes) before starting this sample.
#
# The following types of fast alignment routines are provided:
# - “FDL Area scan”: Fast routine for first light detection
# - “FDR Area scan”: Spiral or raster scan to find the position of the global intensity maximum of the measured signal
# - “Gradient search”: Circular scan with gradient formation to find the maximum intensity value of the measured signal
# Typically, the end position of an area scan routine is used as the start position for a gradient search routine.
# The maximum number of routines is identical to the number of motion axes that are present in the system (E-712 controller only).
# Multiple gradient search routines can run synchronously for the axes on both the sender and receiver side (E-712 controller only).
#
# Application examples:
#
# Sender and receiver are optical fibers. During the alignment of sender and receiver in axes x, y and z, 
# the power of the optical signal (light) is measured on the receiver side with a power meter. 
# The power meter converts the optical power into an analog signal. Goal of the alignment routines is to align 
# sender and receiver so that the maximum optical power is measured on the receiver side. 
# Multiple couplings (for example, of the input and output of a waveguide device) have to be optimized (Only with E-712 controller).
# In this case, gradient search routines for the couplings can be performed simultaneously, ensuring a global optimization,
# even if the couplings interact. Similarly, for lensed couplings a Z gradient search may also be performed at the same time.
#
#
#########################################################
#
# Hardware requirements:
#
# - controller supporting fast routine (E-712)
# - compatible actuator
# - optical fibres (sender and receiver)
# - light source
# - power meter
#
#########################################################
#
# Software requirements:
#
# - MATLAB Driver and DLL 
#
#########################################################
