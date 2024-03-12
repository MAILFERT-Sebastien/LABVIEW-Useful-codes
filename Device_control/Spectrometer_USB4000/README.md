# LABVIEW_OceanOptics_Spectrometer
[LABVIEW] Control of an Ocean Optics USB4000 spectrometer AND measure of the fluorescence transmission of filters


## Aim
This software is designed to control an Ocean Optics USB4000 spectrometer.
As you can see on the following video, the Labview GUI is simple and allows to read the infomation coming from the spectrometer, control it AND measure the transmission of fluorescence filter:

[![Watch the video](https://github.com/MAILFERT-Sebastien/LABVIEW-Useful-codes/tree/main/Device_control/Spectrometer_USB4000/Images/Labview_OceanOptics_USB4000.png)](https://www.youtube.com/watch?v=lz8w-zJ4CPQ)
[![Watch the video](https://github.com/MAILFERT-Sebastien/LABVIEW-Useful-codes/tree/main/Device_control/Spectrometer_USB4000/Images/Labview_OceanOptics_USB4000_2.png)](https://youtu.be/C9iH8P3rPok)

## Prerequisites
This program has been tested under Labview 2023 64-bit. It is therefore necessary to have at least one version of Labview 2023 installed.
> [!TIP]
> If you contact me, I can provide you with a version that is compatible with yours (older or newest).

## Installing

1. Ocean Optics drivers
   <ul>
      <li> Open <i>Labview</i></li>
      <li> In the <i>Help</i> tab, select <i>Find instrument drivers</i></li>
![image](https://github.com/MAILFERT-Sebastien/LABVIEW-Useful-codes/tree/main/Device_control/Spectrometer_USB4000/Images/Labview_OceanOptics_Drivers_0.png)
      <li> Select <i>Ocean Optics</i> in the list of manufacturers</li>
      <li> Click on <i>Search</i> </li>
![image](https://github.com/MAILFERT-Sebastien/LABVIEW-Useful-codes/tree/main/Device_control/Spectrometer_USB4000/Images/Labview_OceanOptics_Drivers_1.png)
      <li> Select the <i>ocean2000 Instrument Driver</i> item</li>
![image](https://github.com/MAILFERT-Sebastien/LABVIEW-Useful-codes/tree/main/Device_control/Spectrometer_USB4000/Images/Labview_OceanOptics_Drivers_2.png)
      <li> Click on <i>Install</i></li>
      <li> Log in or create a <i>NI profile</i></li>
![image](https://github.com/MAILFERT-Sebastien/LABVIEW-Useful-codes/tree/main/Device_control/Spectrometer_USB4000/Images/Labview_OceanOptics_Drivers_3.png)
      <li> Once installed, close this window</li>
![image](https://github.com/MAILFERT-Sebastien/LABVIEW-Useful-codes/tree/main/Device_control/Spectrometer_USB4000/Images/Labview_OceanOptics_Drivers_4.png)
      </ul>
      
2. Labview code

      Download the 2 Labview files AND associated folders on your desktop. Several Labview files are provided:
      <ul>
      <li> <i>231215_Spectrometer_Example_1.0.vi</i> : a first simple example to acquire continously a spectrum</li>
      <li> <i>231215_Spectrometer_Example_2.0.vi</i> : a second example where one can compute the transmission curbe of a fluorescence filter</li>
      </ul>

## Running the tests
1. 231215_Spectrometer_Example_1.0.vi
	<ul>
      	<li> Double-click on the <i>231215_Spectrometer_Example_1.0.vi</i> file</li>
      	<li> Click on the <i>Run</i> arrow to run the vi</li>
      	</ul>


2. 231215_Spectrometer_Example_2.0.vi
	<ul>
      	<li> Double-click on the <i>231215_Spectrometer_Example_2.0.vi</i> file</li>
      	<li> Click on the <i>Run</i> arrow to run the vi</li>
	<li> Measure a transmission curve</li>
      		<ul>
	      	<li> Record a noise spectrum by clicking on the Filter characterization tab named "Type of measurement" and select "Background noise"</li>
		<li> Record a spectrum of a source by selecting "Source without filter"</li>
		<li> Record a spectrum of this source through the filter by selecting "Source after the filter"</li>
		</ul>
	<li> Save the transmission curve</li>
		<ul>
	      	<li> Select a folder by clikcing on "File Path"</li>
		<li> Enter a name for the file (without the extension) in the "File name (wo extension)" box</li>
		<li> Choose the type of curve you want to save (filter transmission or source) and click on "Save"</li>
		</ul>
	<li> Display a theoretical transmission curve</li>
		<ul>
	      	<li> Select the type of filter (BandPass, LongPass, ShortPass)</li>
		<li> Set the center wavelength</li>
		<li> Set the transmission band (in nm) and the transmission (%)</li>
		</ul>
	</ul>
[![Watch the video](https://github.com/MAILFERT-Sebastien/LABVIEW-Useful-codes/tree/main/Device_control/Spectrometer_USB4000/Images/FilterTransmission_Measurement.png)](https://youtu.be/IOkKzA5c4gc)

## Versioning

<ul>
	<li> V1.0, 240104: Simple example</li>
	<li> V2.0, 240104: Example that records the transmission curve of a fluorescence filter</li>
</ul>




## Authors
Sébastien MAILFERT
Institut Fresnel (Marseille, France), CNRS, AMU

## Licence
GNU General Public License v3.0
GNU GPLv3.0

## Acknowledgments
Sébastien MAILFERT
