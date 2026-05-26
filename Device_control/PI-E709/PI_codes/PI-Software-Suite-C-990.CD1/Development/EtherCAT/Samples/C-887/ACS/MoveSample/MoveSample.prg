#/ Controller version = 2.50
#/ Date = 11/23/2018 7:06 PM
#/ User remarks =
#0
AUTOEXEC: 
int TotalNodes 
TotalNodes=ecgetslaves() 
PI_nodeNumber=0 

while PI_nodeNumber<TotalNodes 
	if( ecgetvid(PI_nodeNumber) = 0x0000076D & ecgetpid(PI_nodeNumber) = 0x102F0000)
		!FOUND! 
		GOTO S1 
	end 
	PI_nodeNumber = PI_nodeNumber+1 

end 

S1: 
ecout(ecgetoffset("RxPDO.Modes of operation",PI_nodeNumber,0,0),ModeOfOperation)
ecin(ecgetoffset("TxPDO.Modes of operation display",PI_nodeNumber,1,0),ModeOfOperationDisplay)
ecin(ecgetoffset("TxPDO.Statusword",PI_nodeNumber,1,0),Statusword)


STOP

#1
!(c)2015-2020 Physik Instrumente (PI) SE & Co. KG
!Software products that are provided by PI are subject to the General Software License Agreement of 
!Physik Instrumente (PI) SE & Co. KG and may incorporate and/or make use of third-party software components. 
!For more information, please read the General Software License Agreement and the Third Party Software Note linked below.

!General Software License Agreement :
!http://www.physikinstrumente.com/download/EULA_PhysikInstrumenteGmbH_Co_KG.pdf
!Third Party Software Note :
!http://www.physikinstrumente.com/download/TPSWNote_PhysikInstrumenteGmbH_Co_KG.pdf

!This sample was created with SPiiPlus MMI Application Studio version 2.50

!In this example, the axis unit is mm.

!!! MOTION WARNING !!
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
!Running this example will result in axis movement!																		!
!Please make sure that the axis can move freely along the travel range and that the motion does not violate				!
!neither software- nor hardware limits. Change axis number and target positions according to your system if necessary  !
!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

!Make sure CTIME is set to 1 already during startup

!Set dynamic parameters according to hexapod 
int i
i = 0
loop 6
	ACC(i) = 100	
	DEC(i) = 100
	VEL(i) = 10
	KDEC(i) = 100
	JERK(i) = 1000
	i = i+1
end

!Read and set encoder factor
int PositionFactorNumerator
int PositionFactorDenominator
real EncoderFactor

PositionFactorNumerator=coeread/4 (PI_nodeNumber,0x6893,1)
PositionFactorDenominator=coeread/4 (PI_nodeNumber,0x6893,2)
EncoderFactor = PositionFactorNumerator / PositionFactorDenominator

i = 0
loop 6
	disable(i)
	EFAC(i) =  EncoderFactor  
	i = i + 1
end

wait 100

i = 0
loop 6
	enable(i)
	i = i + 1
end

!Homing
ModeOfOperation = 6 
TILL ModeOfOperationDisplay = 6
wait 100
TILL Statusword = 5175

!Set hexapod absolute position
i = 0
loop 6
	ptp/e i,FPOS(i)
	i = i + 1
end

!Maxtravel in X-direction
Coewrite/4 (PI_nodeNumber,0x5011,1,1)

!Handshake
Coewrite/4 (PI_nodeNumber,0x5010,3,1)
wait 100

int axis
int MaxPosition
real TargetPosition
axis = 0

MaxPosition = coeread/4 (PI_nodeNumber,0x5012,1)
TargetPosition = 0.8 * MaxPosition/1000

ModeOfOperation = 8 
TILL ModeOfOperationDisplay = 8

PTP/e axis, TargetPosition
PTP/e axis, 0

!Segmented motion

INT iXaxis, iYaxis

iXaxis = 0
iYaxis = 1
PTP/e iXaxis, 5
PTP/e iYaxis, 5

MSEG (iXaxis, iYaxis), 5, 5           ! Start the segmented motion from (5,5)
ARC1 (iXaxis, iYaxis), 5, 0, 5, -5, - ! Arc with center (5,0), final point (5,-5), clockwise rotation
LINE (iXaxis, iYaxis), -5, -5         ! Make line with final point of (-5,-5)
ARC2 (iXaxis, iYaxis), -5, 0, -3.14159! Make arc with center of (-5,0), rotation angle of 3.14159 (clockwise rotation)
LINE (iXaxis, iYaxis), 5, 5           ! Make line with final point of (5,5)
ENDS (iXaxis, iYaxis)                 ! End the segmented motion

stop

#A
!axisdef X=0,Y=1,Z=2,T=3,A=4,B=5,C=6,D=7
!axisdef x=0,y=1,z=2,t=3,a=4,b=5,c=6,d=7
global int I(100),I0,I1,I2,I3,I4,I5,I6,I7,I8,I9,I90,I91,I92,I93,I94,I95,I96,I97,I98,I99
global real V(100),V0,V1,V2,V3,V4,V5,V6,V7,V8,V9,V90,V91,V92,V93,V94,V95,V96,V97,V98,V99
global int ModeOfOperation, Statusword, ModeOfOperationDisplay, PI_nodeNumber
STOP

