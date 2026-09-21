// Routing: Creates a new NDI source that routes (switches) between the sources found
// on the network. Useful for building a software switcher.
// Adapted from the NDIStream RoutingExample.

import ndi.stream.*;

NDIRouter router;
NDIFinder finder;

String currentRoute = "(nothing)";
long lastSwitchTime = 0;

// Stay on each source for 15 seconds
final long SWITCH_INTERVAL = 15 * 1000;

void setup() {
  size(400, 200);

  // Create a new routed source
  router = new NDIRouter("Example Routed Source");
  finder = new NDIFinder();
}

void draw() {
  background(0);
  fill(255);
  text("Routing to:", 20, 40);
  text(currentRoute, 20, 70);

  if (millis() - lastSwitchTime < SWITCH_INTERVAL) {
    return;
  }
  lastSwitchTime = millis();

  // Get a list of current sources on the network
  NDISource[] availableSources = finder.getCurrentSources();

  if (availableSources.length == 0) {
    // If no sources were found, route to nowhere (black screen)
    router.clearSource();
    currentRoute = "(no sources found)";
    println("Routing to nothing.");
  } else {
    // If sources were found, choose one at random to route to
    NDISource targetSource = availableSources[(int) random(availableSources.length)];
    router.setSource(targetSource);
    currentRoute = targetSource.getSourceName();
    println("Routing to " + currentRoute);
  }
}

void stop() {
  finder.close();
  router.close();
}
