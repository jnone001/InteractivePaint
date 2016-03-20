This application is dual-licensed. 

License A:  GPL v2 and/or GPL v3

License B: The second license, is a private license available via Dr. Rishe and Dr. Ortega. Write to fortega@cs.fiu.edu for more information. 

# InteractivePaint
ModernTouch4
User Read Me

Multitouch Read Me

	 Without using the Tobii EyeX these UI Boxes should always be showing. 
	If you are currently using the Tobii EyeX you must have your 'gaze' in the vicinity of the UI to have it display as well as work properly.

Top Left Buttons - From left to right


	Color Change Button
		Toggling this button opens up another menu of all the colors that you can change to. Colors correspond to the color inside the boxes.

	Shape Change Button
		Toggling this button opens up another menu of buttons that represent all the currently available shapes 
		Lines
		Circles (Unfilled or Filled depending on the value of the fill button)
		Rectangles (Unfilled or Filled depending on the value of the fill button)
		Triangle (Unfilled or Filled depending on the value of the fill button)

	Toggle Fill Button
		Turns the 'filled' function on and off for some shapes. Circles, Rectangles, and Triangles can all be 'filled' shapes as opposed to just connected lines.
	Line Size Increase Button
		Increases the size of lines as well as non-filled shapes lines.
	Line Size Decrease Button
		Decreases the size of lines as well as non-filled shapes lines.
	Transparency Increase (Purple Button)
		Increases transparency of any shapes or lines you draw 
	Transparency Decrease (Green Button)
		Decreases transparency (makes then more solid) for any shapes or lines you draw.
	Toggle Symmetry (Dotted line Icon)
		Toggles a line of symmetry on and off.
		A dotted white line will split the screen when it is enabled. 
		Any shapes that you draw will be reflected over this Axis.
	Layer Visualization (Two 'Pages' Icon)
		Toggles on and off layer visualization
		These are the three 'layers' you can draw on. Press either of the bottom two layers to move it to the top.
		The top layer is the layer you are currently drawing on.
		
		
Multitouch Gestures 

	Double tap the screen
		Double tapping the screen will open up a radial menu at the fingers location
			Radial Menu - 
				Left button cycles through colors
				Right Button - Cycles through Shapes
				Top button - Toggles on and off symmetry
				Bottom button - toggles on and off UI
				Grey Center Button - Turns off the radial menu.
	More gestures coming soon!
	


Mode Box - Bottom Right

	This box Enables you to see what settings are currently on. 

	Depending on the mode, the box will display a line if you currently have lines active, a Circle if you have Circles Active (Filled circles if filled circles are active) etc. 

	Eraser mode will display a small hollow white circle with the background color inside.
	
	It will also show the shapes in their correct colors as well as line size. 

	Additionality it shows which devices were currently plugged in at launch time of the application. 
		- Blue box in the top left corner of the mode box represents a present touch screen.
		- Green box to the right of the blue box represents a leap motion device is present
		- A Red box further to the right represents the presence of a Tobii EyeX Device. 

