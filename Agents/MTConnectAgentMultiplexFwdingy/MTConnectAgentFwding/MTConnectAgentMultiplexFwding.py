#!/usr/bin/env python2.7
# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. 

import time
import os
import sys
import argparse
import traceback
from os.path import splitext, abspath
from sys import modules
# Backend read of MTConnect agent web server
import urllib2
from multiprocessing import Process

#Http server
#import BaseHTTPServer
from BaseHTTPServer import HTTPServer, BaseHTTPRequestHandler
from SocketServer import ThreadingMixIn
import threading
from threading import Thread

# Service imports
import win32serviceutil
import win32service
import win32event
import servicemanager
import socket

# XML parsing import
try:
    import xml.etree.cElementTree as ET
except ImportError:
    import xml.etree.ElementTree as ET

# Ini Optionreader
from ConfigParser import ConfigParser  # ver.  < 3.0


####### Configuration Ini Parameters #######
HOST_NAME = '0.0.0.0' 
gPorts=[]
gDevices = []
gURLs = []
SERVICENAME = "MTConnectAgentForwarding"
SLEEPAMT = 3.0
REQUERYAMT = 10.0
debuglevel = 0
RPM = 0
rpms = []
####### Global variables #######
debugfile = open(sys.path[0] + '/debug.txt', 'w', 0)
bflag = True
bConnected = False
agentxml = '<error> no mtconnect data from back end agent! </error>'
tagdictionary = {}

# Debug logger, if 
def debug(s):
    global debuglevel
    # assumes:
    # debugfile = open(sys.path[0] + '/debug.txt', 'w', 0)
    if debuglevel > 0:
        debugfile.write(time.asctime() + s)

def get_with_default(section,name,default):
    global config
    try:
        return config.get(section,name)
    except :
        return default

def doConfig():
    global tagdictionary
    global config
    global SERVICENAME,SLEEPAMT,REQUERYAMT,RPM,RPMTAGS,rpms
    global gDevices, gURLs,gPorts
    try:
        config = ConfigParser()
        config.optionxform = str   
        debug(" doConfig read" + sys.path[0] + '\\' + 'Config.INI' + "\n")
        config.read(sys.path[0] + '\\' + 'Config.INI')
        for section in config.sections():
            tagdictionary[section] = {}
            for option in config.options(section):
                tagdictionary[section][option] = config.get(section, option)

        SERVICENAME = get_with_default("MTCONNECT","servicename", "MTCMultiplexFwdAgent")
        SLEEPAMT = float(get_with_default("MTCONNECT","refresh","8.0")) 

        backends = get_with_default("MTCONNECT","backends", "") 
        gDevices = [x.strip() for x in backends.split(',')]

        gURLs = []
        gPorts= []
        for device in gDevices:
            gURLs.append(get_with_default(device,"backurl","127.0.0.1:5000").strip())
            gPorts.append(int(get_with_default(device,"fwdport","5000").strip()))
    
        for URL in gURLs:
            URL = URL.replace("http://","") 
            index = URL.find("/current")
            if(index >= 0): URL = URL[:index]
  
        debuglevel = int(get_with_default("MTCONNECT","debuglevel", "0")) 
        RPM = int(get_with_default("MTCONNECT","RPM", "0")) 
        RPMTAGS = get_with_default("MTCONNECT","RPMTAGS", "Srpm") 
        rpms = RPMTAGS.split(",")
         #REQUERYAMT=tagdictionary["MTCONNECT"]["refresh"]
        # Check legal URL - strip off leading http:// and trailing /current
        
        debug( " BACKENDS=" + backends + "\n")
        debug( " SERVICENAME=" + SERVICENAME + "\n")
        debug(" SLEEPAMT=" + str(SLEEPAMT) + "\n")
        debug( " debuglevel=" + str(debuglevel) + "\n")
    except:
        debugfile.write(time.asctime() + " doConfig exception\n")
 
