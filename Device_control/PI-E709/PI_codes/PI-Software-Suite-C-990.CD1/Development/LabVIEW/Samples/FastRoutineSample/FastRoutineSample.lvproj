<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="20008000">
	<Item Name="My Computer" Type="My Computer">
		<Property Name="NI.SortType" Type="Int">3</Property>
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="Controls" Type="Folder" URL="../Controls">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="FastRoutineSupportVIs" Type="Folder" URL="../FastRoutineSupportVIs">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="FastRoutineSample.vi" Type="VI" URL="../FastRoutineSample.vi"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="whitespace.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/whitespace.ctl"/>
				<Item Name="Trim Whitespace.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Trim Whitespace.vi"/>
				<Item Name="DialogType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogType.ctl"/>
				<Item Name="DialogTypeEnum.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogTypeEnum.ctl"/>
				<Item Name="GetHelpDir.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/GetHelpDir.vi"/>
				<Item Name="BuildHelpPath.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/BuildHelpPath.vi"/>
				<Item Name="LVBoundsTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVBoundsTypeDef.ctl"/>
				<Item Name="Get String Text Bounds.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Get String Text Bounds.vi"/>
				<Item Name="Get Text Rect.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Get Text Rect.vi"/>
				<Item Name="Convert property node font to graphics font.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Convert property node font to graphics font.vi"/>
				<Item Name="Longest Line Length in Pixels.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Longest Line Length in Pixels.vi"/>
				<Item Name="Three Button Dialog CORE.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Three Button Dialog CORE.vi"/>
				<Item Name="Three Button Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Three Button Dialog.vi"/>
				<Item Name="Not Found Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Not Found Dialog.vi"/>
				<Item Name="eventvkey.ctl" Type="VI" URL="/&lt;vilib&gt;/event_ctls.llb/eventvkey.ctl"/>
				<Item Name="ErrWarn.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/ErrWarn.ctl"/>
				<Item Name="Set Bold Text.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Set Bold Text.vi"/>
				<Item Name="Clear Errors.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Clear Errors.vi"/>
				<Item Name="Details Display Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Details Display Dialog.vi"/>
				<Item Name="Search and Replace Pattern.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Search and Replace Pattern.vi"/>
				<Item Name="Find Tag.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Find Tag.vi"/>
				<Item Name="Format Message String.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Format Message String.vi"/>
				<Item Name="Error Code Database.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Code Database.vi"/>
				<Item Name="GetRTHostConnectedProp.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/GetRTHostConnectedProp.vi"/>
				<Item Name="Set String Value.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Set String Value.vi"/>
				<Item Name="TagReturnType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/TagReturnType.ctl"/>
				<Item Name="Check Special Tags.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Check Special Tags.vi"/>
				<Item Name="General Error Handler.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler.vi"/>
				<Item Name="Simple Error Handler.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Simple Error Handler.vi"/>
				<Item Name="Write To Spreadsheet File.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Write To Spreadsheet File.vi"/>
				<Item Name="Write To Spreadsheet File (DBL).vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Write To Spreadsheet File (DBL).vi"/>
				<Item Name="Write Spreadsheet String.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Write Spreadsheet String.vi"/>
				<Item Name="Write To Spreadsheet File (I64).vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Write To Spreadsheet File (I64).vi"/>
				<Item Name="Write To Spreadsheet File (string).vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Write To Spreadsheet File (string).vi"/>
				<Item Name="3DPC_SurfacePlot.xctl" Type="XControl" URL="/&lt;vilib&gt;/Native 3D Graph/xcontrol/3DPC_SurfacePlot.xctl"/>
				<Item Name="NI_AALPro.lvlib" Type="Library" URL="/&lt;vilib&gt;/Analysis/NI_AALPro.lvlib"/>
				<Item Name="LVCursorListTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVCursorListTypeDef.ctl"/>
				<Item Name="Set Cursor (Icon Pict).vi" Type="VI" URL="/&lt;vilib&gt;/Utility/cursorutil.llb/Set Cursor (Icon Pict).vi"/>
				<Item Name="LVRowAndColumnTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVRowAndColumnTypeDef.ctl"/>
				<Item Name="Set Cursor.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/cursorutil.llb/Set Cursor.vi"/>
				<Item Name="Set Cursor (Cursor ID).vi" Type="VI" URL="/&lt;vilib&gt;/Utility/cursorutil.llb/Set Cursor (Cursor ID).vi"/>
				<Item Name="subTimeDelay.vi" Type="VI" URL="/&lt;vilib&gt;/express/express execution control/TimeDelayBlock.llb/subTimeDelay.vi"/>
				<Item Name="Base Datatype.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/Native 3D Graph/Classes/Base Datatype.lvclass"/>
				<Item Name="Parametric.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/Native 3D Graph/Classes/Parametric/Parametric.lvclass"/>
				<Item Name="Binary Search Sorted Array.vi" Type="VI" URL="/&lt;vilib&gt;/Native 3D Graph/Binary Search Sorted Array.vi"/>
				<Item Name="Surface.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/Native 3D Graph/Classes/Surface/Surface.lvclass"/>
				<Item Name="XControlSupport.lvlib" Type="Library" URL="/&lt;vilib&gt;/_xctls/XControlSupport.lvlib"/>
				<Item Name="Line.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/Native 3D Graph/Classes/Line/Line.lvclass"/>
				<Item Name="General Error Handler Core CORE.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler Core CORE.vi"/>
				<Item Name="LVRectTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVRectTypeDef.ctl"/>
				<Item Name="Application Directory.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Application Directory.vi"/>
				<Item Name="NI_FileType.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/lvfile.llb/NI_FileType.lvlib"/>
				<Item Name="Check if File or Folder Exists.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/libraryn.llb/Check if File or Folder Exists.vi"/>
				<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Cluster From Error Code.vi"/>
				<Item Name="NI_PackedLibraryUtility.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/LVLibp/NI_PackedLibraryUtility.lvlib"/>
				<Item Name="Space Constant.vi" Type="VI" URL="/&lt;vilib&gt;/dlg_ctls.llb/Space Constant.vi"/>
				<Item Name="Semaphore RefNum" Type="VI" URL="/&lt;vilib&gt;/Utility/semaphor.llb/Semaphore RefNum"/>
				<Item Name="Obtain Semaphore Reference.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/semaphor.llb/Obtain Semaphore Reference.vi"/>
				<Item Name="Semaphore Refnum Core.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/semaphor.llb/Semaphore Refnum Core.ctl"/>
				<Item Name="AddNamedSemaphorePrefix.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/semaphor.llb/AddNamedSemaphorePrefix.vi"/>
				<Item Name="GetNamedSemaphorePrefix.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/semaphor.llb/GetNamedSemaphorePrefix.vi"/>
				<Item Name="Validate Semaphore Size.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/semaphor.llb/Validate Semaphore Size.vi"/>
				<Item Name="Acquire Semaphore.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/semaphor.llb/Acquire Semaphore.vi"/>
				<Item Name="Release Semaphore.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/semaphor.llb/Release Semaphore.vi"/>
				<Item Name="Not A Semaphore.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/semaphor.llb/Not A Semaphore.vi"/>
				<Item Name="Release Semaphore Reference.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/semaphor.llb/Release Semaphore Reference.vi"/>
				<Item Name="RemoveNamedSemaphorePrefix.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/semaphor.llb/RemoveNamedSemaphorePrefix.vi"/>
			</Item>
			<Item Name="Menu.rtm" Type="Document" URL="../Menu.rtm"/>
			<Item Name="lvanlys.dll" Type="Document" URL="/&lt;resource&gt;/lvanlys.dll"/>
			<Item Name="Cut out additional spaces.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Cut out additional spaces.vi"/>
			<Item Name="qSVO.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/GeneralCommands/qSVO.vi"/>
			<Item Name="qERR.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/GeneralCommands/qERR.vi"/>
			<Item Name="qDRL.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/SpecialCommand/qDRL.vi"/>
			<Item Name="qDRR.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/SpecialCommand/qDRR.vi"/>
			<Item Name="qIDN.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/GeneralCommands/qIDN.vi"/>
			<Item Name="qPOS.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/GeneralCommands/qPOS.vi"/>
			<Item Name="qTCI.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/ControllerAlgorithms/qTCI.vi"/>
			<Item Name="qFGC.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/ControllerAlgorithms/qFGC.vi"/>
			<Item Name="qFRF.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Limits/qFRF.vi"/>
			<Item Name="qFRR.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/ControllerAlgorithms/qFRR.vi"/>
			<Item Name="qFRC.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/ControllerAlgorithms/qFRC.vi"/>
			<Item Name="qSIC.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/ControllerAlgorithms/qSIC.vi"/>
			<Item Name="qHLP.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/GeneralCommands/qHLP.vi"/>
			<Item Name="Is command present in HLP answer.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Is command present in HLP answer.vi"/>
			<Item Name="qFRP.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/ControllerAlgorithms/qFRP.vi"/>
			<Item Name="Controller names.ctl" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/GeneralCommands/Controller names.ctl"/>
			<Item Name="Available interfaces.ctl" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/Available interfaces.ctl"/>
			<Item Name="Available DLLs.ctl" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/Available DLLs.ctl"/>
			<Item Name="Available DLL interfaces.ctl" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/Available DLL interfaces.ctl"/>
			<Item Name="Termination character.ctl" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/Termination character.ctl"/>
			<Item Name="Syntax.ctl" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/Syntax.ctl"/>
			<Item Name="Global1.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/Global1.vi"/>
			<Item Name="GCSTranslator DLL Functions.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/GCSTranslator DLL Functions.vi"/>
			<Item Name="GetGCSTranslatorDLLPath.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/GetGCSTranslatorDLLPath.vi"/>
			<Item Name="Global DaisyChain.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/Global DaisyChain.vi"/>
			<Item Name="Get lines from string.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Get lines from string.vi"/>
			<Item Name="Analog Functions.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/AnalogControl/Analog Functions.vi"/>
			<Item Name="Available Analog Commands.ctl" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/AnalogControl/Available Analog Commands.ctl"/>
			<Item Name="String with ASCII code conversion.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/String with ASCII code conversion.vi"/>
			<Item Name="Analog Receive String.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/AnalogControl/Analog Receive String.vi"/>
			<Item Name="PI VISA Receive Characters.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/PI VISA Receive Characters.vi"/>
			<Item Name="Multi Axis Query.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Multi Axis Query.vi"/>
			<Item Name="Commanded axes connected.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Commanded axes connected.vi"/>
			<Item Name="Get arrays without blanks.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Get arrays without blanks.vi"/>
			<Item Name="Get all axes.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Get all axes.vi"/>
			<Item Name="PI Query.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/PI Query.vi"/>
			<Item Name="PI acquire semaphore.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/PI acquire semaphore.vi"/>
			<Item Name="PI release semaphore.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/PI release semaphore.vi"/>
			<Item Name="Assign booleans from string to axes.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Assign booleans from string to axes.vi"/>
			<Item Name="Assign DRC values.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Assign DRC values.vi"/>
			<Item Name="Count occurrences in string.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Count occurrences in string.vi"/>
			<Item Name="TableIO.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/FileHandling/TableIO.vi"/>
			<Item Name="GCSArray_regex_selector.ctl" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/GCSArray_regex_selector.ctl"/>
			<Item Name="GCSArray_regex_range.ctl" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/GCSArray_regex_range.ctl"/>
			<Item Name="GCSArray_regex_select.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/GCSArray_regex_select.vi"/>
			<Item Name="GCSArray_regex_string.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/GCSArray_regex_string.vi"/>
			<Item Name="GCSArray_valid.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/GCSArray_valid.vi"/>
			<Item Name="GCSArray_valid_action.ctl" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/GCSArray_valid_action.ctl"/>
			<Item Name="GCSArray_header_read.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/GCSArray_header_read.vi"/>
			<Item Name="GCSArray_header.ctl" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/GCSArray_header.ctl"/>
			<Item Name="GCSArray_header_action.ctl" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/GCSArray_header_action.ctl"/>
			<Item Name="GCSArray_regex_matches.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/GCSArray_regex_matches.vi"/>
			<Item Name="GCSArray_difference.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/GCSArray_difference.vi"/>
			<Item Name="GCSArray_intersection.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/GCSArray_intersection.vi"/>
			<Item Name="GCSArray_import.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/GCSArray_import.vi"/>
			<Item Name="PI Send.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/PI Send.vi"/>
			<Item Name="Get Syntax In Global.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/Get Syntax In Global.vi"/>
			<Item Name="Build command substring.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Build command substring.vi"/>
			<Item Name="Return space.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Return space.vi"/>
			<Item Name="Assign values from string to axes.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Assign values from string to axes.vi"/>
			<Item Name="DRC.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/SpecialCommand/DRC.vi"/>
			<Item Name="SVO.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/GeneralCommands/SVO.vi"/>
			<Item Name="PI Send String.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/PI Send String.vi"/>
			<Item Name="PI Receive String.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Communication/PI Receive String.vi"/>
			<Item Name="PITerm.vi" Type="VI" URL="../PITerm.vi"/>
			<Item Name="Global2 (Array).vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/GeneralCommands/Global2 (Array).vi"/>
			<Item Name="Build SPA command substring.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Build SPA command substring.vi"/>
			<Item Name="Build query command substring.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Build query command substring.vi"/>
			<Item Name="ClearMotionError.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/ClearMotionError.vi"/>
			<Item Name="MOV.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/GeneralCommands/MOV.vi"/>
			<Item Name="DRT.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/SpecialCommand/DRT.vi"/>
			<Item Name="RTR.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/SpecialCommand/RTR.vi"/>
			<Item Name="IMP.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/SpecialCommand/IMP.vi"/>
			<Item Name="FDR.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/ControllerAlgorithms/FDR.vi"/>
			<Item Name="FDG.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/ControllerAlgorithms/FDG.vi"/>
			<Item Name="STP.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/GeneralCommands/STP.vi"/>
			<Item Name="GCSTranslateError.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/GCSTranslateError.vi"/>
			<Item Name="MVR.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/GeneralCommands/MVR.vi"/>
			<Item Name="FRF.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Limits/FRF.vi"/>
			<Item Name="FRP.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/ControllerAlgorithms/FRP.vi"/>
			<Item Name="Assign SPA values from string to axes.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Assign SPA values from string to axes.vi"/>
			<Item Name="Convert num array to string.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Convert num array to string.vi"/>
			<Item Name="SIC.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/ControllerAlgorithms/SIC.vi"/>
			<Item Name="FRS.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/ControllerAlgorithms/FRS.vi"/>
			<Item Name="Build num command substring.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Build num command substring.vi"/>
			<Item Name="Assign two values from string to axes.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Assign two values from string to axes.vi"/>
			<Item Name="FGC.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/ControllerAlgorithms/FGC.vi"/>
			<Item Name="Build SPA query command substring.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Build SPA query command substring.vi"/>
			<Item Name="qFRH.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/ControllerAlgorithms/qFRH.vi"/>
			<Item Name="Parse_FRH_Type_String.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Parse_FRH_Type_String.vi"/>
			<Item Name="#24.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/SpecialCommand/#24.vi"/>
			<Item Name="FRC.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/ControllerAlgorithms/FRC.vi"/>
			<Item Name="Global Analog.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/AnalogControl/Global Analog.vi"/>
			<Item Name="Wait for axes to stop.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Wait for axes to stop.vi"/>
			<Item Name="InMotion.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/InMotion.vi"/>
			<Item Name="qSTV.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/GeneralCommands/qSTV.vi"/>
			<Item Name="Status.ctl" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/GeneralCommands/Status.ctl"/>
			<Item Name="Assign status from string.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Assign status from string.vi"/>
			<Item Name="#5.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/SpecialCommand/#5.vi"/>
			<Item Name="qSTA.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/SpecialCommand/qSTA.vi"/>
			<Item Name="qONT.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/GeneralCommands/qONT.vi"/>
			<Item Name="Wait for hexapod system axes to stop.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/OldCommands/Wait for hexapod system axes to stop.vi"/>
			<Item Name="#5_old.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/OldCommands/#5_old.vi"/>
			<Item Name="General wait for movement to stop.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/General wait for movement to stop.vi"/>
			<Item Name="IsControllerBusy.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/IsControllerBusy.vi"/>
			<Item Name="#7.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/SpecialCommand/#7.vi"/>
			<Item Name="Wait for controller ready.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/Wait for controller ready.vi"/>
			<Item Name="HasCommandArray.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/HasCommandArray.vi"/>
			<Item Name="HasCommand.vi" Type="VI" URL="/C/ProgramData/PI/LabVIEW/Low Level/Support/HasCommand.vi"/>
		</Item>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
