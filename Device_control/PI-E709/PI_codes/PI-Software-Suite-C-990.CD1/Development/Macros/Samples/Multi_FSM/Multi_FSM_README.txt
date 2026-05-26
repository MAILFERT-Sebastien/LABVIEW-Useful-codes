The macro "Multi_FSM" can be started as host- or controllermacro. When you use the QEmu-Simulation instead of the real hexapod hardware, you can start the "Multi-FSM" macro only as hostmacro with PIMicromove(R).
Before you start the "Multi-FSM" macro you MUST replace the startpositions of the MOV command and the scanareas of the FSM command with the startposition and scanareas of your alignment.
!!!!!! CAUTION: OTHERWISE YOU CAN DESTROY YOUR ALIGNMENT !!!!!!

Macro description:
 1. reference the hexapod
 2. wait until the referencing is done
 3. move to the startposition
 4. wait until the motion of the Y-axis is finished
 5. wait until the motion of the Y-axis is finished
 6. wait until the motion of the Z-axis is finished
 7. area scan command FSM with a default scanarea
 8. wait until the scann is finished
 9. if the scan was not successfull, jump to point 13 (FSM with a greater scanarea), otherwise:
10. area scan command FSM with a smaller scanarea and a smaller stepsize
11. wait until the scan is finished
12. if the scan was successfull, stop the macro here, otherwise:
13. area scan command FSM with a greater scanarea than the default scanarea
14. wait until the scan is done
15. if the scan was successfull, then jump to point 7 (FSM with default scanarea), otherwise stop here