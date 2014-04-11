	//Part *			FindPart(std::string partid) { return _factory->FindPartById(partid.c_str());}
	//ProcessPlan *	FindProcessPlan(std::string ppid) { return _factory->FindProcessPlanById(ppid.c_str());}
	//Cell *			FindCell(std::string cellid) { return _factory->FindCellById(cellid.c_str());}
	//Resource *		FindResource(std::string rid) { return _factory->FindResourceById(rid.c_str());}


std::vector<std::string>::iterator i = vLine.begin();
while(i != vLine.end())
{
    if(i->find(';', 0) != std::string::npos)
    {
        i = vLine.erase(i);
		
		"								<DataItem type=\"CODE\" id=\"####alarm\" category=\"EVENT\" name=\"alarm\"/>\n"

		
		#include <random>
#include <functional>

typedef std::mt19937 engine_type; // a Mersenne twister engine
std::uniform_int_distribution<engine_type::result_type> udist(0, 200);

engine_type engine;

int main()
{
  // seed rng first:
  engine_type::result_type const seedval = get_seed();
  engine.seed(seedval);

  // bind the engine and the distribution
  auto rng = std::bind(udist, engine);

  // generate a random number
  auto random_number = rng();

  return random_number;
}
Unsure about UNC naming for local file system

		std::vector<std::string> lines;
		lines.push_back(sentence); // last line (has all current info)

		std::string thisline = sentence;
		std::string program= FindPattern(thisline, "(", ")");
		SetMTCTagValue("program",program);
		ReplacePattern(thisline, "(", ")", "0");


1) You will need to install boost to get its include and libraries.

2) You will need to modify the include directive in

C:\Users\michalos\Documents\GitHub\MTConnectCmdSimMTConnectAgent\MTConnectCmdSimAgent\IncludeDirX32.txt

to point to the location of you MTConnectCmdSimAgent (now in C:\Users\michalos\Documents\GitHub\MTConnectCmdSimMTConnectAgen)
which contains:

-I"C:\Program Files\NIST\src\boost_1_54_0"
-I.
-I"C:\Users\michalos\Documents\GitHub\MTConnectCmdSimMTConnectAgent\MTConnectAgent\agent"
-I"C:\Users\michalos\Documents\GitHub\MTConnectCmdSimMTConnectAgent\MTConnectAgent\lib"
-I"C:\Users\michalos\Documents\GitHub\MTConnectCmdSimMTConnectAgent\MTConnectAgent\win32\libxml2-2.7.7\include"
-I"C:\Users\michalos\Documents\GitHub\MTConnectCmdSimMTConnectAgent\MTConnectAgent"
-I"C:\Users\michalos\Documents\GitHub\MTConnectCmdSimMTConnectAgent\NIST"

3) IN the MTConnectCmdSimAgent project setting for the linker, you will need to add the folders that point to your file location for:
a) C:\Users\michalos\Documents\GitHub\MTConnectCmdSimMTConnectAgent\MTConnectAgent\win32\libxml2-2.7.7\lib
b) C:\Program Files\NIST\src\boost_1_54_0\stage or wherever you have built the libraries for boost - Note 32 bit is different
that 64 bit!!


On Target Machine (Installation)
1) Create  install directory C:\Program Files\MTConnect\MTConnectCmdSimAgent
2) Copy files and exe to install directory
3) Change file permission via cmd and InstallShield in install directory to everyone full control.
4) Register Agent in Windows service as automatic with reference to MTConnectCmdSimAgent BUT NOT STARTED
5) User will need to configure  Config.ini for 
	MTConnectDevice=M1 
or whatever device name anb then configure ini file section:
	[M1]
	IPAddress=127.0.0.1
	ProductionLog=C:\Users\michalos\AppData\Local\MTConnect\ProductionLog.csv
	logging_level=ERROR
	Simulation=1
 Most important for now is UNC location for ProductionLog. IPAddress is ignored.

Prerequisite assumptions:

1) Starting midday service - otherwise may have to go back a day to look for Tags
2) Install changes permissions of Program Files (agent might write some logs to install directory) not temp directory.
cd /d %~dp0

echo y|  cacls .  /t  /c  /GRANT Everyone:F
NOTE changed already with addition of echo y and obsoleted cacls recently.
3) FMTConnectCmdSiml debug stoppage will go to Application Event Log (hopefullly)
4) Used sc.exe to install agent (and to stop and delete)
5) Agent is not configured! You will need to setup config.ini and then start agent.

Potential improvements:
1) check modification time stamp before copy


TESTS:
UNC remote fetch