Keyboard Shortcuts 

	'Touch Screen' buttons are available as keyboard shortcuts as well.
	Some additional functionality may be found on the keyboard because we have yet to implement it into the UI.

	'q' and 'w' - Cycle through the various colors. 'q' goes backwards while 'w' goes forward. 
	'e' - Toggle Eraser Mode - Turns on the 'eraser' which draws the current background color to the canvas 
		(The 'Mode Box' shows a small white circle that shows the size of the current eraser')
		In addition you must turn off eraser mode before drawing (Retoggle with the 'e' key)
	'r' - Toggle Random Color Mode - Draws in randomly generated colors. Must turn it off to continue drawing in specific colors.
	'z' - Line size decrease - Decreases size of the lines as well as lines that encompass non-filled shapes.
	'x' - Line size Increase - Increases size of the lines as well as lines that ecompass non-filled shapes.
	'c' - Clears the current canvas, including both layers. 
	'v' - Toggles on and off leap motion drawing capabilities
	'b' - Changes the background color. 
	'u' - Turns on line drawing
	'i' - turns on Circle Drawing
	'o' - turns on rectangle drawing
	'p' - turns on triangle drawing
	'f' - toggles filled shapes on and off
	'v' - Toggles on and off the ability to draw with the leap motion device.
		
		
Drawing - 

	Lines
	
		When in line mode simply drag your finger across the touch screen to draw lines. 
		The Program supports as many fingers as your multitouch display does.
	
	Circles 
		When in circle mode simply put your finger where you want the center of the circle to be.
		Drag your finger until the desired size of the circle is created
		
	Rectangles 
		When in rectangle mode place your finger at the location where you want one of the corners.
		Drag your finger until you get your desired rectangle size, then lift.
		
	Triangles
		When in triangle mode, the touch of your finger represents the center of the base.
		Drag your finger to change the height as well as width of the triangle.
		Release your finger when the desired height and width is created.
		


Leap Motion Read Me

	Leap motion device allows a user to draw by using his or her fingers. 
	Also provides gestures that allows the user to change the current color being used
	to draw and the current shape chosen for drawing. The final gesture a keyboard tap gesture 
	allows user to save current canvas.

Leap Motion Drawing
	
	Leap Motion Drawing capabilities
		The leap motion enables a user to draw lines on the screen by using his or her fingers as the drawing tool.
		Currently only line drawing is allowed and the shapes can only be drawn using the multi touch screen. The Interaction 
		UI will show three lines that tells the user the current mode is in line drawing mode.

	Instructions for drawing a line with Leap Motion
		The leap motion allows all ten of a persons fingers to be using to create lines on the canvas. 
		When a person would like to draw he or she should first uses his or her index finger and slowly approach the leap
		motion from the front side of the device. As a person finger enters the leap motions interaction zone a green circle
		will appear when the device reads the finger. 
		A green circle signals to the user that he or she is hovering over a virtual touch screen created by the leap 
		motion device. The green circle will fade away as the person continues to move their finger towards the screen. 
		Once the green circle disappears the person has entered the drawing zone and will be able to draw by moving their
		fingers around the screen. 
		
		When a person wants to finish a line drawn he or she must remove their hand from the leap motion device's 
		interaction zone.

	Leap Motion Gestures
		Leap motion device has three different gestures that allows a user to change a current setting of the 
		program or save an image. 
		
		Since the leap motion device is always running a person may draw and accidently perform a gesture.
		In order to avoid a gesture being read accidently the user can toggle off and on the drawing mode 
		for the leap motion. 
		
		By pressing KeyBoard shortcut 'v' as stated previously the drawing capabilities will be disabled 
		allowing a user to not have to worry about drawing while performing or gesture.
		
	
	Swipe Gesture
		When a user performs a swipe gesture the current color the program is set on will be incremented to the
		next predefined color. 
		
		A successful gesture reading will provide feedback in the form of the new color appearing on the screen 
		and eventually fades out. 
		
		How to Perform a swipe gesture:
		- The swipe gesture can be performed horizontally or vertically
		- The user may start on the left or right side of the leap motion or start at the top or bottom
		- When the user decides to perform the swipe he or she must move from one side of the leap to the opposite side
		- For example: left to right , right to left , top to bottom , bottom to top
		- 
		-The gesture should be one  fluid motion and best down with a single finger pointing toward the screen.
		The gesture is read best by the device when the user keeps their wrist and hand rigid and moves their arm. 
		User should imagine your finger,hand,wrist and arm as one solid object.

	Circle Gesture
		When a user performs a circle gesture the current shape, for drawing , the program is set on will be 
		incremented to next predefined shape. 
		
		A successful gesture reading will provide feedback as a shape image appearing on the screen 
		and eventually fades out. 
		
		How to Perform a circle gesture:
		- The circle gesture can be performed using either the left or right index finger.
		- The user begins the gesture by using their index finger and pointing towards the screen with his 
		- or her hand over the leap motion device.To begin the gesture the user either goes in a clock 
		- or counterclock direction.The user should continue moving only his or her index finger and complete 
		- a 360 degree rotation which will be read by the leap motion as a single circle gesture. 
		- Again it is suggested for the user to keep hand and wrist still and only rotate index finger in 
		- a circle.
		
		User Note:
		Unlike the multi touch in order to have filled shapes the user must continue traverse the possible 
		shapes until the filled shape they desire has been dispalyed.

		Also user should note that shapes can only be drawn with the touch screen.

	
	Keyboard Tap Gesture
		When a user performs a keyboard tap gesture the current canvas is saved to the users computer.
		
		A successful gesture reading will provide feedback in the form a saved file image. 
		
		How to Perform a keyboard tap gesture:
		- The keyboard tap gesture can be performed using either the left or right index finger.
		- The user begins the gesture by using their index finger and pointing towards the screen 
		- with his or her hand over the leap motion device. To perform the gesture the user keeps his 
		- or her wrist and hand still and points the index finger towards the keyboard in a single fluid 
		- motion while keep hand and wrist still. The motion simulates a person taping a keyboard key.
 
	Troublshooting:
	Make sure the leap motion service is running on the machine perform starting the Interactive Paint Program.

