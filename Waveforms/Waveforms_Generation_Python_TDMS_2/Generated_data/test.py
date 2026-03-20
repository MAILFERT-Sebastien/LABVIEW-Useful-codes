from nptdms import TdmsFile
f = TdmsFile.read(r"C:\Users\Sebastien\Documents\GitHub\LABVIEW-Useful-codes\Waveforms\Waveforms_Generation_Python_TDMS_2\Generated_data\daq_sequence.tdms")
for group in f.groups():
    print(f"Groupe: '{group.name}'")
    for ch in group.channels():
        print(f"  Canal: '{ch.name}'")