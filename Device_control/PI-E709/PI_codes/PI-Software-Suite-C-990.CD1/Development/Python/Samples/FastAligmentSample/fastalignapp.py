#!/usr/bin/python
# -*- coding: utf-8 -*-
"""Define and start fast align routines and query results interactively."""

from __future__ import print_function

from collections import deque, OrderedDict
from json import load as jsonload
from time import sleep

try:
    import matplotlib.pyplot as plt
except ImportError:
    print('WARNING: run "pip install matplotlib" to show plots of the scans')

from pipython import GCSDevice, GCSError, gcserror
from pipython.fastaligntools import ResultID
from pipython.datarectools import RecordOptions, TriggerSources
from pipython.pitools import itemstostr, savegcsarray

# Invalid class name pylint: disable=C0103
# Redefining built-in pylint: disable=W0622
try:
    input = raw_input
except NameError:  # Python 3
    pass

try:
    basestring
except NameError:  # Python 3
    basestring = str

CONFIGFILE = r'./fastalignapp.json'  # full path to file with the system configuration
DATRECFILE = r'./fastalignappexport.dat'  # full path to file where to save the data recorder output

MENU_HEADER = '''\n----------------------------------------------------------------------
Fast Alignment Sample Application
Physik Instrumente (PI) SE & Co. KG\n'''

MENU_MAIN = OrderedDict([  # menu text and according callee
    ('select_device', 'Select active device'),
    ('manage_routines', 'List/edit/add routines'),
    ('change_routinetype', 'Change routine type'),
    ('start_single_routine', 'Start single routine with data recorder'),
    ('start_multiple_routines', 'Start multiple routines without data recorder'),
    ('show_intensity_values', 'Show intensity values'),
    ('show_menu_moveaxes', 'Move axes'),
])

MENU_SINGLERUN = OrderedDict([  # menu text and according callee
    ('refresh_status', 'Refresh status'),
    ('stop_routines', 'Stop running routines'),
    ('show_results', 'Show scan results'),
    ('save_scandata', 'Save scan data'),
    ('show_scandata', 'Show scan data'),
])

MENU_MULTIRUN = OrderedDict([  # menu text and according callee
    ('refresh_status', 'Refresh status'),
    ('stop_routines', 'Stop running routines'),
    ('show_results', 'Show scan results'),
])

MENU_MOVEAXIS = OrderedDict([  # menu text and according callee
    ('move_axis_absolute', 'Move axis absolute'),
    ('move_axis_relative', 'Move axis relative'),
    ('show_positions', 'Show current positions'),
])


class RoutineItem(object):
    """One item of a scan command, i.e. an (optional) label and a value."""

    def __init__(self, descr, label):
        """Create a routine item.
        @param descr : Textual description, which is shown in user interaction.
        @param label : String that is used in GCS command, e.g. "ML" for stop level or empty string.
        """
        assert isinstance(descr, basestring), 'descr must be string'
        assert isinstance(label, basestring), 'label must be string'
        self.descr = descr
        self.label = label.upper()
        self._value = None

    def __str__(self):
        """Return string for GCS command."""
        if self._value:
            if self.label:
                return ' %s %s' % (self.label, self._value)
            return ' %s' % self._value
        return ''

    @property
    def value(self):
        """Return value as string or None if not defined."""
        return self._value

    @value.setter
    def value(self, value):
        """Set value as string or None."""
        if value is not None:
            assert isinstance(value, basestring), 'value must be string'
        self._value = value

    @property
    def valuestr(self):
        """Return value as string or empty string if not defined."""
        if self._value is None:
            return ''
        return self._value


