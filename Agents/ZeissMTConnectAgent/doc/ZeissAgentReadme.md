<html>

<body lang=EN-US link=blue vlink=purple>

<div class=WordSection1>

<div style='border:none;border-bottom:solid #4F81BD 1.0pt;padding:0in 0in 4.0pt 0in'>

<p class=MsoTitle>ZEISS LOGFILE README</p>

</div>

<p class=MsoNormal>6/28/2016 3:39:13 PM</p>

<p class=MsoNormal>This document describes the data gathering for a Zeiss CMM
machine that updates a tab separated log file. Multiple adapters to Zeiss
software are possible, each contained within one MTConnect Agent.</p>

<p class=MsoNormal>The ZEISS Agent contains back end adapters that read a log
file generated from the CMM periodically (typically when an event occurs within
the CMM). The log file contains events and not samples, but all the events are
time stamped and in absolute order of occurrence.</p>

<p class=MsoNormal>The file is specified as a Window cross-platform file, so it
must contain the PC or computer name. UNC is  short for Universal Naming
Convention and specifies a Windows syntax to describe the location of a network
resource, such as a shared file, directory, or printer. The UNC syntax for
Windows systems has the generic form:</p>

<div style='border:solid #2F6FAB 1.0pt;padding:12.0pt 12.0pt 12.0pt 9.0pt;
background:#F9F9F9'>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>\\ComputerName\SharedResource</p>

</div>

<p class=MsoNormal>In our case the SharedResource is a shared file that must be
explicitly sharable. In order to use the UNC file, Microsoft File Operations:
CreateFile, ReadFile and CloseFile are used as other generic C++ file
operations did not work (but were originally tried.) UNC files on Windows seem
to require Windows specific File operations. Note, the UNC file path must be
accessible to other computers or it cannot be read.mInside the Agent are
Adapters for each UNC file. Each Adapter runs as a thread, hence the
distinction between 64 bit and 32 bit C++ solutions must be explicitly acknowledged
in installing the binary exe. That is, 32-bit MTConnect agents do not on 64 bit
platforms, although they may appear to.</p>

<p class=MsoNormal>In the Zeiss file, it logs major events in a UNC shared
file. Below is a sample of the last line found in the Zeiss shared file. </p>

<div style='border:solid #2F6FAB 1.0pt;padding:12.0pt 12.0pt 12.0pt 9.0pt;
background:#F9F9F9'>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>Measurement
Plan Name: 138Z4039-501</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>Run
Speed: 160 mm/s</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>Measurement
Start Time: 9:49:47 am</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>Measurement
End Time: 10:13:57 am</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>Duration
of Run: 00:24:21.0</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>Date of
Run: 2016-5-20 </p>

</div>

<p class=MsoNormal>Each run of a Zeiss inspection is logged, starting with a
plan name, run speed and start time. When the inspection is done the end time,
duration and date of run are logged. Unfortunately, the date of the run
(inspection) occurs after the inspection has completed, thus, no error
detection of runaway date or times can be done.  </p>

<p class=MsoNormal>There are three fields of interest, plan name, start time,
and speed event.</p>

