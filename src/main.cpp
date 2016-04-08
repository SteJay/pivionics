#include <thread>
#include <iostream>
#include <unistd.h>
#include "core_elements.h"
#include "sdlrenderer.h"
#include "sdlcompositor.h"
#include "circle.h"
int main(int argc, char* argv[] ) {
	
	// First, lets set up our window:

	Window window;

	// Now lets add some stuff to our window. We'll use example.pivionics

	window.register_creator("Element",&fn_create_element);
	window.register_creator("Circle",&fn_create_circle);
	window.command("load example.pivionics",&window);
//cout << "Constructing window..." << endl;
	window.construct();
	// Now lets create our Compositor:
//cout << "Setting up compositor..." << endl;
	SdlCompositor compositor;
//cout << "Linking window..." << endl;
	compositor.link_window(&window);
	// Now the scary part - set up the renderer
//cout << "Setting up renderer..." << endl;
	SdlRenderer render;
	///cout << "Initialising renderer..." << endl;
	//render.init();
//cout << "Running renderer..." << endl;
	render.render_run();
//cout << "Linking renderer..." << endl;
	compositor.link_renderer(&render);
	// OK, the renderer should now be up and running
	// Now the compositor needs to get going
	// As we're not changing this so far, we'll just call it once rather than looping
	
//cout << "Composing frame..." << endl;
	Origin origin;
	origin.position.x=360;
	origin.position.y=360;
	origin.angle=0;
	origin.scale.x=1.0;
	origin.scale.y=1.0;
	compositor.compose();
//cout << "Frame composed - Waiting for a while" << endl;
	// This is where we do our stuff
	// for now we'll just sleep
	sleep(15);
//cout << "Stopping renderer..." << endl;
	render.render_stop();
//cout << "Shutting down renderer..." << endl;
	render.shutdown();
//cout << "Program complete." << endl;
	return EXIT_SUCCESS;
}
