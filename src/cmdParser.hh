/* Quick and dirty parser for command line */

#include <map>
#include <string>
#include <vector>
using namespace std;


class cmdParser
{
public:
    typedef map<string, string>::iterator optIterator;
    enum OptionData {None = 0, Exist = 1, Mandatory = 2,
                     HasParameter = 4, HasNoParameter = 8};

    cmdParser ();
    void addOption (const string& optname, int type = cmdParser::HasNoParameter);
    string analyse (int argc, char *argv[]);
    bool hasOption (const string& key);
    string optValue (const string& key);
    int nbOptions ();
    int nbParams ();
    string param (int k);
    optIterator optBegin ();
    optIterator optEnd ();

private:
    string parseOption (const char *p); // decode --option = value pairs
    int optionData(const string& opt); // return option data of opt
    map<string, string> opts; // keyword = value pairs
    vector<string> params; // naked params
    map<string, int> validopts; // valid options
};