<table class=MsoTableLightListAccent5 border=1 cellspacing=0 cellpadding=0
 style='border-collapse:collapse;border:none'>
 <tr style='height:11.7pt'>
  <td width=237 valign=top style='width:142.25pt;border-top:solid #4BACC6 1.0pt;
  border-left:solid #4BACC6 1.0pt;border-bottom:none;border-right:none;
  background:#4BACC6;padding:0in 5.4pt 0in 5.4pt;height:11.7pt'>
  <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
  text-align:center;line-height:normal;text-autospace:none'><b><span
  style='font-size:9.5pt;font-family:Consolas;color:green'>Field</span></b></p>
  </td>
  <td width=383 valign=top style='width:229.65pt;border-top:solid #4BACC6 1.0pt;
  border-left:none;border-bottom:none;border-right:solid #4BACC6 1.0pt;
  background:#4BACC6;padding:0in 5.4pt 0in 5.4pt;height:11.7pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><b><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>Example</span></b></p>
  </td>
 </tr>
 <tr style='height:11.7pt'>
  <td width=237 valign=top style='width:142.25pt;border:solid #4BACC6 1.0pt;
  border-right:none;padding:0in 5.4pt 0in 5.4pt;height:11.7pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><b><span style='font-size:9.5pt;font-family:Consolas;
  color:green'>Start timestamp</span></b></p>
  </td>
  <td width=383 valign=top style='width:229.65pt;border:solid #4BACC6 1.0pt;
  border-left:none;padding:0in 5.4pt 0in 5.4pt;height:11.7pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:green'>9:48:56 am or pm</span></p>
  </td>
 </tr>
 <tr style='height:12.35pt'>
  <td width=237 valign=top style='width:142.25pt;border:none;border-left:solid #4BACC6 1.0pt;
  padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><b><span style='font-size:9.5pt;font-family:Consolas;
  color:green'>End time</span></b></p>
  </td>
  <td width=383 valign=top style='width:229.65pt;border:none;border-right:solid #4BACC6 1.0pt;
  padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:green'>9:58:56 am or pm</span></p>
  </td>
 </tr>
 <tr style='height:12.35pt'>
  <td width=237 valign=top style='width:142.25pt;border:solid #4BACC6 1.0pt;
  border-right:none;padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><b><span style='font-size:9.5pt;font-family:Consolas;
  color:green'>date</span></b></p>
  </td>
  <td width=383 valign=top style='width:229.65pt;border:solid #4BACC6 1.0pt;
  border-left:none;padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>0</span><span style='font-size:9.5pt;font-family:Consolas;
  color:green'>1/23/2014  </span></p>
  </td>
 </tr>
 <tr style='height:12.35pt'>
  <td width=237 valign=top style='width:142.25pt;border:none;border-left:solid #4BACC6 1.0pt;
  padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><b><span style='font-size:9.5pt;font-family:Consolas;
  color:green'>Plan Name</span></b></p>
  </td>
  <td width=383 valign=top style='width:229.65pt;border:none;border-right:solid #4BACC6 1.0pt;
  padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas'>138Z4039-501</span></p>
  </td>
 </tr>
 <tr style='height:12.35pt'>
  <td width=237 valign=top style='width:142.25pt;border:solid #4BACC6 1.0pt;
  border-right:none;padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;text-align:
  justify;line-height:normal;text-autospace:none'><b><span style='font-size:
  9.5pt;font-family:Consolas;color:green'>Speed (mm/sec)</span></b></p>
  </td>
  <td width=383 valign=top style='width:229.65pt;border:solid #4BACC6 1.0pt;
  border-left:none;padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>160</span></p>
  </td>
 </tr>
</table>

<p class=MsoNormal>&nbsp;</p>

<p class=MsoNormal>These log fields, specifically the field 5 “status” offers
sufficient information to develop a stack lite state model. Below is the
translation of the Zeiss status messages into MTConnect controller logic:</p>

