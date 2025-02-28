<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="24008000">
	<Item Name="My Computer" Type="My Computer">
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="Find_String.vi" Type="VI" URL="../Find_String.vi"/>
		<Item Name="States_Ctl.ctl" Type="VI" URL="../States_Ctl.ctl"/>
		<Item Name="Wait.vi" Type="VI" URL="../Wait.vi"/>
		<Item Name="Write_Read.vi" Type="VI" URL="../Write_Read.vi"/>
		<Item Name="YOKOGAWA_CSU-X1.vi" Type="VI" URL="../YOKOGAWA_CSU-X1.vi"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="VISA Configure Serial Port" Type="VI" URL="/&lt;vilib&gt;/Instr/_visa.llb/VISA Configure Serial Port"/>
				<Item Name="VISA Configure Serial Port (Instr).vi" Type="VI" URL="/&lt;vilib&gt;/Instr/_visa.llb/VISA Configure Serial Port (Instr).vi"/>
				<Item Name="VISA Configure Serial Port (Serial Instr).vi" Type="VI" URL="/&lt;vilib&gt;/Instr/_visa.llb/VISA Configure Serial Port (Serial Instr).vi"/>
				<Item Name="VISA Flush IO Buffer Mask.ctl" Type="VI" URL="/&lt;vilib&gt;/Instr/_visa.llb/VISA Flush IO Buffer Mask.ctl"/>
			</Item>
			<Item Name="User_Message.vi" Type="VI" URL="../User_Message.vi"/>
		</Item>
		<Item Name="Build Specifications" Type="Build">
			<Item Name="YOKOGAWA_CSU-X1" Type="EXE">
				<Property Name="App_copyErrors" Type="Bool">true</Property>
				<Property Name="App_INI_aliasGUID" Type="Str">{B004F3A9-31A4-4C2F-8DE8-2031185CB7D0}</Property>
				<Property Name="App_INI_GUID" Type="Str">{AD015DB0-8D3C-4D86-9F1B-9B6EAB07FA7F}</Property>
				<Property Name="App_serverConfig.httpPort" Type="Int">8002</Property>
				<Property Name="App_serverType" Type="Int">1</Property>
				<Property Name="Bld_buildCacheID" Type="Str">{628C2AC0-05B4-4A6F-8DB0-4106681C9CD5}</Property>
				<Property Name="Bld_buildSpecName" Type="Str">YOKOGAWA_CSU-X1</Property>
				<Property Name="Bld_excludeInlineSubVIs" Type="Bool">true</Property>
				<Property Name="Bld_excludeLibraryItems" Type="Bool">true</Property>
				<Property Name="Bld_excludePolymorphicVIs" Type="Bool">true</Property>
				<Property Name="Bld_localDestDir" Type="Path">../builds/NI_AB_PROJECTNAME/YOKOGAWA_CSU-X1</Property>
				<Property Name="Bld_localDestDirType" Type="Str">relativeToCommon</Property>
				<Property Name="Bld_modifyLibraryFile" Type="Bool">true</Property>
				<Property Name="Bld_previewCacheID" Type="Str">{A06D35E5-57C8-4C42-ACF7-E9A0D130A4D7}</Property>
				<Property Name="Bld_version.major" Type="Int">1</Property>
				<Property Name="Destination[0].destName" Type="Str">YOKOGAWA_CSU-X1.exe</Property>
				<Property Name="Destination[0].libraryName" Type="Str">YOKOGAWA_CSU-X1.lvlib</Property>
				<Property Name="Destination[0].path" Type="Path">../builds/NI_AB_PROJECTNAME/YOKOGAWA_CSU-X1/YOKOGAWA_CSU-X1.exe</Property>
				<Property Name="Destination[0].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[0].type" Type="Str">App</Property>
				<Property Name="Destination[1].destName" Type="Str">Support Directory</Property>
				<Property Name="Destination[1].path" Type="Path">../builds/NI_AB_PROJECTNAME/YOKOGAWA_CSU-X1/data</Property>
				<Property Name="DestinationCount" Type="Int">2</Property>
				<Property Name="Source[0].itemID" Type="Str">{1F8D4A0C-26E1-4847-9BFE-D28C58DB5971}</Property>
				<Property Name="Source[0].type" Type="Str">Container</Property>
				<Property Name="Source[1].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[1].itemID" Type="Ref">/My Computer/YOKOGAWA_CSU-X1.vi</Property>
				<Property Name="Source[1].sourceInclusion" Type="Str">TopLevel</Property>
				<Property Name="Source[1].type" Type="Str">VI</Property>
				<Property Name="SourceCount" Type="Int">2</Property>
				<Property Name="TgtF_companyName" Type="Str">CNRS</Property>
				<Property Name="TgtF_fileDescription" Type="Str">Control of a spinning disk YOKOGAWA_CSU-X1 unit with two 6-position filter wheels</Property>
				<Property Name="TgtF_internalName" Type="Str">YOKOGAWA_CSU-X1</Property>
				<Property Name="TgtF_legalCopyright" Type="Str">Copyright MAILFERT Institut Fresnel © 2024 CNRS</Property>
				<Property Name="TgtF_productName" Type="Str">YOKOGAWA_CSU-X1</Property>
				<Property Name="TgtF_targetfileGUID" Type="Str">{DA74FD56-58F7-4D03-BC5A-4885BDE9C0D1}</Property>
				<Property Name="TgtF_targetfileName" Type="Str">YOKOGAWA_CSU-X1.exe</Property>
			</Item>
		</Item>
	</Item>
</Project>
