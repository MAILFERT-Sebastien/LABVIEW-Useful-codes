<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="19008000">
	<Property Name="CCSymbols" Type="Str"></Property>
	<Property Name="NI.LV.All.SaveVersion" Type="Str">19.0</Property>
	<Property Name="NI.LV.All.SourceOnly" Type="Bool">true</Property>
	<Property Name="NI.Project.Description" Type="Str"></Property>
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
		<Item Name="pco.convert" Type="Folder">
			<Item Name="Convert.lvlib" Type="Library" URL="../pco.convert/Convert.lvlib"/>
			<Item Name="getColorMode.vi" Type="VI" URL="../pco.convert/getColorMode.vi"/>
			<Item Name="initConversion.vi" Type="VI" URL="../pco.convert/initConversion.vi"/>
			<Item Name="activateAIDenoiser.vi" Type="VI" URL="../pco.convert/activateAIDenoiser.vi"/>
		</Item>
		<Item Name="errorhandling" Type="Folder">
			<Item Name="ErrorManager.vi" Type="VI" URL="../errorHandling/ErrorManager.vi"/>
		</Item>
		<Item Name="etc.xcite" Type="Folder">
			<Item Name="XCite.lvlib" Type="Library" URL="../etc.xcite/XCite.lvlib"/>
		</Item>
		<Item Name="examples" Type="Folder">
			<Item Name="deprecated" Type="Folder">
				<Item Name="pco.labview_Example.vi" Type="VI" URL="../Examples/deprecated/pco.labview_Example.vi"/>
				<Item Name="pco.labview_LightSheet_Example.vi" Type="VI" URL="../Examples/deprecated/pco.labview_LightSheet_Example.vi"/>
				<Item Name="pco.labview_Live_CamRam_Example.vi" Type="VI" URL="../Examples/deprecated/pco.labview_Live_CamRam_Example.vi"/>
				<Item Name="pco.labview_Live_Example.vi" Type="VI" URL="../Examples/deprecated/pco.labview_Live_Example.vi"/>
				<Item Name="pco.labview_Live_Example_COL.vi" Type="VI" URL="../Examples/deprecated/pco.labview_Live_Example_COL.vi"/>
				<Item Name="pco.labview_Live_Example_PSEUDO_COL.vi" Type="VI" URL="../Examples/deprecated/pco.labview_Live_Example_PSEUDO_COL.vi"/>
				<Item Name="pco.labview_Live_SoftTrigger_Example.vi" Type="VI" URL="../Examples/deprecated/pco.labview_Live_SoftTrigger_Example.vi"/>
				<Item Name="pco.labview_Simple_CamRam_Example.vi" Type="VI" URL="../Examples/deprecated/pco.labview_Simple_CamRam_Example.vi"/>
				<Item Name="pco.labview_Simple_Example.vi" Type="VI" URL="../Examples/deprecated/pco.labview_Simple_Example.vi"/>
			</Item>
			<Item Name="camram" Type="Folder">
				<Item Name="Live_Camram_Example.vi" Type="VI" URL="../Examples/camram/Live_Camram_Example.vi"/>
				<Item Name="Live_Camram_Example_IMAQ.vi" Type="VI" URL="../Examples/camram/Live_Camram_Example_IMAQ.vi"/>
				<Item Name="Read_Camram_Example.vi" Type="VI" URL="../Examples/camram/Read_Camram_Example.vi"/>
				<Item Name="Read_Camram_Example_IMAQ.vi" Type="VI" URL="../Examples/camram/Read_Camram_Example_IMAQ.vi"/>
				<Item Name="Simple_Camram_Example.vi" Type="VI" URL="../Examples/camram/Simple_Camram_Example.vi"/>
				<Item Name="Simple_Camram_Example_IMAQ.vi" Type="VI" URL="../Examples/camram/Simple_Camram_Example_IMAQ.vi"/>
			</Item>
			<Item Name="Avg_Example.vi" Type="VI" URL="../Examples/Avg_Example.vi"/>
			<Item Name="Avg_Example_IMAQ.vi" Type="VI" URL="../Examples/Avg_Example_IMAQ.vi"/>
			<Item Name="FloatingMean_Example.vi" Type="VI" URL="../Examples/FloatingMean_Example.vi"/>
			<Item Name="FloatingMean_Example_IMAQ.vi" Type="VI" URL="../Examples/FloatingMean_Example_IMAQ.vi"/>
			<Item Name="LightSheet_Example.vi" Type="VI" URL="../Examples/LightSheet_Example.vi"/>
			<Item Name="LightSheet_Example_IMAQ.vi" Type="VI" URL="../Examples/LightSheet_Example_IMAQ.vi"/>
			<Item Name="Live_ConvertSetting_Examples.vi" Type="VI" URL="../Examples/Live_ConvertSetting_Examples.vi"/>
			<Item Name="Live_ConvertSettings_Example_IMAQ.vi" Type="VI" URL="../Examples/Live_ConvertSettings_Example_IMAQ.vi"/>
			<Item Name="Live_Example.vi" Type="VI" URL="../Examples/Live_Example.vi"/>
			<Item Name="Live_Example_XCite.vi" Type="VI" URL="../Examples/Live_Example_XCite.vi"/>
			<Item Name="Live_Example_IMAQ.vi" Type="VI" URL="../Examples/Live_Example_IMAQ.vi"/>
			<Item Name="Live_Example_XCite_IMAQ.vi" Type="VI" URL="../Examples/Live_Example_XCite_IMAQ.vi"/>
			<Item Name="SaveImages_Example.vi" Type="VI" URL="../Examples/SaveImages_Example.vi"/>
			<Item Name="SaveImages_Example_IMAQ.vi" Type="VI" URL="../Examples/SaveImages_Example_IMAQ.vi"/>
			<Item Name="SaveMean_Example.vi" Type="VI" URL="../Examples/SaveMean_Example.vi"/>
			<Item Name="SaveMean_Example_IMAQ.vi" Type="VI" URL="../Examples/SaveMean_Example_IMAQ.vi"/>
			<Item Name="Simple_Example.vi" Type="VI" URL="../Examples/Simple_Example.vi"/>
			<Item Name="Simple_Example_IMAQ.vi" Type="VI" URL="../Examples/Simple_Example_IMAQ.vi"/>
			<Item Name="Simple_Example_2Cam.vi" Type="VI" URL="../Examples/Simple_Example_2Cam.vi"/>
			<Item Name="Simple_Example_2Cam_IMAQ.vi" Type="VI" URL="../Examples/Simple_Example_2Cam_IMAQ.vi"/>
			<Item Name="Simple_Example_Soft_Trigger.vi" Type="VI" URL="../Examples/Simple_Example_Soft_Trigger.vi"/>
			<Item Name="Simple_Example_dicam.vi" Type="VI" URL="../Examples/Simple_Example_dicam.vi"/>
			<Item Name="XCite_Only_Example.vi" Type="VI" URL="../Examples/XCite_Only_Example.vi"/>
		</Item>
		<Item Name="pco.recorder" Type="Folder">
			<Item Name="Recorder.lvlib" Type="Library" URL="../pco.recorder/Recorder.lvlib"/>
		</Item>
		<Item Name="pco.sdk" Type="Folder">
			<Property Name="NI.SortType" Type="Int">3</Property>
			<Item Name="SDK.lvlib" Type="Library" URL="../pco.sdk/SDK.lvlib"/>
		</Item>
		<Item Name="misc" Type="Folder">
			<Item Name="bcd_counter.vi" Type="VI" URL="../misc/bcd_counter.vi"/>
			<Item Name="C2LVstring.vi" Type="VI" URL="../misc/C2LVstring.vi"/>
			<Item Name="LVstring2C.vi" Type="VI" URL="../misc/LVstring2C.vi"/>
			<Item Name="BCDToDecimal.vi" Type="VI" URL="../misc/BCDToDecimal.vi"/>
			<Item Name="DecimalToBCD.vi" Type="VI" URL="../misc/DecimalToBCD.vi"/>
		</Item>
		<Item Name="GetDLLPaths.vi" Type="VI" URL="../GetDLLPaths.vi"/>
		<Item Name="Xcite.lvclass" Type="LVClass" URL="../xcite/Xcite.lvclass"/>
		<Item Name="Camera.lvclass" Type="LVClass" URL="../camera/Camera.lvclass"/>
		<Item Name="Dependencies" Type="Dependencies"/>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
