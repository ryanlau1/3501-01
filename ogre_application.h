#ifndef OGRE_APPLICATION_H_
#define OGRE_APPLICATION_H_

#include <exception>
#include <string>

#include "OGRE/OgreRoot.h"
#include "OGRE/OgreRenderSystem.h"
#include "OGRE/OgreRenderWindow.h"
#include "OGRE/OgreWindowEventUtilities.h"
#include "OGRE/OgreManualObject.h"
#include "OGRE/OgreEntity.h"
#include "OIS/OIS.h"

namespace ogre_application {


	/* A useful type to define */
	typedef std::vector<Ogre::String> Strings;

	/* Our exception type */
	class OgreAppException: public std::exception
    {
		private:
		    std::string message_;
	    public:
			OgreAppException(std::string message) : message_(message) {};
			virtual const char* what() const throw() { return message_.c_str(); };
	};

	/* Our Ogre application */
	class OgreApplication :
	    public Ogre::FrameListener, // Derive from FrameListener to be able to have render event callbacks
        public Ogre::WindowEventListener // Derive from FrameListener to be able to have window event callbacks
		{

        public:
			Ogre::SceneNode* torus;
            OgreApplication(void);
			// Call Init() before running the main loop
            void Init(void); 
			// Create a torus and add it to the list of mesh resources
			void CreateTorus(Ogre::String object_name, float loop_radius = 0.6, float circle_radius = 0.2, int num_loop_samples = 90, int num_circle_samples = 30);
			// Create an instance of mesh "object_name"
			// Return the scene node of the object so that we can edit its position in the world
            Ogre::SceneNode* OgreApplication::CreateEntity(Ogre::String entity_name, Ogre::String object_name, Ogre::String material_name);
			// Setup a specific rotation animation for an object
            void SetupAnimation(Ogre::String object_name); 
			// Keep application active
            void MainLoop(void); 

        private:
			// Create root that allows us to access Ogre commands
            std::auto_ptr<Ogre::Root> ogre_root_;
            // Main Ogre window
            Ogre::RenderWindow* ogre_window_;

			// For animating an object
			Ogre::AnimationState *animation_state_; // Keep state of the animation
			bool animating_; // Whether animation is on or off
			bool space_down_; // Whether space key was pressed

			// Input managers
			OIS::InputManager *input_manager_;
			OIS::Mouse *mouse_;
			OIS::Keyboard *keyboard_;

			/* Methods to initialize the application */
			void InitRootNode(void);
			void InitPlugins(void);
			void InitRenderSystem(void);
			void InitWindow(void);
			void InitViewport(void);
			void InitEvents(void);
			void InitOIS(void);
			void LoadMaterials(void);

			/* Methods to handle events */
			bool frameRenderingQueued(const Ogre::FrameEvent& fe);
			void windowResized(Ogre::RenderWindow* rw);
			
			Ogre::SceneNode* torus2;
			Ogre::SceneNode* torus3;
			Ogre::SceneNode* torus5;

			bool t2;
			bool t3;
			bool t5;
    }; // class OgreApplication

} // namespace ogre_application;

#endif // OGRE_APPLICATION_H_