class FARoutine(object):  # Too many instance attributes pylint: disable=R0902
    """Provide properties of a fast alignment scan routine."""

    PROPERTIES = {  # routine item label as UPPERCASE string and according property name
        'FDR': OrderedDict([
            ('1', 'name'),
            ('2', 'scanaxis'),
            ('3', 'scanrange'),
            ('4', 'stepaxis'),
            ('5', 'steprange'),
            ('L', 'threshold'),
            ('A', 'aligninputchannel'),
            ('F', 'frequency'),
            ('V', 'velocity'),
            ('MP1', 'scanmiddlepos'),
            ('MP2', 'stepmiddlepos'),
            ('TT', 'targettype'),
            ('CM', 'estimationmethod'),
            ('MIIL', 'mininput'),
            ('MAIL', 'maxinput'),
            ('ST', 'stopoption'),
        ]),
        'FDG': OrderedDict([
            ('1', 'name'),
            ('2', 'scanaxis'),
            ('3', 'stepaxis'),
            ('ML', 'minlevel'),
            ('A', 'aligninputchannel'),
            ('MIA', 'minampl'),
            ('MAA', 'maxampl'),
            ('F', 'frequency'),
            ('SP', 'speedfactor'),
            ('V', 'velocity'),
            ('MDC', 'maxdirectionchanges'),
            ('SPO', 'speedoffset'),
        ]),
        'FDL': OrderedDict([
            ('1', 'name'),
            ('2', 'scanaxis'),
            ('3', 'scanrange'),
            ('4', 'stepaxis'),
            ('5', 'steprange'),
            ('S', 'thresholdsigma'),
            ('C', 'thresholdcalculation'),
            ('A', 'aligninputchannel'),
            ('MT', 'maximumsearchtime'),
            ('MP1', 'scanmiddlepos'),
            ('MP2', 'stepmiddlepos'),
            ('CP1', 'scancalculationpos'),
            ('CP2', 'stepcalculationpos'),  
        ]),
    }

    KEEPVALUES = (  # when routine type is changed these values will not be reset
        'name', 'scanaxis', 'stepaxis', 'aligninputchannel', 'frequency', 'velocity',
    )

    def __init__(self, commandstring):
        """Create a scan routine object from 'commandstring'.
        @param commandstring : Full GCS command as string.
        """
        items = deque(commandstring.split())  # deque supports popleft()
        self._scantype = items.popleft().upper()
        self._scanprops = self.PROPERTIES[self._scantype]
        self.name = RoutineItem('Routine name', label='')
        self.scanaxis = RoutineItem('Scan axis', label='')
        self.scanrange = RoutineItem('Scan axis range', label='')
        self.stepaxis = RoutineItem('Step axis', label='')
        self.steprange = RoutineItem('Step axis range', label='')
        self.aligninputchannel = RoutineItem('Alignment signal input channel', label='A')
        self.minlevel = RoutineItem('Stop level', label='ML')
        self.minampl = RoutineItem('Minimum radius', label='MIA')
        self.maxampl = RoutineItem('Maximum radius', label='MAA')
        self.frequency = RoutineItem('Frequency', label='F')
        self.speedfactor = RoutineItem('Speed factor', label='SP')
        self.velocity = RoutineItem('Velocity', label='V')
        self.maxdirectionchanges = RoutineItem('Max direction changes', label='MDC')
        self.maximumsearchtime = RoutineItem('Max search time', label='MT')
        self.speedoffset = RoutineItem('Speed offset', label='SPO')
        self.threshold = RoutineItem('Threshold level', label='L')
        self.thresholdcalculation = RoutineItem('Threshold calculation flag', label='C')
        self.thresholdsigma = RoutineItem('Threshold sigma multiplicator', label='S')
        self.scanmiddlepos = RoutineItem('Scan axis middle position', label='MP1')
        self.stepmiddlepos = RoutineItem('Step axis middle position', label='MP2')
        self.scancalculationpos = RoutineItem('Scan axis calculation position', label='CP1')
        self.stepcalculationpos = RoutineItem('Step axis calculation position', label='CP2')
        self.targettype = RoutineItem('Target Type', label='TT')
        self.estimationmethod = RoutineItem('Centroid method', label='CM')
        self.mininput = RoutineItem('Minimum relative fast alignment input level', label='MIIL')
        self.maxinput = RoutineItem('Maximum relative fast alignment input level', label='MAIL')
        self.stopoption = RoutineItem('Stop position option', label='ST')
        self.name.value = items.popleft()
        if self._scantype == 'FDR' or self._scantype == 'FDL':
            self.scanaxis.value = items.popleft()
            self.scanrange.value = items.popleft()
            self.stepaxis.value = items.popleft()
            self.steprange.value = items.popleft()
        elif self._scantype == 'FDG':
            self.scanaxis.value = items.popleft()
            self.stepaxis.value = items.popleft()
        else:
            raise ValueError('unknown routine name %r' % self._scantype)
        for i in range(0, len(items), 2):
            for label, scanprop in self._scanprops.items():
                if label == items[i].upper():
                    getattr(self, scanprop).value = items[i + 1]

    def __str__(self):
        """Return GCS command string of fast alignment routine."""
        cmdstr = self._scantype
        for scanprop in self._scanprops.values():
            cmdstr += str(getattr(self, scanprop))
        return cmdstr

    @property
    def scantype(self):
        """Return current scan type as upper case string, e.g. "FDG", "FDR" or "FDL"."""
        return self._scantype

    def userinput(self):
        """Query user input interactively to change scan definition values."""
        for scanprop in self._scanprops.values():
            userinput = input('%s (%s): ' % (getattr(self, scanprop).descr, getattr(self, scanprop).valuestr))
            userinput = userinput or getattr(self, scanprop).valuestr
            if scanprop in ('name', 'scanaxis', 'scanrange', 'stepaxis', 'steprange'):
                assert userinput.strip(), '%s is empty' % scanprop
            getattr(self, scanprop).value = userinput

    def changetype(self):
        """Change scan type interactively and reset specific properties values."""
        userinput = input('New scan type: ')
        userinput = userinput.upper()
        assert userinput in list(self.PROPERTIES.keys()), 'invalid scan type %r' % userinput
        for scanprop in self._scanprops.values():
            if scanprop not in self.KEEPVALUES:
                getattr(self, scanprop).value = None
        self._scantype = userinput
        self._scanprops = self.PROPERTIES[self._scantype]
        self.userinput()