<table class=MsoTableLightListAccent5 border=1 cellspacing=0 cellpadding=0
 style='border-collapse:collapse;border:none'>
 <tr style='height:11.7pt'>
  <td width=279 valign=top style='width:167.4pt;border-top:solid #4BACC6 1.0pt;
  border-left:solid #4BACC6 1.0pt;border-bottom:none;border-right:none;
  background:#4BACC6;padding:0in 5.4pt 0in 5.4pt;height:11.7pt'>
  <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
  text-align:center;line-height:normal;text-autospace:none'><b><span
  style='font-size:9.5pt;font-family:Consolas;color:green'>State</span></b></p>
  </td>
  <td width=428 valign=top style='width:256.5pt;border-top:solid #4BACC6 1.0pt;
  border-left:none;border-bottom:none;border-right:solid #4BACC6 1.0pt;
  background:#4BACC6;padding:0in 5.4pt 0in 5.4pt;height:11.7pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><b><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>Action</span></b></p>
  </td>
 </tr>
 <tr style='height:11.7pt'>
  <td width=279 valign=top style='width:167.4pt;border:solid #4BACC6 1.0pt;
  border-right:none;padding:0in 5.4pt 0in 5.4pt;height:11.7pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><b><span style='font-size:9.5pt;font-family:Consolas;
  color:green'>End time detected</span></b></p>
  </td>
  <td width=428 valign=top style='width:256.5pt;border:solid #4BACC6 1.0pt;
  border-left:none;padding:0in 5.4pt 0in 5.4pt;height:11.7pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>power=ON</span></p>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>controllermode=MANUAL</span></p>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>execution=IDLE</span></p>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>program = “”</span></p>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>feed=0</span></p>
  </td>
 </tr>
 <tr style='height:11.7pt'>
  <td width=279 valign=top style='width:167.4pt;border:none;border-left:solid #4BACC6 1.0pt;
  padding:0in 5.4pt 0in 5.4pt;height:11.7pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><b><span style='font-size:9.5pt;font-family:Consolas;
  color:green'>Start time</span></b></p>
  </td>
  <td width=428 valign=top style='width:256.5pt;border:none;border-right:solid #4BACC6 1.0pt;
  padding:0in 5.4pt 0in 5.4pt;height:11.7pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>power=ON</span></p>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>controllermode=AUTOMATIC</span></p>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>execution=EXECUTING</span></p>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>program = plan name</span></p>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>feed = speed</span></p>
  </td>
 </tr>
 <tr style='height:12.35pt'>
  <td width=279 valign=top style='width:167.4pt;border:solid #4BACC6 1.0pt;
  border-right:none;padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><b><span style='font-size:9.5pt;font-family:Consolas;
  color:green'>Side effects</span></b></p>
  </td>
  <td width=428 valign=top style='width:256.5pt;border:solid #4BACC6 1.0pt;
  border-left:none;padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>RPM and xyz move if automatic and executing</span></p>
  </td>
 </tr>
 <tr style='height:12.35pt'>
  <td width=279 valign=top style='width:167.4pt;border-top:none;border-left:
  solid #4BACC6 1.0pt;border-bottom:solid #4BACC6 1.0pt;border-right:none;
  padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><b><span style='font-size:9.5pt;font-family:Consolas;
  color:green'>&nbsp;</span></b></p>
  </td>
  <td width=428 valign=top style='width:256.5pt;border-top:none;border-left:
  none;border-bottom:solid #4BACC6 1.0pt;border-right:solid #4BACC6 1.0pt;
  padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>&nbsp;</span></p>
  </td>
 </tr>
</table>

<p class=MsoNormal>Because of the deficiency of the MTConnect state logic, some
side effects are generated to make the controller appear to be operating:
positions for x,y,z and RPM change after every update if the controller is in
automatic mode and executing.</p>

<p class=MsoNormal>Some MTConnect tags are updated indepently of the contents
of the log file, these include:</p>

