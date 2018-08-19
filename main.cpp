#include <iostream>
#include <exception>
#include "ogre_application.h"

/* Macro for printing exceptions */
#define PrintException(exception_object)\
	std::cerr << exception_object.what() << std::endl

/* Main function that builds and runs the application */
int main(void){
    ogre_application::OgreApplication application;

	try {
        // Initialize application
		application.Init();
        // Create a torus mesh and add it to the mesh resources
		application.CreateTorus("TorusMesh");
        // Create an instance of the torus mesh
        Ogre::SceneNode* torus = application.CreateEntity("TorusInstance", "TorusMesh", "WhiteSurface");
        // Scale the instance
		torus->scale(0.3, 0.3, 0.3);
		// Init animation
		//application.SetupAnimation("TorusInstance");
		application.torus = torus;
		std::cout<<"HI";

		application.MainLoop();	
	}
	catch (std::exception &e){
		PrintException(e);
	}

    return 0;
}
