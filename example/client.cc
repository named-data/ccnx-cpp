
#include <ndn.cxx.h>
#include <iostream>

using namespace std;

const char *FILENAME = NULL;
ndn::Name InterestBaseName;

// create a global handler
ndn::Wrapper handler;

void OnData (ndn::Name name, ndn::PcoPtr pco);
void OnTimeout (ndn::Name name, const ndn::Closure &closure, ndn::Selectors selectors);

void OnData (ndn::Name name, ndn::PcoPtr pco)
{
  ndn::BytesPtr content = pco->contentPtr ();
  cout << string ((char*)ndn::head (*content), content->size ());

  int seqnum = ndn::Name::asSeqNum (*name.rbegin ());
  if (seqnum >= 10)
    {
      return;
    }

  cerr << ">> C++ " << ndn::Name (InterestBaseName).appendSeqNum (seqnum + 1) << endl; // a shortcut to construct name
  handler.sendInterest (ndn::Name (InterestBaseName).appendSeqNum (seqnum + 1),
                        ndn::Closure (OnData, OnTimeout),
                        ndn::Selectors ().scope (ndn::Selectors::SCOPE_LOCAL_HOST));
}

void OnTimeout (ndn::Name name, const ndn::Closure &closure, ndn::Selectors selectors)
{
  // re-express interest
  handler.sendInterest (name, closure, selectors);
}

int
main (int argc, char **argv)
{
  if (argc < 2)
    {
      std::cerr << "You have to specify filename as an argument" << std::endl;
      return -1;
    }
  
  // this code does not check for most of the bad conditions
  FILENAME = argv[1];

  InterestBaseName = ndn::Name ("/my-local-prefix/simple-fetch/file");
  InterestBaseName.append (FILENAME);

  cerr << ">> C++ " << ndn::Name (InterestBaseName).appendSeqNum (0) << endl;
  handler.sendInterest (ndn::Name (InterestBaseName).appendSeqNum (0),
                        ndn::Closure (OnData, OnTimeout),
                        ndn::Selectors ().scope (ndn::Selectors::SCOPE_LOCAL_HOST));

  sleep (3);
  return 0;
}