<table class=MsoTableLightListAccent5 border=1 cellspacing=0 cellpadding=0
 style='border-collapse:collapse;border:none'>
 <tr style='height:12.35pt'>
  <td width=620 colspan=2 valign=top style='width:371.9pt;border:solid #4BACC6 1.0pt;
  border-bottom:none;background:#4BACC6;padding:0in 5.4pt 0in 5.4pt;height:
  12.35pt'>
  <p class=MsoNormal align=center style='margin-bottom:0in;margin-bottom:.0001pt;
  text-align:center;line-height:normal;text-autospace:none'><b><span
  style='font-size:9.5pt;font-family:Consolas;color:green'>SIDE EFFECTS</span></b></p>
  </td>
 </tr>
 <tr style='height:12.35pt'>
  <td width=237 valign=top style='width:142.25pt;border:solid #4BACC6 1.0pt;
  border-right:none;padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><b><span style='font-size:9.5pt;font-family:Consolas;
  color:green'>power</span></b></p>
  </td>
  <td width=383 valign=top style='width:229.65pt;border:solid #4BACC6 1.0pt;
  border-left:none;padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>ON/OFF depending if log file found</span></p>
  </td>
 </tr>
 <tr style='height:12.35pt'>
  <td width=237 valign=top style='width:142.25pt;border:none;border-left:solid #4BACC6 1.0pt;
  padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><b><span style='font-size:9.5pt;font-family:Consolas;
  color:green'>AVAIL</span></b></p>
  </td>
  <td width=383 valign=top style='width:229.65pt;border:none;border-right:solid #4BACC6 1.0pt;
  padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>AVAILABLE</span></p>
  </td>
 </tr>
 <tr style='height:12.35pt'>
  <td width=237 valign=top style='width:142.25pt;border:solid #4BACC6 1.0pt;
  border-right:none;padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><b><span style='font-size:9.5pt;font-family:Consolas;
  color:green'>last_update</span></b></p>
  </td>
  <td width=383 valign=top style='width:229.65pt;border:solid #4BACC6 1.0pt;
  border-left:none;padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>Most recent line with “:” in it</span></p>
  </td>
 </tr>
 <tr style='height:12.35pt'>
  <td width=237 valign=top style='width:142.25pt;border-top:none;border-left:
  solid #4BACC6 1.0pt;border-bottom:solid #4BACC6 1.0pt;border-right:none;
  padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><b><span style='font-size:9.5pt;font-family:Consolas;
  color:green'>heartbeat</span></b></p>
  </td>
  <td width=383 valign=top style='width:229.65pt;border-top:none;border-left:
  none;border-bottom:solid #4BACC6 1.0pt;border-right:solid #4BACC6 1.0pt;
  padding:0in 5.4pt 0in 5.4pt;height:12.35pt'>
  <p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
  normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas;
  color:blue'>Increments every data gathering</span></p>
  </td>
 </tr>
</table>

<h1>Installation</h1>

<p class=MsoNormal>To install the Nikon Agent double click the (for 64 bit
machines only). These installation instructions have only been tested on
Windows 7 Enterprise edition PC.</p>

<p class=MsoNormal><img width=727 height=42 id="Picture 26"
src="ZeissAgentReadme_files/image001.png"></p>

<p class=MsoNormal>&nbsp;</p>

<p class=MsoNormal><img width=639 height=523 id="Picture 8"
src="ZeissAgentReadme_files/image002.png"></p>

<p class=MsoNormal>&nbsp;</p>

<p class=MsoNormal><img width=639 height=523 id="Picture 9"
src="ZeissAgentReadme_files/image003.png"></p>

<p class=MsoNormal>&nbsp;</p>

<p class=MsoNormal>&nbsp;</p>

<p class=MsoNormal><img width=639 height=523 id="Picture 10"
src="ZeissAgentReadme_files/image004.png"></p>

<p class=MsoNormal>&nbsp;</p>

<p class=MsoNormal><img width=639 height=523 id="Picture 11"
src="ZeissAgentReadme_files/image005.png"></p>

<p class=MsoNormal>&nbsp;</p>

<p class=MsoNormal><img width=639 height=523 id="Picture 12"
src="ZeissAgentReadme_files/image006.png"></p>

<p class=MsoNormal>&nbsp;</p>

<p class=MsoNormal>Acknowledge the installation permission challenge (you must
have administrator rights to install the mtconnect agent) then you should see
the installation screen:</p>

<p class=MsoNormal><img width=639 height=523 id="Picture 14"
src="ZeissAgentReadme_files/image007.png"></p>

<p class=MsoNormal>If you bring up Services in the Computer Management ( enter
“<b><i><span style='font-family:"Georgia","serif";color:#111111;background:
white'>compmgmt.msc” </span></i></b>in Run command line, then browse tree for
services – see below.) Again, you must have administrator rights to view this
screen, and you will be challenged to authenticate that you have access rights.</p>

<p class=MsoNormal><img width=780 height=403 id="Picture 15"
src="ZeissAgentReadme_files/image008.jpg"></p>

<p class=MsoNormal>The agent will NOT start unless you tell it to START (unless
you reboot).</p>

<h1>Uninstall</h1>

<p class=MsoNormal>First make sure the Nikon Agent is stopped in the Service
Control Manager:</p>

<p class=MsoNormal>Right click on My Computer -&gt; Select Manage -&gt;
Acknowledge UAC challenge</p>

<p class=MsoNormal>Select Services and Applications and then Services, scroll
down to ZeissAgent, and push Stop button.</p>

