This application is dual-licensed.

License A:  GPL v2 and/or GPL v3

License B: The second license, is a private license available via Dr. Rishe and Dr. Ortega. Write to fortega@cs.fiu.edu for more information.



![](https://cloud.githubusercontent.com/assets/9858188/15053885/c8e0fb9c-12d2-11e6-8890-e6214fa2a825.png)


Multi Modal Interactive Paint is an application designed to showcase various input devices. Currently implemented devices include
	Multitouch Monitor
	Tobii EyeX
	Leap Motion Controller
	Intel Real Sense Camera

The application was designed for the OpenHID lab at Florida International University. It will be an application that sets the baseline for testing new devices on their indevelopment Smart Desk, a desk designed for students that will make use of various input devices.

The application is a painting application, allowing users to draw on a canvas using different shapes, colors, as well as more intricate functionality such as layer and alpha color support.

The software also gives the user some basic control over what they want their active devices to do, for example the Leap Motion can be toggled on to perform Drawing and Hand Gesture Recognition, you can also toggle one of these off so making performing the other simpler.

User Read Me

Multitouch Read Me

	 Without using the Tobii EyeX these UI Boxes should always be showing unless disabled using the Radial Menu.
	If you are currently using the Tobii EyeX you must have your 'gaze' in the vicinity of the UI to have it display as well as work properly.

Drawing -

Drawing with the Multitouch device is simple. Simply drag your fingers accross any non-interactive UI elements. The multitouch will pick up that your finger is moving and then begin to draw a shape. Drawings can be various shapes. Circles, rectangles, and triangles can also be filled as opposed to hollow shapes. Below is a list of the various shapes implemented thus far.

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

Here is our basic user interface :

![image](https://cloud.githubusercontent.com/assets/9858188/15054055/99c1cab6-12d3-11e6-9198-7e89836b47d6.png)



Mode Buttons (Top left of UI) - From left to right


	Color Change Button
		Toggling this button opens up another menu of all the colors that you can change to. Colors correspond to the color inside the boxes.

	Shape Change Button
		Toggling this button opens up another menu of buttons that represent all the currently available shapes
		Lines
		Circles (Unfilled or Filled depending on the value of the fill button)
		Rectangles (Unfilled or Filled depending on the value of the fill button)
		Triangle (Unfilled or Filled depending on the value of the fill button)
	Brush Buttons - Opens the following buttons
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
		These are the three 'layers' you can draw on. Press either of the bottom two layers to swap it with the top.
		The top layer is the layer you are currently drawing on.

		The Menu also allows you to drag a green dot up and down a slider. This will allow you to change the Alpha color of that layer.

![image](https://cloud.githubusercontent.com/assets/9858188/15054100/d582c9a6-12d3-11e6-9768-23c1b99f5197.png)
				Layer Visualization Menu

	'A' Text Icon
		Tap this icon, and then tap anywhere on the screen to begin writing text.
		A Touch Keyboard will appear, simply tap any keys to type.
		You can drag this keyboard around by touching the edges and moving your finger.
		In addition you can hit 'Done' or touch anywhere on the screen without the keyboard interface to close it.

	Undo Button
		The Undo button simply undoes the last drawing that you performed on the currently active layer.

Here is an example of some of these menus

![image](https://cloud.githubusercontent.com/assets/9858188/15054083/c43a0d8a-12d3-11e6-85ed-a8db4aecd0e9.png)


Multitouch Gestures

	Double tap the screen
		Double tapping the screen will open up a radial menu at the fingers location
			Radial Menu -
				Left button cycles through colors
				Right Button - Cycles through Shapes
				Top button - Toggles on and off symmetry
				Bottom button - toggles on and off UI
				Grey Center Button - Turns off the radial menu.

	Extended Touch Gesture
		Press your finger on the multitouch and hold it in place.

		You should then see icons which cycle through various shapes.

	Two Finger Tap Gesture
		Place two fingers together, at about the same height, and tap the screen. Make sure the fingers are separated enough to be detected as two separate taps.

		This gesture will cycle through colors.




Mode Box - Bottom Right of UI

	This box Enables you to see what settings are currently on.

	Depending on the mode, the box will display a line if you currently have lines active, a Circle if you have Circles Active (Filled circles if filled circles are active) etc.

	Eraser mode will display a small hollow white circle with the background color inside.

	It will also show the shapes in their correct colors as well as line size.

	Additionality it shows which devices were currently plugged in at launch time of the application.
		- Blue box in the top left corner of the mode box represents a present touch screen.
		- Green box to the right of the blue box represents a leap motion device is present
		- A Red box further to the right represents the presence of a Tobii EyeX Device.
		- A Yellow Box indicating the Intel Real Sense Camera is plugged in.

	The mode box also has a 'Cog' button in the bottom left for settings, this menu contains
		A toggle button for Frames Per Second
		A Cycle Backgrounds button

	In addition it has a 'Device Modes' menu located at the top right. Opening it will provide you various options for the implemented devices. Currently a green box means 'On' while a black box means 'Off'

EyeX Guide -
	Simply move your gaze around so the EyeX can pick them up. You can see two gray circles jumping around indicating where you gaze is located.

	Simply gaze to the top left corner to 'draw' the mode buttons, as well as gaze to the bottom right corner to draw the mode box.

	In addition, tapping space with the EyeX enabled will change your shape based on where you are looking.

	![image](https://cloud.githubusercontent.com/assets/9858188/15054183/27b7a58e-12d4-11e6-9a85-455dd248eb89.png)
This image displays the Two circles that show where you gaze is being tracked to. In addition it is close enough to the mode box to pull it up.


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
	'u' - Turns on line drawing
	'i' - turns on Circle Drawing
	'o' - turns on rectangle drawing
	'p' - turns on triangle drawing
	'f' - toggles filled shapes on and off







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


![image](https://cloud.githubusercontent.com/assets/9858188/15054139/f6ea33b8-12d3-11e6-8f88-8321a76c6d4d.png)
						Leap Motion Drawing in Action!

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

Intel Real Sense Guide

	Drawing -
		Drawing can be performed similar to the Leap Motion device. You simply move your hand forward and draw iwth your entire hand. The interface for drawing is very similar, instead of green circles they are simply yellow circles to indicate where the drawing will occur.
	Facial Recognition
		The real sense also provides a facial detection service. We implemented 5 Facial Gesture so far including :

		Kiss Gesture - Toggles User Interface for Mode box and Mode Buttons

		Smile Gesture - Changes Shapes

		Eyebrow Raise Gesture - Calls the undo function

		Puffy Cheeks Gesture - Changes colors

		Tongue Out Gesture - Toggles Symmetry Line.
