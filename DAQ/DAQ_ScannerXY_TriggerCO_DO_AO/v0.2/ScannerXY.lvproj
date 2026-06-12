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
		<Item Name="v0.2" Type="Folder">
			<Item Name=".ScannerXY.UserState" Type="Folder">
				<Item Name="debug.lvprojstate" Type="Document" URL="../.ScannerXY.UserState/debug.lvprojstate"/>
			</Item>
			<Item Name="SubVIs" Type="Folder">
				<Item Name="Create_WF.vi" Type="VI" URL="../SubVIs/Create_WF.vi"/>
				<Item Name="DAQ_Configure.vi" Type="VI" URL="../SubVIs/DAQ_Configure.vi"/>
				<Item Name="DAQ_Configure_it_live.vi" Type="VI" URL="../SubVIs/DAQ_Configure_it_live.vi"/>
				<Item Name="DAQ_Configure_ScanXY.vi" Type="VI" URL="../SubVIs/DAQ_Configure_ScanXY.vi"/>
				<Item Name="DAQ_Start_Tasks.vi" Type="VI" URL="../SubVIs/DAQ_Start_Tasks.vi"/>
				<Item Name="DAQ_Stop_Clear_Tasks.vi" Type="VI" URL="../SubVIs/DAQ_Stop_Clear_Tasks.vi"/>
				<Item Name="DAQ_Stop_Tasks.vi" Type="VI" URL="../SubVIs/DAQ_Stop_Tasks.vi"/>
				<Item Name="Reconstruct_Image.vi" Type="VI" URL="../SubVIs/Reconstruct_Image.vi"/>
				<Item Name="Remove_First_Point.vi" Type="VI" URL="../SubVIs/Remove_First_Point.vi"/>
				<Item Name="Scaling_WF.vi" Type="VI" URL="../SubVIs/Scaling_WF.vi"/>
				<Item Name="Wait.vi" Type="VI" URL="../SubVIs/Wait.vi"/>
			</Item>
			<Item Name="Main.vi" Type="VI" URL="../Main.vi"/>
			<Item Name="ScannerXY.aliases" Type="Document" URL="../ScannerXY.aliases"/>
			<Item Name="ScannerXY.lvlps" Type="Document" URL="../ScannerXY.lvlps"/>
		</Item>
		<Item Name="Dépendances" Type="Dependencies"/>
		<Item Name="Spécifications de construction" Type="Build"/>
	</Item>
</Project>