# https://wiki.python.org/moin/BaseHttpServer
def do_XML(xmlstr):
    try:
        #  avoid the ns0 prefix the default namespace should be set before
        #  reading the XML data
        ET.register_namespace('xsi', "http://www.w3.org/2001/XMLSchema-instance")
        ET.register_namespace('', "urn:mtconnect.org:MTConnectStreams:1.1")
        ET.register_namespace('', "urn:mtconnect.org:MTConnectStreams:1.2")
        ET.register_namespace('', "urn:mtconnect.org:MTConnectStreams:1.3")
        ET.register_namespace('x', "urn:mazakusa.com:MazakStreams:1.2")
        
        # Parse xml from string
        root = ET.fromstring(xmlstr)
        # Find all rpm assume max is value- if none, use 0.0
        srpm = []
        d = [0.0]
        srpm.extend(root.findall(".//*[@name='%s']" % rpms[0]))
        try:
            srpm.extend(root.findall(".//*[@name='%s']" % rpms[1]))
            srpm.extend(root.findall(".//*[@name='%s']" % rpms[2]))
            srpm.extend(root.findall(".//*[@name='%s']" % rpms[3]))
            srpm.extend(root.findall(".//*[@name='%s']" % rpms[4]))
        except Exception:
            pass
        for r in srpm:
            if r.text != "UNAVAILABLE" :
                d.extend([float(r.text)])
        bestd = max(d)
        srpmnew = root.find(".//*[@name='%s']" % rpms[0])
        srpmnew.text = str(bestd)

        # Write xml to string - FIXME: should only write if changed
        newxmlstr = ET.tostring(root, encoding='utf8', method='xml')
        newxmlstr = newxmlstr.replace("'1.0'", "\"1.0\"")
        newxmlstr = newxmlstr.replace("'utf8'", "\"UTF-8\"")
        # Syntax checked out with
        # http://www.w3schools.com/xml/xml_validator.asp
        return newxmlstr.strip()
    except:
        debug( " do_XML exception\n")
        return xmlstr

def do_UPDATE():
    global agentxml,bConnected
    global URL, URLPORT
    try:
        xmlwebsite = "http://" + gURL + "/current"
        toursurl = urllib2.urlopen(xmlwebsite)
        str = toursurl.read()

        # Perform tag name substitutions
        section = 'TAGS'
        for option in tagdictionary[section].keys():
            str = str.replace('name="{}"'.format(option), 'name="{}"'.format(tagdictionary[section][option]))

        # Fixme: this could be a better substitution
        section = 'ENUMS'
        for option in tagdictionary[section].keys():
            str = str.replace('{}'.format(option), '{}'.format(tagdictionary[section][option]))

        # Perform tag dataIds substitutions
        section = 'DATAIDS'
        for option in tagdictionary[section].keys():
            str = str.replace('dataItemId="{}"'.format(option), 'dataItemId="{}"'.format(tagdictionary[section][option]))

        # Perform XML gyrations
        if(RPM > 0):
            str = do_XML(str)
 
        # Update current agent string- note, probe other queries not supported
        lock.acquire()
        agentxml = str
        lock.release()
        bConnected = True 
        #raise TypeError("Again !?!")
        #debugfile.writelines(sys.exc_info()[0])
    except Exception:
        debug( " MTConnect update exception\n")
        var = traceback.format_exc()
        if(debuglevel > 0):
            debugfile.write(time.asctime() + var)
        bConnected = False 

class quietHTTPServer(BaseHTTPRequestHandler):
    def log_request(self, code): pass

