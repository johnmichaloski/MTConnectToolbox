#!/usr/bin/env python2.7
#
# This software was produced by NIST, an agency of the U.S. government,
# and by statute is not subject to copyright in the United States.
# Recipients of this software assume all responsibilities associated
# with its operation, modification and maintenance. 

import time
import os,sys
import argparse
import traceback
from os.path import splitext, abspath
from sys import modules

# Backend read of MTConnect agent web server
import urllib2

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
try:
    from configparser import ConfigParser
except ImportError:
    from ConfigParser import ConfigParser  # ver. < 3.0


####### Configuration Ini Parameters #######
HOST_NAME = '0.0.0.0' 
PORT_NUMBER = 5010
URL="agent.mtconnect.org:80"
SERVICENAME="MTConnectAgentForwarding"
SLEEPAMT=3.0
REQUERYAMT=10.0
debuglevel=0
RPM=0
rpms=[]
####### Global variables #######
dfile=open(sys.path[0]+'/debug.txt', 'w', 0)
bflag=True
bConnected=False
agentxml = '<error> no mtconnect data from back end agent! </error>'
lock=threading.Lock()
tagdictionary = {}

def get_with_default(section,name,default):
    global config
    try:
        return config.get(section,name)
    except :
        return default

def doConfig():
    global tagdictionary
    global config
    global PORT_NUMBER, URL, SERVICENAME,SLEEPAMT,REQUERYAMT,RPM,RPMTAGS,rpms
    try:
        config = ConfigParser()
        config.optionxform = str   
        dfile.write(time.asctime() + "doConfig read" + sys.path[0] + '\\' + 'Config.INI' + "\n")
        config.read(sys.path[0] + '\\' + 'Config.INI')
        for section in config.sections():
            tagdictionary[section] = {}
            for option in config.options(section):
                tagdictionary[section][option] = config.get(section, option)
        PORT_NUMBER=int(get_with_default("MTCONNECT","fwdport", "5000")) # tagdictionary["MTCONNECT"]["fwdport"])
        URL=get_with_default("MTCONNECT","backurl", "agent.mtconnect.org:80") # tagdictionary["MTCONNECT"]["backurl"]
        SERVICENAME=get_with_default("MTCONNECT","servicename", "MTConnectFwdAgent")# tagdictionary["MTCONNECT"]["servicename"]
        SLEEPAMT=float(get_with_default("MTCONNECT","refresh","8.0")) # tagdictionary["MTCONNECT"]["refresh"])
        debuglevel=int(get_with_default("MTCONNECT","debuglevel", "0")) 
        RPM=int(get_with_default("MTCONNECT","RPM", "0")) 
        RPMTAGS=get_with_default("MTCONNECT","RPMTAGS", "Srpm") 
        rpms = RPMTAGS.split(",")
         #REQUERYAMT=tagdictionary["MTCONNECT"]["refresh"]
        # Check legal URL - strip off leading http:// and trailing /current
        URL = URL.replace("http://","")
        index = URL.find("/current")
        if(index >= 0): URL = URL[:index]
        dfile.write(time.asctime() + "PORT_NUMBER=" + str(PORT_NUMBER)+"\n")
        dfile.write(time.asctime() + "URL=" + URL+"\n")
        dfile.write(time.asctime() + "SERVICENAME=" + SERVICENAME+"\n")
        dfile.write(time.asctime() + "SLEEPAMT=" + str(SLEEPAMT)+"\n")
        dfile.write(time.asctime() + "debuglevel=" + str(debuglevel)+"\n")
    except:
        dfile.write(time.asctime() + "doConfig exception\n")
 
# https://wiki.python.org/moin/BaseHttpServer