<p class=MsoNormal>After you have stopped the Nikon Agent service, go into the
control panel and uninstall the program: MTConnectAgentNikonx64</p>

<p class=MsoNormal>&nbsp;</p>

<p class=MsoNormal><img width=780 height=439 id="Picture 16"
src="ZeissAgentReadme_files/image009.jpg"></p>

<p class=MsoNormal>&nbsp;</p>

<p class=MsoNormal><img width=540 height=195 id="Picture 17"
src="ZeissAgentReadme_files/image010.png"></p>

<p class=MsoNormal>&nbsp;</p>

<p class=MsoNormal>Please wait .. acknowledge UAC permission to uninstall
challenge (you must be administrator or have administrator priviledges).  It
will uninstall and you may see a black console screen popup in the background
momemtarily.</p>

<p class=MsoNormal>And then in the Service Manager click Action-&gt;Refresh,
and the Nikon Agent service should be removed. The agent code in C:\Program
Files\MTConnect\MTConnectAgentNikonx64 should also be removed.</p>

<h1>Configuration</h1>

<p class=MsoNormal>The installation wizard installs the log file Agent into the
folder: C:\Program Files\MTConnect\ZeissMTConnectAgentX64 where X64 means a 64
bit installation platform, e.g., windows 7. The MTConnect Agent executable <u>must
be paired with the correct platform</u> (32 or 64 bit).</p>

<p class=MsoNormal><img width=780 height=423 id="Picture 24"
src="ZeissAgentReadme_files/image011.jpg"></p>

<p class=MsoNormal>In the folder, the vb script MTConnectPage-1.4.vbs in the
can be used to verify that the log file Agent is working. MTConnectPage-1.4.vbs
reads the data from the agent via <a href="http://127.0.0.1:5000">http://127.0.0.1:5000</a>
and then formats the data. (assuming you have configure the agent port to
5000).</p>

<h1>Configuration</h1>

<h2>Configuring MTConnect Devices</h2>

<p class=MsoNormal>Configuration of the Devices.xml file is done with the
Config.ini file.  The LogFile Agent looks in the [GLOBALS] section at the
“MTConnectDevice” tag to see how many devices with accompanying sections are
defined.  (The device name must be unique, but no check is done.)  For example,</p>

<div style='border:solid #2F6FAB 1.0pt;padding:12.0pt 12.0pt 12.0pt 9.0pt;
background:#F9F9F9'>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>[GLOBALS]</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>MTConnectDevice=M1</p>

</div>

<p class=MsoNormal>specifies a device M1, which has a unique section in the
Config.ini file that defines all the necessary configuration parameters
required. For actual deployment in a shop environment, only the ini file tag
“ProductionLog” is important and must be instantiated, because the embedded
MTConnect “Adapter” must know where the log file is located. </p>

<div style='border:solid #2F6FAB 1.0pt;padding:12.0pt 12.0pt 12.0pt 9.0pt;
background:#F9F9F9'>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>[M1]</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>ProductionLog=C:\Users\michalos\Documents\GitHub\Agents\AgfmMTConnectAgent\AgfmAgent\x64\Debug\ProductionLog.csv</p>

</div>

<p class=MsoNormal>Typically, a UNC file name is used to describe the file for
the production log. In the example above used for testing, a local file is
shown.</p>

<p class=MsoNormal>In the UNC Name Syntax UNC names identify network resources
using a specific notation. UNC names consist of three parts - a server name, a
share name, and an optional file path. These three elements are combined using
backslashes as follows:</p>

<div style='border:solid #2F6FAB 1.0pt;padding:12.0pt 12.0pt 12.0pt 9.0pt;
background:#F9F9F9'>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>\\server\share\file_path</p>

</div>

<p class=MsoNormal>The server portion of a UNC name references the strings
maintained by a network naming service such as DNS or WINS. Server names are
“computer names”. The share portion of a UNC name references a label created by
an administrator or, in some cases, within the operating system. In most
versions of Microsoft Windows, for example, the built-in share name admin$
refers to the root directory of the operating system installation (usually
C:\WINNT or C:\WINDOWS). The file path portion of a UNC name references the
local subdirectories beneath the share point. For example:</p>