# http://stackoverflow.com/questions/8403857/python-basehttpserver-not-serving-requests-properly
def MakeHandlerClassFromArgv(URL):
    class CustomHandler(quietHTTPServer, object):
        str=''
        URL=''
        xmlwebsite=''
        def __init__(self, *args, **kwargs):
             self.URL=URL
             self.str=''
             super(CustomHandler, self).__init__(*args, **kwargs)
        def do_GET(self):
            global debuglevel
            try:
                self.xmlwebsite = "http://" + self.URL + "/current"                                                                                                                                            
                toursurl = urllib2.urlopen(self.xmlwebsite)
                self.str = toursurl.read()
                """Respond to a GET request."""
                self.send_response(200)
                self.send_header("Content-type", "text/xml")
                self.end_headers()
                self.wfile.write(self.str)
                self.wfile.close()

            except Exception, x:
                debug(  " do_GET exception:%s\n" % x)
                debug(  " xmlwebsite=%s\n" % self.xmlwebsite)
                debug(  " str=%s\n" % self.str[0:100])
                var = traceback.format_exc()
                debug( var)
    return CustomHandler

class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    def process_request(self, request, client_address):
        thread = threading.Thread(target=self.__new_request, args=(self.RequestHandlerClass, request, client_address, self))
        thread.start()
    def __new_request(self, handlerClass, request, address, server):
        handlerClass(request, address, server)
        self.shutdown_request(request)
 
class Thread(threading.Thread):
    i=0
    def __init__(self, i):
        threading.Thread.__init__(self)
        self.i=i
        threading.Thread.setDaemon(self,True)
        self.start()

    def run(self):
        httpd = ThreadedHTTPServer((HOST_NAME, int(gPorts[self.i])), MakeHandlerClassFromArgv(gURLs[self.i]))
        httpd.serve_forever()

def MyMain():
    global bflag,gURLs,gPorts
    threads=[]
    results = []
    servers=[]
    for i,URL in enumerate(gURLs):
        t=Thread(i)

    try:
        while bflag:
            time.sleep(SLEEPAMT)
    except KeyboardInterrupt:
        bflag = False


class Service(win32serviceutil.ServiceFramework):
    global SERVICENAME
    _svc_name_ = SERVICENAME
    _svc_display_name_ = SERVICENAME
    _svc_description_ = "Echo MTConnect XML data stream"
    def __init__(self,args):
        debugfile.write(time.asctime() + " Server enter __init__ \n")
        win32serviceutil.ServiceFramework.__init__(self,args)
        self.hWaitStop = win32event.CreateEvent(None,0,0,None)
        
        socket.setdefaulttimeout(60)
        # This is how long the service will wait to run / refresh itself (see
        # script below)
        self.timeout = 30000     #30 seconds
        self.stop_event = win32event.CreateEvent(None, 0, 0, None)

    def log(self, msg):
        #import servicemanager
        #servicemanager.LogInfoMsg(str(msg))
        debugfile.write(time.asctime() + msg + "\n")
    def sleep(self, sec):
        win32api.Sleep(sec * 1000, True)
    def SvcStop(self):
        self.ReportServiceStatus(win32service.SERVICE_STOP_PENDING)
        self.stop()
        win32event.SetEvent(self.stop_event)
        self.ReportServiceStatus(win32service.SERVICE_STOPPED)

    def SvcDoRun(self):
        self.ReportServiceStatus(win32service.SERVICE_START_PENDING)
        try:
            self.ReportServiceStatus(win32service.SERVICE_RUNNING)
            self.start()
            win32event.WaitForSingleObject(self.stop_event, win32event.INFINITE)
        except Exception, x:
            self.log('Exception : %s' % x)
            self.SvcStop()
    def start(self):
        MyMain()
        self.runflag = True
        while self.runflag:
            self.sleep(10)
    def stop(self):
        global bflag
        self.runflag = False
        bflag = False

if __name__ == '__main__':
    doConfig()
    if len(sys.argv) == 1:
        servicemanager.Initialize()
        servicemanager.PrepareToHostSingle(Service)
        servicemanager.StartServiceCtrlDispatcher()
    elif len(sys.argv) > 1 and (sys.argv[1:2][0] == 'debug' or sys.argv[1:2][0] == 'run'):
        MyMain()
    else:
        win32serviceutil.HandleCommandLine(Service)
