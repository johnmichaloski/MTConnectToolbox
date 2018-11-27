from distutils.core import setup
import py2exe
import sys
if len(sys.argv) == 1:
    sys.argv.append("py2exe")
    sys.argv.append("-q")

class Target:
    def __init__(self, **kw):
    self.__dict__.update(kw)
     # for the versioninfo resources
     self.version = "1.0.0"
     self.company_name = "NIST"
     self.copyright = "no copyright"
     self.name = "MTConnectAgentFwding"

 myservice = Target(
     # used for the versioninfo resource
     description = "A sample Windows NT service",
     # what to build.  For a service, the module name (not the
     # filename) must be specified!
     modules = ["winservice_py2exe"]
     )

 setup(
     options = {"py2exe": {"typelibs":
                      # typelib for WMI
                       [('{565783C6-CB41-11D1-8B02-00600806D9B6}', 0, 1, 2)],
                       # create a compressed zip archive
                       "compressed": 1,
                       "optimize": 2}},
     # The lib directory contains everything except the executables and the python dll.
     # Can include a subdirectory name.
     zipfile = "lib/shared.zip",

     service = [MTConnectAgentFwding]
    )