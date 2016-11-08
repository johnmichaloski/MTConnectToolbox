# Usage:
# service.exe install
# service.exe start
# service.exe stop
# service.exe remove

# you can see output of this program running python\site - packages\win32\lib\win32traceutil

# setup.py
#from distutils.core import setup
#import py2exe
#
#class Target:
#    def __init__(self, **kw):
#        self.__dict__.update(kw)
#        # for the versioninfo resources
#        self.version = "0.5.0"
#        self.company_name = "No Company"
#        self.copyright = "no copyright"
#        self.name = "py2exe sample files"
#
#
#myservice = Target(
#    description = 'foo',
#    modules = ['service'],
#    cmdline_style='pywin32'
#)
#
#setup(
#    options = {"py2exe": {"compressed": 1, "bundle_files": 1} },
#    console=["service.py"],
#    zipfile = None,
#    service=[myservice]
#)

import win32service
import win32serviceutil
import win32event
import win32evtlogutil
import win32traceutil
import time
import sys

class aservice(win32serviceutil.ServiceFramework):
    _svc_name_ = "aservice"
    _svc_display_name_ = "aservice - It Does nothing"
    _svc_deps_ = ["EventLog"]

    def __init__(self, args):
        win32serviceutil.ServiceFramework.__init__(self, args)
        self.hWaitStop = win32event.CreateEvent(None, 0, 0, None)

    self.isAlive = True

    def SvcStop(self):
        # tell Service Manager we are trying to stop (required)
        self.ReportServiceStatus(win32service.SERVICE_STOP_PENDING)

        # write a message in the SM (optional)
        # import servicemanager
        # servicemanager.LogInfoMsg("aservice - Recieved stop signal")

        # set the event to call
        win32event.SetEvent(self.hWaitStop)

        self.isAlive = False

    def SvcDoRun(self):
        import servicemanager
        # Write a 'started' event to the event log... (not required)
        #

        win32evtlogutil.ReportEvent(self._svc_name_, servicemanager.PYS_SERVICE_STARTED, 0,servicemanager.EVENTLOG_INFORMATION_TYPE, (self._svc_name_, ''))

        # methode 1: wait for beeing stopped ...
        # win32event.WaitForSingleObject(self.hWaitStop,win32event.INFINITE)

        # methode 2: wait for beeing stopped ...
        self.timeout = 1000  # In milliseconds (update every second)

        while self.isAlive:

            # wait for service stop signal, if timeout, loop again
            rc = win32event.WaitForSingleObject(self.hWaitStop, self.timeout)

            print "looping"

        # and write a 'stopped' event to the event log (not required)
        #
        win32evtlogutil.ReportEvent(self._svc_name_, servicemanager.PYS_SERVICE_STOPPED, 0, servicemanager.EVENTLOG_INFORMATION_TYPE, (self._svc_name_, ''))

        self.ReportServiceStatus(win32service.SERVICE_STOPPED)

        return

if __name__ == '__main__':
    win32serviceutil.HandleCommandLine(aservice)