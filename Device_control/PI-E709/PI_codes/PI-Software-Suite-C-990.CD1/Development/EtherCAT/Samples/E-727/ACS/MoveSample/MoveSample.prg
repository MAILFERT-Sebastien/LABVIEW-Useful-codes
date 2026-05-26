#/ Controller version = 2.50
#/ Date = 3/15/2018 2:25 PM
#/ User remarks = 
#0
!(c)2015-2020 Physik Instrumente (PI) SE & Co. KG
!Software products that are provided by PI are subject to the General Software License Agreement of 
!Physik Instrumente (PI) SE & Co. KG and may incorporate and/or make use of third-party software components. 
!For more information, please read the General Software License Agreement and the Third Party Software Note linked below.

!General Software License Agreement :
!http://www.physikinstrumente.com/download/EULA_PhysikInstrumenteGmbH_Co_KG.pdf
!Third Party Software Note :
!http://www.physikinstrumente.com/download/TPSWNote_PhysikInstrumenteGmbH_Co_KG.pdf

!This sample was created with SPiiPlus MMI Application Studio version 2.50

!It is assumed that there is at least one axis in the system (default: 0) which is already fully configured, commutated
!(if applicable) and can be moved freely.

!In this example, the axis unit is um.

!!! MOTION WARNING !!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!Running this example will result in axis movement!																		!
!Please make sure that the axis can move freely along the travel range and that the motion does not violate				!
!neither software- nor hardware limits. Change axis number and target positions according to your system, if necessary!!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

!Make sure CTIME is set to 1 already during startup

!Define axis to be moved
int axis
axis = 0

!Define EtherCAT slave node number for PDO mapping
int EtherCATSlaveNode
EtherCATSlaveNode = 0

!Global variables MoO, MoOD defined in D-Buffer
!Map PDO "Modes of operation" of selected EtherCAT slave node for selected axis to global variable MoO
ecout(ecgetoffset("Modes of operation",EtherCATSlaveNode,,axis),MoO)

!Map PDO "Modes of operation display" of selected EtherCAT slave node for selected axis to global variable MoOD
ecin(ecgetoffset("Modes of operation display",EtherCATSlaveNode,,axis),MoOD) 

!Set "Modes of operation" to 8 (Cyclic Synchronous Position Mode)
MoO = 8

!Wait till "Modes of operation display" of selected EtherCAT slave node for selected axis has changed to 8
TILL MoOD = 8

!Check if axis is enabled. If not, enable axis
IF MST(axis).#ENABLED = 0
	ENABLE(axis)
END

!Move axis 0 to absolute position 0 um
PTP/e axis, 0

!Move back and forth in loop
WHILE 1

	!Move axis 0 to absolute position 1 um
	PTP/e axis, 1

	WAIT 100

	!Move axis 0 back to position 0 um by moving -1 um relative
	PTP/er axis, -1

	WAIT 150

END

STOP


#A
!Definition of global variables for PDO Mapping
global int MoO, MoOD

