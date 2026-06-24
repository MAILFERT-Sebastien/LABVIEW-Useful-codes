<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="26008000">
	<Property Name="NI.LV.All.SaveVersion" Type="Str">26.0</Property>
	<Property Name="NI.LV.All.SourceOnly" Type="Bool">true</Property>
	<Item Name="Poste de travail" Type="My Computer">
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">Poste de travail/VI Serveur</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">Poste de travail/VI Serveur</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="v0.0" Type="Folder" URL="..">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="#7.vi" Type="VI" URL="/&lt;instrlib&gt;/PI/Low Level/SpecialCommand/#7.vi"/>
		<Item Name="#24.vi" Type="VI" URL="/&lt;instrlib&gt;/PI/Low Level/SpecialCommand/#24.vi"/>
		<Item Name="Assign values from string to axes.vi" Type="VI" URL="/&lt;instrlib&gt;/PI/Low Level/Support/Assign values from string to axes.vi"/>
		<Item Name="ATZ.vi" Type="VI" URL="/&lt;instrlib&gt;/PI/Low Level/Limits/ATZ.vi"/>
		<Item Name="Define connected axes.vi" Type="VI" URL="/&lt;instrlib&gt;/PI/Low Level/GeneralCommands/Define connected axes.vi"/>
		<Item Name="Define connected systems (Array).vi" Type="VI" URL="/&lt;instrlib&gt;/PI/Low Level/GeneralCommands/Define connected systems (Array).vi"/>
		<Item Name="Initialize Global DaisyChain.vi" Type="VI" URL="/&lt;instrlib&gt;/PI/Low Level/Communication/Initialize Global DaisyChain.vi"/>
		<Item Name="PI Open Interface of one system.vi" Type="VI" URL="/&lt;instrlib&gt;/PI/Low Level/Communication/PI Open Interface of one system.vi"/>
		<Item Name="PI Receive String.vi" Type="VI" URL="/&lt;instrlib&gt;/PI/Low Level/Communication/PI Receive String.vi"/>
		<Item Name="Select DaisyChain device.vi" Type="VI" URL="/&lt;instrlib&gt;/PI/Low Level/Communication/Select DaisyChain device.vi"/>
		<Item Name="Select USB device.vi" Type="VI" URL="/&lt;instrlib&gt;/PI/Low Level/Communication/Select USB device.vi"/>
		<Item Name="SVO.vi" Type="VI" URL="/&lt;instrlib&gt;/PI/Low Level/GeneralCommands/SVO.vi"/>
		<Item Name="Dépendances" Type="Dependencies"/>
		<Item Name="Spécifications de construction" Type="Build"/>
	</Item>
</Project>
