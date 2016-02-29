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
	'l' - Changes between the two layers. 
		The application currently has two layers. There is no UI Representation for which layer is currently on top at the moment. 
		
		
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