class FADevice(object):
    """Hold configuration and connection of a GCS device for fast alignment."""

    def __init__(self, config):
        """Store 'config' and initialize GCS device.
        @param config : System configuration as dictionary.
        """
        self._cfg = config
        self._routines = []
        for routine in self.cfg['ScanRoutines']:
            self._routines.append(FARoutine(routine))
        self._gcs = GCSDevice()
        self._idn = None
        self._do_connect = self.cfg['DoConnect']

    @property
    def gcs(self):
        """Access to GCS device."""
        return self._gcs
    
    @property
    def do_connect(self):
        """Connect to this device"""
        return self._do_connect

    @property
    def cfg(self):
        """Access to system configuration."""
        return self._cfg

    @property
    def routines(self):
        """List of fast align routines as FARoutine objects."""
        return self._routines

    @property
    def idn(self):
        """Textual description of device."""
        if self.gcs.gcsdevice.connected:
            if self._idn is None:
                self._idn = self.gcs.qIDN().split(',')[1].strip()
                # 0X0D000700: Device name, 0X0D001000: Customer device name
                if self.gcs.getparam(0X0D000700) is not None:  # controller does support this parameter
                    self._idn += ' %s %s' % (self.gcs.getparam(0X0D000700), self.gcs.getparam(0X0D001000))
            return self._idn
        return 'not connected'

    def connect(self):
        if self._do_connect:
            try:
                """Connect GCS device according to given system configuration and update all scan routines."""
                if self.cfg['CommunicationType'].upper() == 'TCPIP':
                    self.gcs.ConnectTCPIP(self.cfg['IPAdress'], self.cfg['IPPort'])
                elif self.cfg['CommunicationType'].upper() == 'USB':
                    self.gcs.ConnectUSB(self.cfg['USBSerialNr'])
                elif self.cfg['CommunicationType'].upper() == 'RS232':
                    self.gcs.ConnectRS232(self.cfg['ComPortNr'], self.cfg['ComPortBaudrate'])
                else:
                    raise ValueError('unknown communication type %r' % self.cfg['CommunicationType'])
                print('Connected: %s via %s' % (self.idn, self.cfg['CommunicationType']))
                for routine in self._routines:
                    self.send_routine(routine)
                self.update_routines()
            except:
                print('Connection failed: %s via %s' % (self.idn, self.cfg['CommunicationType']))

    def send_routine(self, routine):
        """Send 'routine' to controller.
        @type routine : FARoutine
        """
        try:
            if routine.scantype == 'FDR':
                self.gcs.FDR(routinename=routine.name.value, scanaxis=routine.scanaxis.value,
                            scanrange=routine.scanrange.value, stepaxis=routine.stepaxis.value,
                            steprange=routine.steprange.value, threshold=routine.threshold.value,
                            aligninputchannel=routine.aligninputchannel.value, frequency=routine.frequency.value,
                            velocity=routine.velocity.value, scanmiddlepos=routine.scanmiddlepos.value,
                            stepmiddlepos=routine.stepmiddlepos.value, targettype=routine.targettype.value,
                            estimationmethod=routine.estimationmethod.value, mininput=routine.mininput.value,
                            maxinput=routine.maxinput.value, stopoption=routine.stopoption.value)
            if routine.scantype == 'FDG':
                self.gcs.FDG(routinename=routine.name.value, scanaxis=routine.scanaxis.value,
                            stepaxis=routine.stepaxis.value, minlevel=routine.minlevel.value,
                            aligninputchannel=routine.aligninputchannel.value, minampl=routine.minampl.value,
                            maxampl=routine.maxampl.value, frequency=routine.frequency.value,
                            speedfactor=routine.speedfactor.value, maxvelocity=routine.velocity.value,
                            maxdirectionchanges=routine.maxdirectionchanges.value, speedoffset=routine.speedoffset.value)
            if routine.scantype == 'FDL':    
                self.gcs.FDL(routinename=routine.name.value, scanaxis=routine.scanaxis.value,
                            scanrange=routine.scanrange.value, stepaxis=routine.stepaxis.value,
                            steprange=routine.steprange.value, thresholdsigma=routine.thresholdsigma.value,
                            thresholdcalculation=routine.thresholdcalculation.value, aligninputchannel=routine.aligninputchannel.value,
                            maximumsearchtime=routine.maximumsearchtime.value, scanmiddlepos=routine.scanmiddlepos.value,
                            stepmiddlepos=routine.stepmiddlepos.value, scancalculationpos=routine.scancalculationpos.value,
                            stepcalculationpos=routine.stepcalculationpos.value)
        except:
            print('Sending routine %s to %s failed. ' % (routine.scantype, self.idn))

    def update_routines(self, routine=None):
        """Send 'routine' to controller and read back all routines and update self._cfg accordingly.
        @type routine : FARoutine
        """
        if self.gcs.gcsdevice.connected:
            self._routines = []
            if routine:
                print('\n%s send: %s' % (self.idn, routine))
                self.send_routine(routine)
            try:
                results = self.gcs.qFRR()
                for routineid in results:
                    cmdstring = ' '.join(results[routineid][ResultID.routine_definition])
                    self._routines.append(FARoutine(cmdstring))
            except:
                print('\nqFRR failed for %s. Controller not supported for FA.' % (self.idn))

