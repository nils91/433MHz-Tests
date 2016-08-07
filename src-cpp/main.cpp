#include <cstdlib>

#include "Sniffer.h"

int main(int argc, char* argv[])
{
  Sniffer *snif=new Sniffer();
  snif->snif();
}
