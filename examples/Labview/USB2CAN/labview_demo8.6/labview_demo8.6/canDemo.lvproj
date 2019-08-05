<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="8608001">
	<Item Name="我的电脑" Type="My Computer">
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">我的电脑/VI服务器</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">我的电脑/VI服务器</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="demo.vi" Type="VI" URL="../demo.vi"/>
		<Item Name="boardinfo.vi" Type="VI" URL="../boardinfo.vi"/>
		<Item Name="errinfo.vi" Type="VI" URL="../errinfo.vi"/>
		<Item Name="status.vi" Type="VI" URL="../status.vi"/>
		<Item Name="依赖关系" Type="Dependencies">
			<Item Name="ControlCAN.dll" Type="Document" URL="../ControlCAN.dll"/>
		</Item>
		<Item Name="程序生成规范" Type="Build">
			<Item Name="TestCAN8.6" Type="EXE">
				<Property Name="App_applicationGUID" Type="Str">{D02987C7-7054-47EF-80AB-03E2436488AE}</Property>
				<Property Name="App_applicationName" Type="Str">TestCAN8.6.exe</Property>
				<Property Name="App_companyName" Type="Str">ZHIYUAN</Property>
				<Property Name="App_fileVersion.major" Type="Int">1</Property>
				<Property Name="App_INI_aliasGUID" Type="Str">{A444A2BF-8F5C-4762-A1D4-0053E2D10BFE}</Property>
				<Property Name="App_INI_GUID" Type="Str">{DD39B08A-2295-4A8E-B469-0DF564DF7E6D}</Property>
				<Property Name="App_internalName" Type="Str">TestCAN8.6</Property>
				<Property Name="App_legalCopyright" Type="Str">版权(C) 2009 ZHIYUAN</Property>
				<Property Name="App_productName" Type="Str">TestCAN8.6</Property>
				<Property Name="Bld_buildSpecName" Type="Str">TestCAN8.6</Property>
				<Property Name="Bld_defaultLanguage" Type="Str">ChineseS</Property>
				<Property Name="Bld_excludeLibraryItems" Type="Bool">true</Property>
				<Property Name="Bld_excludePolymorphicVIs" Type="Bool">true</Property>
				<Property Name="Bld_excludeTypedefs" Type="Bool">true</Property>
				<Property Name="Bld_modifyLibraryFile" Type="Bool">true</Property>
				<Property Name="Bld_supportedLanguage[0]" Type="Str">ChineseS</Property>
				<Property Name="Bld_supportedLanguage[1]" Type="Str">English</Property>
				<Property Name="Bld_supportedLanguageCount" Type="Int">2</Property>
				<Property Name="Destination[0].destName" Type="Str">TestCAN8.6.exe</Property>
				<Property Name="Destination[0].path" Type="Path">../labview_demo8.6/TestCAN8.6/internal.llb</Property>
				<Property Name="Destination[0].type" Type="Str">App</Property>
				<Property Name="Destination[1].destName" Type="Str">支持目录</Property>
				<Property Name="Destination[1].path" Type="Path">../labview_demo8.6/TestCAN8.6/data</Property>
				<Property Name="Destination[2].destName" Type="Str">目标目录</Property>
				<Property Name="Destination[2].path" Type="Path">../labview_demo8.6/TestCAN8.6</Property>
				<Property Name="DestinationCount" Type="Int">3</Property>
				<Property Name="Source[0].Container.applyDestination" Type="Bool">true</Property>
				<Property Name="Source[0].Container.applyInclusion" Type="Bool">true</Property>
				<Property Name="Source[0].Container.applyProperties" Type="Bool">true</Property>
				<Property Name="Source[0].itemID" Type="Str">{588E6AC8-2436-4B39-B2B1-555C9C3DFFA7}</Property>
				<Property Name="Source[0].type" Type="Str">Container</Property>
				<Property Name="Source[1].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[1].itemID" Type="Ref">/我的电脑/demo.vi</Property>
				<Property Name="Source[1].properties[0].type" Type="Str">Remove front panel</Property>
				<Property Name="Source[1].properties[0].value" Type="Bool">false</Property>
				<Property Name="Source[1].propertiesCount" Type="Int">1</Property>
				<Property Name="Source[1].sourceInclusion" Type="Str">TopLevel</Property>
				<Property Name="Source[1].type" Type="Str">VI</Property>
				<Property Name="Source[2].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[2].itemID" Type="Ref">/我的电脑/boardinfo.vi</Property>
				<Property Name="Source[2].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[2].type" Type="Str">VI</Property>
				<Property Name="Source[3].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[3].itemID" Type="Ref">/我的电脑/errinfo.vi</Property>
				<Property Name="Source[3].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[3].type" Type="Str">VI</Property>
				<Property Name="Source[4].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[4].itemID" Type="Ref">/我的电脑/status.vi</Property>
				<Property Name="Source[4].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[4].type" Type="Str">VI</Property>
				<Property Name="SourceCount" Type="Int">5</Property>
			</Item>
		</Item>
	</Item>
</Project>