class Application(object):
    """Control several fast alignment devices interactively."""

    def __init__(self, configfile):
        """Read 'configfile' and initialize GCS devices accordingly.
        @param configfile : Full path to configuration JSON file as string.
        """
        with open(configfile, 'rb') as fobj:
            configs = jsonload(fobj)
        self._devices = []
        for config in configs:
            self._devices.append(FADevice(config))
        self._devnum = 0
        self._running_routines = []
        self._state = GCSError(gcserror.PI_CNTR_NO_ERROR_0)

    def __enter__(self):
        return self

    def __exit__(self, exctype, _excvalue, _exctraceback):
        self.close()

    @property
    def state(self):
        """Return last error state from buffer and reset buffer to "no error"."""
        state = self._state
        self._state = GCSError(gcserror.PI_CNTR_NO_ERROR_0)
        return state

    @property
    def dev(self):
        """Access to current FADevice instance."""
        return self._devices[self._devnum]

    def connect(self):
        """Connect all GCS devices according to given system configuration."""
        for device in self._devices:
            device.connect()

    def close(self):
        """Close connection to all GCS devices."""
        for device in self._devices:
            if device.gcs.IsConnected():
                print('Close %s connection to %s' % (device.cfg['CommunicationType'], device.idn))
                device.gcs.close()

    def _get_user_input(self, iterable, message=''):
        """Show menu of 'iterable' on screen and return user input.
        @param iterable : Iterable with strings which are shown on screen.
        @param message : Optional message to show above the menu.
        @return : User input which is the index of the according 'iterable' as integer or None for "go back".
        """
        print(MENU_HEADER)
        print('Current connection: %s' % self.dev.idn)
        print('Status: %s' % self.state)
        print()
        if message:
            print('%s:' % message)
        for i, _ in enumerate(iterable):
            print('%3d - %s' % (i, list(iterable)[i]))
        print('%3d - Go back' % len(iterable))
        print()
        userinput = input('--> ')
        print()
        try:
            userinput = int(userinput)
        except ValueError:
            return None
        if userinput not in range(len(iterable)):
            return None
        return userinput

    def execute(self, menu, message=''):
        """Show menu on screen and execute a method according to the user input.
        @param menu : Name of dictionary that describes the menu and the methods to call.
        @param message : Optional message to show above the menu.
        @return : Index of chosen menu item as integer or None to go back.
        """
        userinput = self._get_user_input(menu.values(), message)
        if userinput is not None:
            try:
                getattr(self, '_%s' % list(menu.keys())[userinput])()
            except GCSError as exc:
                self._state = GCSError(exc)
        return userinput

    def _select_device(self):
        """List all available devices and select the active device interactively."""
        devnames = [device.idn for device in self._devices]
        userinput = self._get_user_input(devnames, message='Select device')
        if userinput is None:
            return
        self._devnum = userinput
        if not self._devices[self._devnum].gcs.gcsdevice.connected:
            self._devices[self._devnum].connect()

    def _manage_routines(self):
        """List routines of active device and select the routine to edit interactively."""
        userinput = self._get_user_input(self.dev.routines, message='Select routine to edit, new name to add routine')
        if userinput is None:
            return
        routine = self.dev.routines[userinput]
        routine.userinput()
        self.dev.update_routines(routine)

    def _change_routinetype(self):
        """List routines of active device and select the routine to change its type interactively."""
        userinput = self._get_user_input(self.dev.routines, message='Select routine to change type')
        if userinput is None:
            return
        routine = self.dev.routines[userinput]
        routine.changetype()
        self.dev.update_routines(routine)

    def _start_single_routine(self):
        """List routines of active device and select the routine to start interactively."""
        userinput = self._get_user_input(self.dev.routines, message='Select routine to start')
        if userinput is None:
            return
        routine = self.dev.routines[userinput]
        self.dev.gcs.RTR(1)
        self.dev.gcs.DRT(tables=1, sources=TriggerSources.POSITION_CHANGING_COMMAND_1, values=0)  # 'values' is a dummy
        self.dev.gcs.DRC(tables=[1, 2, 3],
                         sources=[routine.scanaxis.value, routine.stepaxis.value, routine.aligninputchannel.value],
                         options=[RecordOptions.ACTUAL_POSITION_2, RecordOptions.ACTUAL_POSITION_2,
                                  RecordOptions.FAST_ALIGNMENT_INPUT_CHANNEL_150])
        self.dev.gcs.FRS(routine.name.value)
        self._running_routines = [routine]
        self._routines_running(MENU_SINGLERUN)

    def _routines_running(self, menu):
        """Show which routines are running and a menu to select further action."""
        while True:
            running = []
            for routine in self._running_routines:
                if self.dev.gcs.qFRP(routine.name.value)[routine.name.value] == 2:
                    running.append(routine.name.valuestr)
            print('Current routines running: %s' % ' '.join(running))
            if self.execute(menu) is None:
                return

    def _start_multiple_routines(self):
        """Ask user to select the routines to start."""
        self._running_routines = []
        routinenames = input('Enter routine names to start, separated by space --> ')
        for routinename in routinenames.split():
            for routine in self.dev.routines:
                if routine.name.valuestr.upper() == routinename.upper():
                    self._running_routines.append(routine)
                    self.dev.gcs.FRS(routine.name.value)
        self._routines_running(MENU_MULTIRUN)

    @staticmethod
    def _refresh_status():
        """Dummy function, the menu will update the status itself."""
        pass

    def _stop_routines(self):
        """Stop all running routines on active device."""
        for routinename, state in self.dev.gcs.qFRP().items():
            if state:
                self.dev.gcs.FRP(routinename, 0)

    def _show_results(self):
        """Show results of routines that have been started by this application."""
        result = self.dev.gcs.qFRR()
        for routine in self._running_routines:
            print('Routine: %s' % routine.name.valuestr)
            for resultid in (ResultID.success, ResultID.max_value, ResultID.max_position, ResultID.scan_time,
                             ResultID.reason_of_abort):
                print('%s: %s' % (ResultID.name[resultid], itemstostr(result[routine.name.valuestr][resultid])))
            print()

    def _read_data_recorder(self):
        """Read out data recorder of active device according to current scan time.
        @return : Tuple of (header, data) read out from data recorder.
        """
        routinename = self._running_routines[0].name.value
        scantime = float(self.dev.gcs.qFRR(routinename, ResultID.scan_time)[routinename][ResultID.scan_time])
        servotime = self.dev.gcs.qSPA(1, 0xe000200)[1][0xe000200]
        recrate = self.dev.gcs.qRTR()
        datareclength = int((scantime + 0.1) / servotime * recrate)  # scantime + 100 ms
        print('Read %d data sets from controller...' % datareclength)
        header = self.dev.gcs.qDRR(tables=[1, 2, 3], offset=1, numvalues=datareclength)
        while self.dev.gcs.bufstate is not True:
            sleep(0.1)
        return header, self.dev.gcs.bufdata

    def _save_scandata(self):
        """Read out data recorder of active device and save a GCSArray file."""
        header, data = self._read_data_recorder()
        print('Save data to %r' % DATRECFILE)
        savegcsarray(DATRECFILE, header, data)

    def _show_scandata(self):
        """Read out data recorder and show 3D plot. Plot window must be closed interactively."""
        _header, data = self._read_data_recorder()
        fig = plt.figure()
        subplot = fig.add_subplot(projection='3d')
        subplot.scatter(data[0], data[1], zs=data[2], c=data[2])
        subplot.set_xlabel('Position of axis %s' % self._running_routines[0].scanaxis.valuestr)
        subplot.set_ylabel('Position of axis %s' % self._running_routines[0].stepaxis.valuestr)
        subplot.set_zlabel('Signal intensity %s' % self._running_routines[0].aligninputchannel.valuestr)
        plt.show()

    def _show_intensity_values(self):
        """Show current intensity values of active device."""
        print('Current intensity values:')
        for key, value in self.dev.gcs.qTCI().items():
            print('%s: %s' % (key, value))

    def _show_menu_moveaxes(self):
        """Ask user to select the type of motion."""
        self.execute(MENU_MOVEAXIS, message='Select motion type')

    def _move_axis_absolute(self):
        """Query target positions from user for all axes and move them seperately."""
        for axis, position in self.dev.gcs.qPOS().items():
            userinput = input('%s (%s): ' % (axis, position))
            if userinput:
                self.dev.gcs.SVO(axis, True)
                self.dev.gcs.MOV(axis, userinput)
        print()
        self._show_positions()

    def _move_axis_relative(self):
        """Query axis and stepsize and show menu for user to select the direction of the stepwise motion."""
        axis = input('Axis to move: ')
        stepsize = float(input('Step size: '))
        while True:
            userinput = self._get_user_input(('Positive step', 'Negative step'), 'Select direction')
            if userinput is None:
                return
            self.dev.gcs.SVO(axis, True)
            if userinput == 0:
                self.dev.gcs.MVR(axis, stepsize)
            else:
                self.dev.gcs.MVR(axis, -stepsize)
            print('Current position of axis %s: %s' % (axis, self.dev.gcs.qPOS(axis)[axis]))

    def _show_positions(self):
        """Show positions of all connected axes of active device."""
        print('Current positions:')
        for axis, position in self.dev.gcs.qPOS().items():
            print('%s: %s' % (axis, position))


def main():
    """Connect all fast alignment systems and control them interactively."""
    with Application(CONFIGFILE) as app:
        app.connect()
        while True:
            if app.execute(MENU_MAIN) is None:
                break


if __name__ == '__main__':
    # import logging
    # logging.basicConfig(level=logging.DEBUG)
    main()