def do_XML(xmlstr):
    try:
        #  avoid the ns0 prefix the default namespace should be set before reading the XML data
        ET.register_namespace('xsi', "http://www.w3.org/2001/XMLSchema-instance")
        ET.register_namespace('', "urn:mtconnect.org:MTConnectStreams:1.1")
        ET.register_namespace('', "urn:mtconnect.org:MTConnectStreams:1.2")
        ET.register_namespace('', "urn:mtconnect.org:MTConnectStreams:1.3")
        ET.register_namespace('x', "urn:mazakusa.com:MazakStreams:1.2")
        
        # Parse xml from string
        root = ET.fromstring(xmlstr)
        # Find all rpm assume max is value- if none, use 0.0
        srpm=[]
        d=[0.0]
        srpm.extend(root.findall(".//*[@name='%s']"% rpms[0]))
        try:
            srpm.extend(root.findall(".//*[@name='%s']"% rpms[1]))
            srpm.extend(root.findall(".//*[@name='%s']"% rpms[2]))
            srpm.extend(root.findall(".//*[@name='%s']"% rpms[3]))
            srpm.extend(root.findall(".//*[@name='%s']"% rpms[4]))
        except Exception:
            pass
        for r in srpm:
            if r.text != "UNAVAILABLE" :
                d.extend([float(r.text)])
        bestd=max(d)
        srpmnew = root.find(".//*[@name='%s']"% rpms[0])
        srpmnew.text=str(bestd)

        # Write xml to string - FIXME: should only write if changed
        newxmlstr =  ET.tostring(root, encoding='utf8', method='xml')
        newxmlstr=newxmlstr.replace("'1.0'", "\"1.0\"")
        newxmlstr=newxmlstr.replace("'utf8'", "\"UTF-8\"")
        # Syntax checked out with http://www.w3schools.com/xml/xml_validator.asp
        return newxmlstr.strip()
    except:
        dfile.write(time.asctime() + "do_XML exception\n")
        return xmlstr


def do_UPDATE():
    global agentxml,bConnected
    global PORT_NUMBER, URL, URLPORT
    try:
        xmlwebsite = "http://"+ URL + "/current"
        toursurl = urllib2.urlopen(xmlwebsite)
        str = toursurl.read()

        # Perform tag name substitutions
        section ='TAGS'
        for option in tagdictionary[section].keys():
            str=str.replace('name="{}"'.format(option), 'name="{}"'.format(tagdictionary[section][option]))

        # Fixme: this could be a better substitution
        section ='ENUMS'
        for option in tagdictionary[section].keys():
            str=str.replace('{}'.format(option), '{}'.format(tagdictionary[section][option]))

        # Perform tag dataIds substitutions
        section ='DATAIDS'
        for option in tagdictionary[section].keys():
            str=str.replace('dataItemId="{}"'.format(option), 'dataItemId="{}"'.format(tagdictionary[section][option]))

        # Perform XML gyrations
        if(RPM>0):
            str=do_XML(str)
 
        # Update current agent string- note, probe other queries not supported
        lock.acquire()
        agentxml=str
        lock.release()
        bConnected=True 
        #raise TypeError("Again !?!")
        #dfile.writelines(sys.exc_info()[0])
    except Exception:
        dfile.write(time.asctime() + "MTConnect update exception\n")
        var = traceback.format_exc()
        if(debuglevel>0):
            dfile.write(time.asctime() + var)
        bConnected=False 
# http://stackoverflow.com/questions/8403857/python-basehttpserver-not-serving-requests-properly

class MyWebHandler(BaseHTTPRequestHandler):
    def do_HEAD(s):
        s.send_response(200)
        s.send_header("Content-type", "text/xml")
        s.end_headers()
    def do_GET(s):
        try:
            global agentxml
            """Respond to a GET request."""
            s.send_response(200)
            s.send_header("Content-type", "text/xml")
            s.end_headers()
            # Only handling current since there are substitutions
            #if(s.path.find("/current") >= 0) : # could bee /DeviceName/current
            lock.acquire()
            s.wfile.write(agentxml)
            s.wfile.close()
            lock.release()
        except Exception:
            dfile.write(time.asctime() + "do_GET exception\n")
            var = traceback.format_exc()
            if(debuglevel > 0):
                dfile.write(time.asctime() + var)

