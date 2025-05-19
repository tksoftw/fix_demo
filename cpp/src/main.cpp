#include "Replayer.hpp"
#include "DBNReader.hpp"
#include <thread>
#include <iostream>
#include <fix8/f8includes.hpp>
#include <fix8/f8types.hpp>
#include "FIX44_types.hpp"
#include "FIX44_router.hpp"
#include "FIX44_classes.hpp"

int main(int argc, char* argv[])
{
  // Expect an XML config file and DBN file as arguments:
  // e.g. ./tick_sim server.xml ticks.dbn
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <config> <ticks.dbn>\n";
    return 1;
  }

  const std::string configFile = argv[1];
  const std::string dbnFile    = argv[2];

  try {
    // Instantiate the Replayer session
    Replayer session(configFile.c_str());
    std::cout << "got here" << std::endl;
    // Log on synchronously (handshake)
    if (!session.start(true)) {
      std::cerr << "Failed to start FIX session\n";
      return 1;
    }

    std::cout << "FIX session started successfully\n";

    // Start replay loop
    DBNReader reader(dbnFile);
    Tick tick;
    while (reader.next(tick)) {
      session.send_tick(tick);
      std::this_thread::sleep_for(Replayer::throttle);
    }

    // Logout & clean up
    session.stop();
    std::cout << "Replay completed successfully\n";
    return 0;
  }
  catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
}