<div style='border:solid #2F6FAB 1.0pt;padding:12.0pt 12.0pt 12.0pt 9.0pt;
background:#F9F9F9'>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>\\grandfloria\temp
(to reach C:\temp on the computer grandfloria)</p>

</div>

<p class=MsoNormal>In this example, the file temp must be shared to allow
remote access for MTConnect to read the file. Also, file permissions must allow
read access to this file. This may require administrator privileges in order to
perform the file read. The embedded MTConnect adapter only reads the file, it
does not write, delete or modify in any manner the contents of the log file. It
is on the onus of the logging service of the CNC to modify the log file.
Excessively large log files, will result in delays reading thru the log file to
reach the latest reading (typically because we cannot be sure what, when is
recorded in the log file.) </p>

<p class=MsoNormal>A complete config.ini file is shown below:</p>

<div style='border:solid #2F6FAB 1.0pt;padding:12.0pt 12.0pt 12.0pt 9.0pt;
background:#F9F9F9'>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>[GLOBALS]</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>Config=UPDATED</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>Debug=0</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>MTConnectDevice=M1,M2</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>ServiceName=ZeissAgent</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>logging_level=FATAL</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>QueryServer=10000</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>ServerRate=5000</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>AgentPort=5010</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>&nbsp;</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>[M1]</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>ProductionLog=C:\Users\michalos\Documents\GitHub\Agents\ZeissMTConnectAgent\ZeissAgent\x64\Debug\RunTracker.txt</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>&nbsp;</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>[M2]</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>ProductionLog=\\bpq465\log\events.log</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>&nbsp;</p>

</div>

<p class=MsoNormal>There are three sections in the config.ini file: Globals,
M1, and M2.  In the Globals section, the Config tag can be New or Updated. If
New, then a devices.xml file is generated based on the names of the
MTConnectDevice tag. The MTConnectDevice tag specifies the devices for which to
monitor the log file.  In this case, device “M1” and “M2” are  specified, each
with its own ini file section. The tag ServiceName specifies that the service
that runs will be called ZeissAgent, and monitor the http port specified by
AgentPort (i.e., 5010). The ServerRate tag specifies the wait in milliseconds
between log file reads. The QueryServer flag specifies the wait in milliseconds
in which to try to reconnect to the log file, if that read fails.</p>

<p class=MsoNormal>Sections [M1] and [M2] each contains a ProductionLog tag
that give the accompanying log file name for that device.</p>

<h1>Add devices after installation</h1>

<p class=MsoNormal><span style='color:#1F497D'>Modify Config.ini in C:\Program
Files\MTConnect\ZeissAgentx64directory</span></p>

<p class=MsoListParagraph style='margin-left:.25in;text-indent:-.25in'><span
style='color:#1F497D'>1)<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span><span style='color:#1F497D'>Stop Zeiss agent, edit config.ini
file, add new configuration:</span></p>

<p class=MsoListParagraph><span style='color:#1F497D'>&nbsp;</span></p>

<div style='border:solid #2F6FAB 1.0pt;padding:12.0pt 12.0pt 12.0pt 9.0pt;
background:#F9F9F9'>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>[GLOBALS]</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>Config=NEW</p>

</div>

<p class=MsoNormal style='margin-bottom:0in;margin-bottom:.0001pt;line-height:
normal;text-autospace:none'><span style='font-size:9.5pt;font-family:Consolas'>&nbsp;</span></p>

<p class=MsoListParagraph style='margin-left:.25in;text-indent:-.25in'><span
style='color:#1F497D'>2)<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span><span style='color:#1F497D'>Add new devices under [GLOBALS]
section tag “MTConnectDevice” (spaces are stripped out)</span></p>

<p class=MsoListParagraph><span style='color:#1F497D'>&nbsp;</span></p>

<div style='border:solid #2F6FAB 1.0pt;padding:12.0pt 12.0pt 12.0pt 9.0pt;
background:#F9F9F9'>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>MTConnectDevice=M1,
M2, M3</p>

</div>

<p class=MsoNormal style='margin-left:.25in'><span style='font-family:"Courier New";
color:#1F497D'>&nbsp;</span></p>