class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    """ This class allows to handle requests in separated threads.
        No further content needed, don't touch this. """
# goes way back: http://www.gossamer-threads.com/lists/python/bugs/278139

#def run_while_true(server_class=BaseHTTPServer.HTTPServer,
#                   handler_class=BaseHTTPServer.BaseHTTPRequestHandler):
def run_while_true():    
    """
    This assumes that keep_running() is a function of no arguments which
    is tested initially and after each request.  If its return value
    is true, the server continues.
    """
    global bConnected
    dfile.write(time.asctime() + " Server Starts - %s:%s\n" % (HOST_NAME, PORT_NUMBER))

    ServerClass = ThreadedHTTPServer((HOST_NAME, PORT_NUMBER), MyWebHandler)
 
    #ServerClass = BaseHTTPServer.HTTPServer
    #Protocol = "HTTP/1.1"
    #MyWebHandler.protocol_version = Protocol
    #httpd = ServerClass((HOST_NAME, PORT_NUMBER), MyWebHandler)

 
 #    httpd = server_class((HOST_NAME, PORT_NUMBER), MyWebHandler)
    while bflag:
        try:
            if bConnected:
                ServerClass.handle_request()
            else:
                time.sleep(REQUERYAMT)

        except urllib.URLError as e: 
            dfile.write(time.asctime() + 'cannot get %s - %s' % (szurl,e.reason)) 
        except urllib.HTTPError as e: 
            dfile.write(time.asctime() + 'cannot get %s - server reply: %d %s' % (szurl,e.code,e.reason)) 
        except (IOError,httplib.BadStatusLine,httplib.HTTPException): 
            dfile.write(time.asctime() + 'cannot get %s - connection reset by router or firewall' % szurl) 
        except socket.error as msg: 
            dfile.write(time.asctime() + 'cannot get %s - %s' % (szurl,msg)) 

    ServerClass.server_close()
    #httpd.server_close()
    dfile.write( time.asctime()+ "Server Stops - %s:%s\n" % (HOST_NAME, PORT_NUMBER))

def MyMain():
    global bflag
    t1 = Thread(target=run_while_true).start()   
    try:
        while bflag:
            do_UPDATE()
            time.sleep(SLEEPAMT)
    except KeyboardInterrupt:
        bflag=False
    t1.join()

class Service (win32serviceutil.ServiceFramework):
    global SERVICENAME
    _svc_name_ = SERVICENAME
    _svc_display_name_ = SERVICENAME
    _svc_description_ = "Echo MTConnect XML stream with modifications"
    def __init__(self,args):
        dfile.write( time.asctime()+ "Server enter __init__ \n" )
        win32serviceutil.ServiceFramework.__init__(self,args)
        self.hWaitStop = win32event.CreateEvent(None,0,0,None)
        
        socket.setdefaulttimeout(60)
        # This is how long the service will wait to run / refresh itself (see script below)
        self.timeout = 30000     #30 seconds
        self.stop_event = win32event.CreateEvent(None, 0, 0, None)

    def log(self, msg):
        #import servicemanager
        #servicemanager.LogInfoMsg(str(msg))
        dfile.write(time.asctime() + msg + "\n")
    def sleep(self, sec):
        win32api.Sleep(sec*1000, True)
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
        self.runflag=True
        while self.runflag:
            self.sleep(10)
    def stop(self):
        global bflag
        self.runflag=False
        bflag=False

if __name__ == '__main__':
    doConfig()
    if len(sys.argv) == 1:
        servicemanager.Initialize()
        servicemanager.PrepareToHostSingle(Service)
        servicemanager.StartServiceCtrlDispatcher()
    elif len(sys.argv) > 1  and (sys.argv[1:2][0] == 'debug' or sys.argv[1:2][0] == 'run'):
        MyMain()
    else:
        win32serviceutil.HandleCommandLine(Service)
