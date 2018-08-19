#include "ogre_application.h"
#include "bin/path_config.h"

namespace ogre_application {

/* Some configuration constants */
/* They are written here as global variables, but ideally they should be loaded from a configuration file */

/* Initialization */
const Ogre::String config_filename_g = "";
const Ogre::String plugins_filename_g = "";
const Ogre::String log_filename_g = "Ogre.log";

/* Main window settings */
const Ogre::String window_title_g = "Demo";
const Ogre::String custom_window_capacities_g = "";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const bool window_full_screen_g = false;

/* Viewport and camera settings */
float viewport_width_g = 0.95f;
float viewport_height_g = 0.95f;
float viewport_left_g = (1.0f - viewport_width_g) * 0.5f;
float viewport_top_g = (1.0f - viewport_height_g) * 0.5f;
unsigned short viewport_z_order_g = 100;
const Ogre::ColourValue viewport_background_color_g(0.0, 0.0, 0.0);
float camera_near_clip_distance_g = 0.01;
float camera_far_clip_distance_g = 100.0;
Ogre::Vector3 camera_position_g(0.5, 0.5, 1.5);
Ogre::Vector3 camera_look_at_g(0.0, 0.0, 0.0);
Ogre::Vector3 camera_up_g(0.0, 1.0, 0.0);

/* Materials */
const Ogre::String material_directory_g = MATERIAL_DIRECTORY;


OgreApplication::OgreApplication(void){

    /* Don't do work in the constructor, leave it for the Init() function */
}


void OgreApplication::Init(void){

	/* Set default values for the variables */
	animating_ = false;
	space_down_ = false;
	animation_state_ = NULL;
	input_manager_ = NULL;
	keyboard_ = NULL;
	mouse_ = NULL;

	/* Run all initialization steps */
    InitRootNode();
    InitPlugins();
    InitRenderSystem();
    InitWindow();
    InitViewport();
	InitEvents();
	InitOIS();
	LoadMaterials();
}


void OgreApplication::InitRootNode(void){

    try {
		
		/* We need to have an Ogre root to be able to access all Ogre functions */
        ogre_root_ = std::auto_ptr<Ogre::Root>(new Ogre::Root(config_filename_g, plugins_filename_g, log_filename_g));
		//ogre_root_->showConfigDialog();

    }
    catch (Ogre::Exception &e){
		throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
		throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::InitPlugins(void){

    try {

		/* Load plugin responsible for OpenGL render system */
        Strings plugin_names;
        plugin_names.push_back("RenderSystem_GL");
		
        Strings::iterator iter = plugin_names.begin();
        Strings::iterator iter_end = plugin_names.end();
        for (; iter != iter_end; iter++){
            Ogre::String& plugin_name = (*iter);
            if (OGRE_DEBUG_MODE){
                plugin_name.append("_d");
            }
            ogre_root_->loadPlugin(plugin_name);
        }

    }
    catch (Ogre::Exception &e){
		throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::InitRenderSystem(void){

    try {

        /* Load render system */
        const Ogre::RenderSystemList& render_system_list = ogre_root_->getAvailableRenderers();
        if (render_system_list.size() == 0)
        {
			throw(OgreAppException(std::string("OgreApp::Exception: Sorry, no rendersystem was found.")));
        }

        Ogre::RenderSystem *render_system = render_system_list.at(0);
        ogre_root_->setRenderSystem(render_system);

    }
    catch (Ogre::Exception &e){
		throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}

        
void OgreApplication::InitWindow(void){

    try {

        /* Create main window for the application */
		bool create_window_automatically = false;
        ogre_root_->initialise(create_window_automatically, window_title_g, custom_window_capacities_g);

        Ogre::NameValuePairList params;
        params["FSAA"] = "0";
        params["vsync"] = "true";
        ogre_window_ = ogre_root_->createRenderWindow(window_title_g, window_width_g, window_height_g, window_full_screen_g, &params);

        ogre_window_->setActive(true);
        ogre_window_->setAutoUpdated(false);
    }
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::InitViewport(void){

    try {

        /* Retrieve scene manager and root scene node */
        Ogre::SceneManager* scene_manager = ogre_root_->createSceneManager(Ogre::ST_GENERIC, "MySceneManager");
        Ogre::SceneNode* root_scene_node = scene_manager->getRootSceneNode();

        /* Create camera object */
        Ogre::Camera* camera = scene_manager->createCamera("MyCamera");
        Ogre::SceneNode* camera_scene_node = root_scene_node->createChildSceneNode("MyCameraNode");
        camera_scene_node->attachObject(camera);

        camera->setNearClipDistance(camera_near_clip_distance_g);
        camera->setFarClipDistance(camera_far_clip_distance_g); 

		camera->setPosition(camera_position_g);
		camera->lookAt(camera_look_at_g);
		camera->setFixedYawAxis(true, camera_up_g);

        /* Create viewport */
        Ogre::Viewport *viewport = ogre_window_->addViewport(camera, viewport_z_order_g, viewport_left_g, viewport_top_g, viewport_width_g, viewport_height_g);

        viewport->setAutoUpdated(true);
        viewport->setBackgroundColour(viewport_background_color_g);

		/* Set aspect ratio */
		float ratio = float(viewport->getActualWidth()) / float(viewport->getActualHeight());
        camera->setAspectRatio(ratio);

    }
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::InitEvents(void){

	try {

		/* Add this object as a FrameListener for render events (see frameRenderingQueued event) */
		ogre_root_->addFrameListener(this);

		/* Add this object as a WindowEventListener to handle the window resize event */
		Ogre::WindowEventUtilities::addWindowEventListener(ogre_window_, this);

	}
    catch (Ogre::Exception &e){
		throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
		throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::InitOIS(void){

	/* Initialize the Object Oriented Input System (OIS) */
	try {

		/* Initialize input manager */
		OIS::ParamList pl; // Parameter list passed to the input manager initialization
		size_t windowHnd = 0;
		std::ostringstream windowHndStr;
		ogre_window_->getCustomAttribute("WINDOW", &windowHnd);
		windowHndStr << windowHnd;
		pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
		pl.insert(std::make_pair(std::string("w32_mouse"), 
		std::string("DISCL_FOREGROUND" )));
		pl.insert(std::make_pair(std::string("w32_mouse"), 
		std::string("DISCL_NONEXCLUSIVE")));
		input_manager_ = OIS::InputManager::createInputSystem(pl);

		/*size_t hWnd = 0;
		ogre_window_->getCustomAttribute("WINDOW", &hWnd);
		input_manager_ = OIS::InputManager::createInputSystem(hWnd);*/

		/* Initialize keyboard and mouse */
		keyboard_ = static_cast<OIS::Keyboard*>(input_manager_->createInputObject(OIS::OISKeyboard, false));

		mouse_ = static_cast<OIS::Mouse*>(input_manager_->createInputObject(OIS::OISMouse, false));
		unsigned int width, height, depth;
		int top, left;
		ogre_window_->getMetrics(width, height, depth, left, top);
		const OIS::MouseState &ms = mouse_->getMouseState();
		ms.width = width;
		ms.height = height;

	}
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::LoadMaterials(void){

    try {
		
		/* Load materials that can then be assigned to objects in the scene */
		Ogre::String resource_group_name = "MyGame";
		Ogre::ResourceGroupManager& resource_group_manager = Ogre::ResourceGroupManager::getSingleton();
		resource_group_manager.createResourceGroup(resource_group_name);
		bool is_recursive = false;
		resource_group_manager.addResourceLocation(material_directory_g, "FileSystem", resource_group_name, is_recursive);
		resource_group_manager.initialiseResourceGroup(resource_group_name);
		resource_group_manager.loadResourceGroup(resource_group_name);

	}
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::CreateTorus(Ogre::String object_name, float loop_radius, float circle_radius, int num_loop_samples, int num_circle_samples){

    try {
		/* Create a torus
		   The torus is built from a large loop with small circles around the loop */

        /* Retrieve scene manager and root scene node */
        Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
        Ogre::SceneNode* root_scene_node = scene_manager->getRootSceneNode();

        /* Create the 3D object */
        Ogre::ManualObject* object = NULL;
        object = scene_manager->createManualObject(object_name);
        object->setDynamic(false);

        /* Create triangle list for the object */
		object->begin("", Ogre::RenderOperation::OT_TRIANGLE_LIST);

		/* Add vertices to the object */
		float theta, phi; // Angles for circles
		Ogre::Vector3 loop_center;
		Ogre::Vector3 vertex_position;
		Ogre::Vector3 vertex_normal;
		Ogre::ColourValue vertex_color;
		for (int i = 0; i < num_loop_samples; i++){ // large loop
			
			theta = Ogre::Math::TWO_PI*i/num_loop_samples; // loop sample (angle theta)
			loop_center = Ogre::Vector3(loop_radius*cos(theta), loop_radius*sin(theta), 0); // centre of a small circle

			for (int j = 0; j < num_circle_samples; j++){ // small circle
				
				phi = Ogre::Math::TWO_PI*j/num_circle_samples; // circle sample (angle phi)
				
				/* Define position, normal and color of vertex */
				vertex_normal = Ogre::Vector3(cos(theta)*cos(phi), sin(theta)*cos(phi), sin(phi));
				vertex_position = loop_center + vertex_normal*circle_radius;
				/*Ogre::Vector3(loop_center.x + local_normal.x*circle_radius, 
				                loop_center.y + local_normal.y*circle_radius, 
				     			loop_center.z + local_normal.z*circle_radius);*/
				vertex_color = Ogre::ColourValue(1.0 - ((float) i / (float) num_loop_samples), 
				                                 (float) i / (float) num_loop_samples, 
				                                 (float) j / (float) num_circle_samples);
				/*vertex_color = Ogre::ColourValue(0.0, 0.0, 1.0);*/

				/* Add them to the object */	

				object->position(vertex_position);			
				object->normal(vertex_normal);
				object->colour(vertex_color); 
			}
		}

		/* Add triangles to the object */
		for (int i = 0; i < num_loop_samples; i++){
			for (int j = 0; j < num_circle_samples; j++){
				// Two triangles per quad
				object->triangle(((i + 1) % num_loop_samples)*num_circle_samples + j, 
							     i*num_circle_samples + ((j + 1) % num_circle_samples),
								 i*num_circle_samples + j);	
				object->triangle(((i + 1) % num_loop_samples)*num_circle_samples + j,
					             ((i + 1) % num_loop_samples)*num_circle_samples + ((j + 1) % num_circle_samples),
								 i*num_circle_samples + ((j + 1) % num_circle_samples));
				/*object->triangle(((i + 1) % num_loop_samples)*num_circle_samples + j, 
					             i*num_circle_samples + j,
							     i*num_circle_samples + ((j + 1) % num_circle_samples));	
				object->triangle(((i + 1) % num_loop_samples)*num_circle_samples + j,
					             i*num_circle_samples + ((j + 1) % num_circle_samples),
					             ((i + 1) % num_loop_samples)*num_circle_samples + ((j + 1) % num_circle_samples));*/
			}
		}

		/* We finished the object */
        object->end();
		
        /* Convert triangle list to a mesh */
        object->convertToMesh(object_name);
    }
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


Ogre::SceneNode* OgreApplication::CreateEntity(Ogre::String entity_name, Ogre::String object_name, Ogre::String material_name){

	try {
		/* Create one instance of the mesh "object_name" */
		/* The same object can have multiple instances or entities */

		/* Retrieve scene manager and root scene node */
        Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
        Ogre::SceneNode* root_scene_node = scene_manager->getRootSceneNode();

		/* Create entity */
        Ogre::Entity* entity = scene_manager->createEntity(object_name);

		/* Apply a material to the entity to give it color, etc. */
		entity->setMaterialName(material_name);

		/* Create a scene node for the entity */
		/* The scene node keeps track of the entity's position, orientation and scale */
        Ogre::SceneNode* scene_node = root_scene_node->createChildSceneNode(entity_name);
        scene_node->attachObject(entity);

        return scene_node;
	}
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::MainLoop(void){

    try {
		
		t2 = t3 = t5 = false;
		int x = rand() % 1000;
		int y = 0;
        /* Main loop to keep the application going */

        ogre_root_->clearEventTimes();

		int numOthers = 0;
        while(!ogre_window_->isClosed()){
            ogre_window_->update(false);

            ogre_window_->swapBuffers();

            ogre_root_->renderOneFrame();

            Ogre::WindowEventUtilities::messagePump();				
			if (y < 3){
				if (x == 1){
					if (y == 0){
						this->CreateTorus("TorusMesh2");
						// Create an instance of the torus mesh
						torus2 = this->CreateEntity("TorusInstance2", "TorusMesh2", "TRed");
						torus2->scale(0.2,0.3,0.5);
						torus2->setPosition(0.3,0.2, 0.4);
						t2 = true;
					}
					else if (y == 1){
						this->CreateTorus("TorusMesh3");
						// Create an instance of the torus mesh
						torus3 = this->CreateEntity("TorusInstance3", "TorusMesh3", "Simple");
						torus3->scale(0.1,0.1,0.1);
						torus3->setPosition(0.1,0.1,0.8);
						t3 = true;
					}
					else if (y == 2){
						this->CreateTorus("TorusMesh5");
						// Create an instance of the torus mesh
						torus5 = this->CreateEntity("TorusInstance5", "TorusMesh5", "TRed");
						torus5->scale(0.4,0.2,0.1);
						torus5->setPosition(0.1,0.7,0.3);
						t5 = true;
					}
					y++;
				}
				x= rand()%100;
			}
			if (t2){
				if (torus2->getPosition().distance(torus->getPosition()) < (0.33)){
					torus2->setPosition(5,5,5);
					t2 = false;
				}
			}
			if (t3){
				if (torus3->getPosition().distance(torus->getPosition()) < 0.2){
					torus3->setPosition(5,5,5);
					t3 = false;
				}
			}
			if (t5){
				if (torus5->getPosition().distance(torus->getPosition()) < 0.4){
					torus5->setPosition(5,5,5);
					t5 = false;
				}
			}
		}
		
    }
    catch (Ogre::Exception &e){
        throw(OgreAppException(std::string("Ogre::Exception: ") + std::string(e.what())));
    }
    catch(std::exception &e){
        throw(OgreAppException(std::string("std::Exception: ") + std::string(e.what())));
    }
}


void OgreApplication::SetupAnimation(Ogre::String object_name){

	/* Retrieve scene manager and root scene node */
    Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
	Ogre::SceneNode* root_scene_node = scene_manager->getRootSceneNode();

	/* Set up animation */
	Ogre::Real duration = Ogre::Math::TWO_PI;
	Ogre::Real num_steps = 36;
	Ogre::Real step = duration/num_steps;
	Ogre::Animation* animation = scene_manager->createAnimation("Animation", duration);
	animation->setInterpolationMode(Ogre::Animation::IM_LINEAR);
	Ogre::Node *object_scene_node = root_scene_node->getChild(object_name);
	Ogre::NodeAnimationTrack* track = animation->createNodeTrack(0, object_scene_node);
	
	/* Set up frames for animation */
	Ogre::TransformKeyFrame* key;
	Ogre::Quaternion quat;
	Ogre::Matrix3 rot1, rot2, rot3;
	rot1 = Ogre::Matrix3::IDENTITY;
	for (int i = 0; i < num_steps; i++){
		Ogre::Real current = ((float) i) * step;
		key = track->createNodeKeyFrame(current);
		rot2.FromAngleAxis(Ogre::Vector3(0, 1, 0), Ogre::Radian(-current));
		rot3 = rot2 * rot1;
		quat.FromRotationMatrix(rot3);
		key->setRotation(quat);
		key->setScale(Ogre::Vector3(0.8, 0.8, 0.8));
	}

	/* Create animation state */
	animation_state_ = scene_manager->createAnimationState("Animation");
	animation_state_->setEnabled(true);
	animation_state_->setLoop(true);

	/* Turn on animating flag */
	animating_ = true;
}


bool OgreApplication::frameRenderingQueued(const Ogre::FrameEvent& fe){
  
	/* This event is called after a frame is queued for rendering */
	/* Do stuff in this event since the GPU is rendering and the CPU is idle */
	/* Keep animating if flag is on */
	if (animating_){
		animation_state_->addTime(fe.timeSinceLastFrame);
	}

	/* Capture input */
	keyboard_->capture();
	mouse_->capture();

	/* Handle specific key events */
	if (keyboard_->isKeyDown(OIS::KC_SPACE)){
		space_down_ = true;
	}
	if ((!keyboard_->isKeyDown(OIS::KC_SPACE)) && space_down_){
		animating_ = !animating_;
		space_down_ = false;
	}
	if (keyboard_->isKeyDown(OIS::KC_RETURN)){
		animation_state_->setTimePosition(0);
	}
	if (keyboard_->isKeyDown(OIS::KC_ESCAPE)){
		ogre_root_->shutdown();
		ogre_window_->destroy();
		return false;
	}
	if (keyboard_->isKeyDown(OIS::KC_RIGHT)){
		torus->translate(0.005,0, 0);
	}
	if (keyboard_->isKeyDown(OIS::KC_DOWN)){
		torus->translate(0,-0.005, 0);
	}
	if (keyboard_->isKeyDown(OIS::KC_LEFT)){
		torus->translate(-0.005,0, 0);
	}
	if (keyboard_->isKeyDown(OIS::KC_UP)){
		torus->translate(0,0.005, 0);
		
	}
	if (keyboard_->isKeyDown(OIS::KC_W)){
		torus->translate(0,0, 0.005);
		
	}
	if (keyboard_->isKeyDown(OIS::KC_S)){
		torus->translate(0,0, -0.005);
		
	}
    return true;
}


void OgreApplication::windowResized(Ogre::RenderWindow* rw){

	/* Update the window and aspect ratio when the window is resized */
	int width = rw->getWidth(); 
    int height = rw->getHeight();
      
    Ogre::SceneManager* scene_manager = ogre_root_->getSceneManager("MySceneManager");
    Ogre::Camera* camera = scene_manager->getCamera("MyCamera");

	if (camera != NULL){
		//std::cout << "1 " << (double)width/height << std::endl;
		camera->setAspectRatio((double)width/height);
    }

	const OIS::MouseState &ms = mouse_->getMouseState();
    ms.width = width;
    ms.height = height;

	ogre_window_->resize(width, height);
	ogre_window_->windowMovedOrResized();
	ogre_window_->update();
}


} // namespace ogre_application;
