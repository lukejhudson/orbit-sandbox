# orbit-sandbox
# orbit-sandbox #
## About ##
A simple game built around the force of gravity acting on celestial bodies in space. Written in C++ using the Qt framework.

## Learning Outcomes ##


Simulation
	Gravity equation
	Calculating orbit velocity
	Satisfying to see it work realistically
C++ skills
	Pointers, memory management
		Valgrind
		Gain overall confidence
		Learn about Qt which is a very popular framework
Graphics
	Drawing sprites, simple animations
		Having a camera that can move around and zoom in/out posed some interesting challenges
			Needed to ensure that proportions stayed the same when zooming
			Had to really understand the problem to ensure that the camera offsets were calculated perfectly
				Wanted camera controls to feel accurate and responsive
					Proud of the solution
	Tiling the backgrounds
		A new problem for me
		Making backgrounds tile infinitely and move slightly as the camera moves
			Also attempts to give a sense of parallax
Optimisation
	Can be very large numbers (several thousands) of bodies in the simulation 
		Optimisation very important since performance was severly lacking with this number of bodies
		Kept track of how many ticks were performed each second
			Limited to 60, in a simple simulation like this should always be 60
			Used this metric to determine the effects of different optimisations
	Multi-thread
		Thread safety using mutexes
		1 cpu core maxed out --> parallelise to spread the workload out more evenly, make much better use of multicore and multithread cpus
	Collision detection
		Check if bodies are remotely close to each other
		If so, check if bounding rectangles overlap
		If so and body is on screen and we aren't zoomed out very far, check if sprites overlap
			Simulation less precise when zoomed out (since wouldn't be able to tell anyway)
				Collision detection with bounding rectangles good enough
			Costly but accurate, only calculate when necessary
	Gravitational pull
		Ignore scenarios where the resulting pull is practically 0
			Tiny body vs massive body
			Bodies super far apart
		Uses costly square root function so want to minimise the number of times the force needs to be calculated
	General optimisations such as reducing the number of times variables need to be fetched

## Tour of the Software ##

![Main Menu](Screenshots/MainMenu.png)

There are currently two game modes available: Sandbox mode and Exploration mode.

![Sandbox Mode](Screenshots/SandboxMode1.png)

In Sandbox mode the player has the ability to spawn a number of celestial bodies using the panel on the right of the screen. 

![Sandbox Mode - Spawning](Screenshots/SandboxMode2.png)

Once a type of body has been chosen, click and drag on the screen to spawn it. The further the mouse is dragged, the greater the body's velocity as it spawns.

![Sandbox Mode - Many Bodies](Screenshots/SandboxMode4.png)

Spawn many bodies at once by selecting the 'Planetary System' body type. This spawns a large celestial body (randomly chosen between a star, white dwarf or black hole) and then a number of planets in orbit around this central body, and in turn spawns asteroids in orbit around those planets. Observe the bodies orbit around each other and combine together when they collide. Zoom in or out by using the scroll wheel, and move the camera around by holding the middle mouse button and dragging the mouse.

![Exploration Mode](Screenshots/ExplorationMode1.png)

In Exploration mode the player controls a rocket placed in the centre of the screen. The rocket is initially spawned orbiting a planet within a planetary system. 

![Exploration Mode - Controlling the Rocket](Screenshots/ExplorationMode2.png)

Press and hold W to fire the rocket's engines and increase its velocity in the direction it is currently facing. Use the A and D keys to rotate the rocket anti-clockwise and clockwise respectively. The panel in the bottom right of the screen shows the rocket's speed, angle and direction of travel. 

![Exploration Mode - Exploring the Universe](Screenshots/ExplorationMode3.png)

As the rocket moves around, planetary systems are procedurally generated to provide an endless universe to explore.

![Exploration Mode - Crash](Screenshots/ExplorationMode4.png)

But try not to crash or the game will end!

![Controls Screen](Screenshots/ControlsScreen.png)

A recap of the game modes and the controls available in each can be found in the controls screen.
