#include <thread>
#include "core_elements.h"
#include "sdlrenderer.h"

int main(int argc, char* argv[] ) {
	
	// First, lets set up our window:

	Window window;

	// Now lets add some stuff to our window. We'll use example.pivionics

	window.command("load example.pivionics",&window);

	// Now lets create our Display:

	Display display;
	display.link_window(&window);

	// Now the scary part - set up the renderer

	SdlRenderer render;
	render.init();
	render.render_run();
	display.link_renderer(&render);

	// This is where we do our stuff

	render.render_stop();
	render.shutdown();

	return EXIT_SUCCESS;
}