<p class=MsoListParagraph style='margin-left:.25in;text-indent:-.25in'><span
style='color:#1F497D'>3)<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span><span style='color:#1F497D'>Make sure there is an ini file
“section” for each device (in this case M1, M2, M3) and ProductionLog tag that
points to the UNC (Windows Universal Naming Convention) path to the log file as
in:</span></p>

<p class=MsoNormal><span style='color:#1F497D'>&nbsp;</span></p>

<div style='border:solid #2F6FAB 1.0pt;padding:12.0pt 12.0pt 12.0pt 9.0pt;
background:#F9F9F9'>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>[M1]</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>ProductionLog=\\grandflorio\c$\logfolder\Events_log_BP_NIKON.txt</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>[M2]</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>ProductionLog=\\rufous\c$\logfolder\Events_log_BP_NIKON.txt</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>[M3]</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>ProductionLog=\\synchro\c$\logfolder\Events_log_BP_NIKON.txt</p>

</div>

<p class=MsoListParagraph style='margin-left:.25in;text-indent:-.25in'><span
style='color:#1F497D'>4)<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span><span style='color:#1F497D'>Start Ziess agent, the agent will
detect a new configuration, and then write a new Devices.xml file to add the
new devices.</span></p>

<p class=MsoListParagraph style='margin-left:.25in;text-indent:-.25in'><span
style='color:#1F497D'>5)<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span><span style='color:#1F497D'>If it works config.ini tag should say
:”Config=UPDATED” if a problem tag will say: “Config=ERROR”</span></p>

<p class=MsoNormal>&nbsp;</p>

<h2>Add network share</h2>

<p class=MsoNormal>The MTConnect agent runs as a service, and thus has the
service rights. Service rights are generally local, and often cannot be used to
access network files across the enterprise domain. The following describes a
way to allow a domain user to access the files and then allow the MTConnect
agent service to access the file as a networked share.</p>

<p class=MsoListParagraph style='margin-left:.25in;text-indent:-.25in'><span
style='color:#1F497D'>1)<span style='font:7.0pt "Times New Roman"'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
</span></span><span style='color:#1F497D'>Add&nbsp; the lines in the config.ini
file section under each devices tag, in this case [M1]</span></p>

<div style='border:solid #2F6FAB 1.0pt;padding:12.0pt 12.0pt 12.0pt 9.0pt;
background:#F9F9F9'>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>[M1]</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>User=AGFM6449\oper</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>Pw=
Qwerty123\#</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>LocalShare=Z:</p>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>NetworkShare=\\Agfm6449\gfm\CNC\UserData_PRT\Logging</p>

</div>

<p class=MsoNormal>Some caveats are in orders. It is apparent that plain
password are not good. Further, you cannot use the same letter for the local
network share.</p>

<h1>Version Information from MTConnect Agent</h1>

<p class=MsoNormal>The version information of the various MTConnect components
is  available through web browser access. It is recommended to use Google
Chrome as it understand the XSLT formatting of the XML (Internet Explorer is
baffled.) </p>

<p class=MsoNormal>The open source core MTConnect agent is downloaded from
github and “frozen”. The version used is:</p>

<div style='border:solid #2F6FAB 1.0pt;padding:12.0pt 12.0pt 12.0pt 9.0pt;
background:#F9F9F9'>

<p class=BoxedCode style='background:#F9F9F9;border:none;padding:0in'>MTConnect
Agent Version 1.2.0.0 - built on Sat Oct 12 13:30:24 2013</p>

</div>

<p class=MsoNormal>It is not perfect but the output shown below gives an
indication of the software involved in the MTConnect Agent operation. It can be
modified to include other version information, but requires a recompilation at
this time.</p>

<p class=MsoNormal><img border=0 width=667 height=378 id="Picture 1"
src="ZeissAgentReadme_files/image012.png"></p>

<h2>&nbsp;</h2>

<p class=MsoNormal><span style='color:#1F497D'><img border=0 width=106
height=187 src="ZeissAgentReadme_files/image013.jpg"></span></p>

</div>

</body>

</html>
