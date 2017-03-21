# petrinet
boost/C++ implementation of a formal PetriNet, with some extensions

Supported
* Places, transitions and tokens  https://en.wikipedia.org/wiki/Petri_net
* Colored petrinets               https://en.wikipedia.org/wiki/Coloured_Petri_net
* Place capacity                  https://en.wikipedia.org/wiki/Petri_net#Boundedness
* Multi-level tokens              (no literature)

Each token is a dynamically allocated object, making it perfect for
data-management in complex asynchronous software systems.

The multi-level token system allows tokens to be split up and merged between
places. The resulting child-tokens can move through the net individually. This
is another tool to simplify data-management. In testpetrinet.cpp, check out the
tests 'testPlaceLevel2', 'testLevelUp' and 'testSimultaneousUpDown' for examples.


