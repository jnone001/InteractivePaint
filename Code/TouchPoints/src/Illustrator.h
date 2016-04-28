#ifndef ILLUSTRATOR_H
#define ILLUSTRATOR_H
//Cinder and OpenGL Includes
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/System.h"
#include "cinder/Rand.h"
#include "cinder/Log.h"
#include "cinder/gl/Fbo.h"

//Our own includes
//#include "TouchShapes.h"


//Standard Library Includes
#include <vector>
#include <map>
#include <list>
#include <stack>

#include "TouchShapes.h"
#include "SymmetryLine.h"
#include "Brush.h"
#include "Enums.h"
struct Illustrator{

	Illustrator(){}
	//Creates a brush
	Illustrator(Brush *brush, std::vector<std::shared_ptr<gl::Fbo>>* layerList){
		mLayerList = layerList;
		mBrush = brush;
		activeDrawings = 0;

		//Create Map for all Fbo's in Program 
		for (auto layers : (*mLayerList)) {
			std::list<std::shared_ptr<gl::Fbo>> storedFbo;
			//storedFbo.emplace_back(*layers);
			myTimeMachine.insert(make_pair(layers, storedFbo));
		}
	}


	void beginTouchShapes(uint32_t myId, vec2 myPos);
	void movingTouchShapes(uint32_t myId, vec2 myPos, vec2 prevPos);
	void endTouchShapes(uint32_t myId);
	void missedPoints(int xi, int yi, int xf, int yf, TouchPoint& points);
	int getActiveDrawings();
	//Time Machine
	void saveCurrentFbo();
	void undoDraw(Color background);
	void clearTimeMachine();
	void drawActiveShapes();



private:
	std::vector<std::shared_ptr<gl::Fbo>>* mLayerList;
	Brush* mBrush;
	int activeDrawings;
	map<uint32_t, TouchPoint>		myActivePoints;
	list<TouchPoint>				myPoints;
	map<uint32_t, TouchPoint>		myActivePointsEraser;
	list<TouchPoint>				myPointsEraser;
	map<uint32_t, TouchCircle>		myActiveCirclesEraser;
	list<TouchCircle>				myCirclesEraser;
	map<uint32_t, TouchCircle>		myActiveCircles;
	list<TouchCircle>				myCircles;
	map<uint32_t, TouchRectangle>	myActiveRectangles;
	list<TouchRectangle>			myRectangles;
	map<uint32_t, TouchTriangle>	myActiveTriangles;
	list<TouchTriangle>				myTriangles;

	//TimeMachine 
	map< std::shared_ptr<gl::Fbo>, std::list<std::shared_ptr<gl::Fbo>> > myTimeMachine;
	//Stack that tells which item in map to pop
	stack < std::shared_ptr<gl::Fbo> > undoOrder;
	int undoCount = 0;

};

int Illustrator::getActiveDrawings(){
	return activeDrawings;
}

void Illustrator::drawActiveShapes(){
	for (auto &activePoint : myActiveCircles) {
		activePoint.second.draw();
		if ((*(*mBrush).getSymmetry()).getSymmetryOn())
			(*(*mBrush).getSymmetry()).symmetricCircle(activePoint.second).draw();
	}
	for (auto &activePoint : myActiveRectangles) {
		activePoint.second.draw();
		if ((*(*mBrush).getSymmetry()).getSymmetryOn())
			(*(*mBrush).getSymmetry()).symmetricRectangle(activePoint.second).draw();
	}
	for (auto &activePoint : myActiveTriangles) {
		activePoint.second.draw();
		if ((*(*mBrush).getSymmetry()).getSymmetryOn())
			(*(*mBrush).getSymmetry()).symmetricTriangle(activePoint.second).draw();
	}
	for (auto &activePoint : myActivePointsEraser) {
		activePoint.second.draw();
	}

	for (auto &activePoint : myActiveCirclesEraser) {
		activePoint.second.draw();
		//if (symmetryOn)
		//mySymmetry.symmetricCircle(activePoint.second).draw();
	}
}
/*Bresenhams Line Algorithm for smooth lines*/
void Illustrator::missedPoints(int xi, int yi, int xf, int yf, TouchPoint& points){


	int dx, dy, x, y, d, ds, dt;
	//Find Abs value of difference between x and y coords of two points
	dx = abs(xi - xf);
	dy = abs(yi - yf);
	//Ceck which coordinates have bigger range x or y
	if (dx >= dy){
		d = 2 * dy - dx;
		ds = 2 * dy;
		dt = 2 * (dy - dx);
		//Figure out which point to use as starting point
		if (xi < xf){
			x = xi;
			y = yi;
		}
		else{
			x = xf;
			y = yf;
			xf = xi;
			yf = yi;
		}

		//Would be starting point passed to method
		points.addPoint(vec2(x, y));

		while (x < xf){
			if (d < 0){
				d += ds;
			}
			else{
				if (y < yf){
					y++;
					d += dt;
				}
				else{
					y--;
					d += dt;
				}
			}

			x++;

			points.addPoint(vec2(x, y));
		}
	}
	else{
		d = 2 * dx - dy;
		ds = 2 * dx;
		dt = 2 * (dx - dy);
		//Figure out which point to use as starting point
		if (yi < yf){
			x = xi;
			y = yi;
		}
		else{
			x = xf;
			y = yf;
			xf = xi;
			yf = yi;
		}

		points.addPoint(vec2(x, y));

		while (y < yf){
			if (d < 0){
				d += ds;
			}
			else{
				if (x > xf){
					x--;
					d += dt;
				}
				else{
					x++;
					d += dt;
				}
			}

			y++;

			points.addPoint(vec2(x, y));
		}
	}
}

void Illustrator::beginTouchShapes(uint32_t myId, vec2 myPos)
{
	/*Should go here*/
	saveCurrentFbo();


	if ((*mBrush).getEraserMode()){
		activeDrawings++;
		//ColorA newColor(backgroundArray[currBackground][0], backgroundArray[currBackground][1], backgroundArray[currBackground][2], 1);
		myActivePoints.insert(make_pair(myId, TouchPoint(myPos, (*mBrush).getColor(), (*mBrush).getLineSize() * 2)));
		bool tempBool = false;
		myActiveCirclesEraser.insert(make_pair(myId, TouchCircle(myPos, (*mBrush).getLineSize() * 2, Color(0.0, 0.0, 0.0), 1, tempBool)));
	}
	else {
		switch ((*mBrush).getShape()){
		case Shape::Shape::Line:
			activeDrawings++;
			if ((*mBrush).getRandColor()){
				ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, (*mBrush).getAlphaColor());
				myActivePoints.insert(make_pair(myId, TouchPoint(myPos, newColor, (*mBrush).getLineSize())));
			}
			else {
				//ColorA newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2], currentAlpha);
				myActivePoints.insert(make_pair(myId, TouchPoint(myPos, (*mBrush).getColor(), (*mBrush).getLineSize())));
			}
			break;

		case Shape::Shape::Circle:
			activeDrawings++;
			if ((*mBrush).getRandColor()){
				ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, (*mBrush).getAlphaColor());
				bool filledShapes = (*mBrush).getFilledShapes();
				myActiveCircles.insert(make_pair(myId, TouchCircle(myPos, 30.0f, newColor, (*mBrush).getLineSize(), filledShapes)));
			}
			else {
				//ColorA newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2], currentAlpha);
				bool filledShapes = (*mBrush).getFilledShapes();
				myActiveCircles.insert(make_pair(myId, TouchCircle(myPos, 30.0f, (*mBrush).getColor(), (*mBrush).getLineSize(), filledShapes)));
			}
			break;


		case Shape::Shape::Rectangle:
			activeDrawings++;
			if ((*mBrush).getRandColor()){
				ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, (*mBrush).getAlphaColor());
				bool filledShapes = (*mBrush).getFilledShapes();
				myActiveRectangles.insert(make_pair(myId, TouchRectangle(myPos.x, myPos.y, myPos.x, myPos.y, newColor, (*mBrush).getLineSize(), filledShapes)));
			}
			else {
				//ColorA newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2], currentAlpha);
				bool filledShapes = (*mBrush).getFilledShapes();
				myActiveRectangles.insert(make_pair(myId, TouchRectangle(myPos.x, myPos.y, myPos.x, myPos.y, (*mBrush).getColor(), (*mBrush).getLineSize(), filledShapes)));
			}

		case Shape::Shape::Triangle:
			activeDrawings++;
			if ((*mBrush).getRandColor()){
				ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, (*mBrush).getAlphaColor());
				bool filledShapes = (*mBrush).getFilledShapes();
				myActiveTriangles.insert(make_pair(myId, TouchTriangle(myPos, myPos, myPos, myPos, newColor, (*mBrush).getLineSize(), filledShapes)));
			}
			else {
				//ColorA newColor(colorArray[currColor][0], colorArray[currColor][1], colorArray[currColor][2], currentAlpha);
				bool filledShapes = (*mBrush).getFilledShapes();
				myActiveTriangles.insert(make_pair(myId, TouchTriangle(myPos, myPos, myPos, myPos, (*mBrush).getColor(), (*mBrush).getLineSize(), filledShapes)));
			}

		}
	}
}
void Illustrator::movingTouchShapes(uint32_t myId, vec2 myPos, vec2 prevPos)
{
	if ((*mBrush).getEraserMode()){
		if (myActivePoints.find(myId) == myActivePoints.end())
			return;


		missedPoints(prevPos.x, prevPos.y, myPos.x, myPos.y, myActivePoints[myId]);
		myPoints.push_back(myActivePoints[myId]);
		myActivePoints[myId].clearPoints();

		myActiveCirclesEraser[myId].changePosition(myPos);
		//Draws to the layer at the end of the list. Which is drawn on 'top'
		(*(*mLayerList).back()).bindFramebuffer();
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		
		for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
			oldPoints->draw();
			if ((*(*mBrush).getSymmetry()).getSymmetryOn())
				(*((*mBrush).getSymmetry())).symmetricLine(*oldPoints).draw();
			++oldPoints;
		}
		
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBlendEquation(GL_FUNC_ADD);
		
		(*(*mLayerList).back()).unbindFramebuffer();


		
		/*
		if (currLayer == 0){


		(*firstFbo).bindFramebuffer();

		for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
		oldPoints->draw();
		++oldPoints;
		}

		(*firstFbo).unbindFramebuffer();
		}
		else if (currLayer == 1){
		(*secondFbo).bindFramebuffer();

		for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
		oldPoints->draw();
		++oldPoints;
		}

		(*secondFbo).unbindFramebuffer();
		}
		*/

		myPoints.clear();

	}
	else {
		switch ((*mBrush).getShape()){
		case Shape::Shape::Line:
			if (myActivePoints.find(myId) == myActivePoints.end())
				return;


			missedPoints(prevPos.x, prevPos.y, myPos.x, myPos.y, myActivePoints[myId]);
			myPoints.push_back(myActivePoints[myId]);
			myActivePoints[myId].clearPoints();

			//Draws to the layer at the end of the list. Which is drawn on 'top'
			(*(*mLayerList).back()).bindFramebuffer();
			for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
				oldPoints->draw();
				if ((*(*mBrush).getSymmetry()).getSymmetryOn())
					(*((*mBrush).getSymmetry())).symmetricLine(*oldPoints).draw();
				++oldPoints;
			}
			(*(*mLayerList).back()).unbindFramebuffer();
			/*
			if (currLayer == 0){


			(*firstFbo).bindFramebuffer();

			for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
			oldPoints->draw();
			if (symmetryOn)
			mySymmetry.symmetricLine(*oldPoints).draw();
			++oldPoints;
			}

			(*firstFbo).unbindFramebuffer();
			}
			else if (currLayer == 1){
			(*secondFbo).bindFramebuffer();

			for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();) {
			oldPoints->draw();
			if (symmetryOn)
			mySymmetry.symmetricLine(*oldPoints).draw();
			++oldPoints;
			}

			(*secondFbo).unbindFramebuffer();
			}
			*/
			myPoints.clear();
			break;



		case Shape::Shape::Circle:

			if (myActiveCircles.find(myId) == myActiveCircles.end())
				return;
			float a;
			float b;
			float c;
			a = myPos.x - myActiveCircles[myId].getCenterX();
			b = myPos.y - myActiveCircles[myId].getCenterY();
			a = a*a;
			b = b*b;
			c = sqrt(a + b);

			myActiveCircles[myId].changeRadius(c);
			break;

		case Shape::Shape::Rectangle:
		{
										if (myActiveRectangles.find(myId) == myActiveRectangles.end())
											return;
										myActiveRectangles[myId].changePoint(myPos.x, myPos.y);



		}
			break;
		case Shape::Shape::Triangle:
		{
									   if (myActiveTriangles.find(myId) == myActiveTriangles.end())
										   return;
									   myActiveTriangles[myId].changeSize(myPos.x, myPos.y);



		}
			break;
		}
	}
}
void Illustrator::endTouchShapes(uint32_t myId)
{
	if ((*mBrush).getEraserMode()){

		if (myActivePoints.find(myId) == myActivePoints.end())
			return;

		activeDrawings--;
		myActivePoints.erase(myId);
		myActiveCirclesEraser.erase(myId);
	}

	switch ((*mBrush).getShape()){
	case Shape::Shape::Line: {

			if (myActivePoints.find(myId) == myActivePoints.end())
				return;

			activeDrawings--;
			myActivePoints.erase(myId);

		}
	case Shape::Shape::Circle: {

			if (myActiveCircles.find(myId) == myActiveCircles.end())
				return;

			activeDrawings--;

			myCircles.push_back(myActiveCircles[myId]);
			
			
			//Draws to the layer at the end of the list. Which is drawn on 'top'
			(*(*mLayerList).back()).bindFramebuffer();
			for (auto oldPoints = myCircles.begin(); oldPoints != myCircles.end();) {
				oldPoints->draw();
				if ((*((*mBrush).getSymmetry())).getSymmetryOn())
					(*((*mBrush).getSymmetry())).symmetricCircle(*oldPoints).draw();
				++oldPoints;
			}
			(*(*mLayerList).back()).unbindFramebuffer();
			myActiveCircles.erase(myId);
			/*
			if (currLayer == 0){


			(*firstFbo).bindFramebuffer();
			for (auto oldPoints = myCircles.begin(); oldPoints != myCircles.end();) {
			gl::color(1.0, 1.0, 1.0, 1.0);
			oldPoints->draw();
			if (symmetryOn)
			mySymmetry.symmetricCircle(*oldPoints).draw();
			++oldPoints;
			}

			(*firstFbo).unbindFramebuffer();
			}
			else if (currLayer == 1){
			(*secondFbo).bindFramebuffer();

			for (auto oldPoints = myCircles.begin(); oldPoints != myCircles.end();) {
			gl::color(1.0, 1.0, 1.0, 1.0);
			oldPoints->draw();
			if (symmetryOn)
			mySymmetry.symmetricCircle(*oldPoints).draw();
			++oldPoints;
			}

			(*secondFbo).unbindFramebuffer();
			}
			*/
			myCircles.clear();

		}
	case Shape::Shape::Rectangle: {

			if (myActiveRectangles.find(myId) == myActiveRectangles.end())
				return;

			activeDrawings--;

			myRectangles.push_back(myActiveRectangles[myId]);
			
			//Draws to the layer at the end of the list. Which is drawn on 'top'
			(*(*mLayerList).back()).bindFramebuffer();
			for (auto oldPoints = myRectangles.begin(); oldPoints != myRectangles.end();) {
				oldPoints->draw();
				if ((*((*mBrush).getSymmetry())).getSymmetryOn())
					(*((*mBrush).getSymmetry())).symmetricRectangle(*oldPoints).draw();
				++oldPoints;
			}
			(*(*mLayerList).back()).unbindFramebuffer();
			myActiveRectangles.erase(myId);
			/*
			if (currLayer == 0){


			(*firstFbo).bindFramebuffer();
			for (auto oldPoints = myRectangles.begin(); oldPoints != myRectangles.end();) {
			oldPoints->draw();
			if (symmetryOn)
			mySymmetry.symmetricRectangle(*oldPoints).draw();
			++oldPoints;
			}

			(*firstFbo).unbindFramebuffer();
			}
			else if (currLayer == 1){
			(*secondFbo).bindFramebuffer();

			for (auto oldPoints = myRectangles.begin(); oldPoints != myRectangles.end();) {
			oldPoints->draw();
			if (symmetryOn)
			mySymmetry.symmetricRectangle(*oldPoints).draw();
			++oldPoints;
			}

			(*secondFbo).unbindFramebuffer();
			}
			*/
			myRectangles.clear();
		}
	case Shape::Shape::Triangle:{

			if (myActiveTriangles.find(myId) == myActiveTriangles.end())
				return;

			activeDrawings--;

			myTriangles.push_back(myActiveTriangles[myId]);
			myActiveTriangles.erase(myId);
			//Draws to the layer at the end of the list. Which is drawn on 'top'
			(*(*mLayerList).back()).bindFramebuffer();
			for (auto oldPoints = myTriangles.begin(); oldPoints != myTriangles.end();) {
				oldPoints->draw();
				if ((*((*mBrush).getSymmetry())).getSymmetryOn())
					(*((*mBrush).getSymmetry())).symmetricTriangle(*oldPoints).draw();
				++oldPoints;
			}
			(*(*mLayerList).back()).unbindFramebuffer();
			myActiveTriangles.erase(myId);
			/*
			if (currLayer == 0){


			(*firstFbo).bindFramebuffer();

			for (auto oldPoints = myTriangles.begin(); oldPoints != myTriangles.end();) {
			oldPoints->draw();
			if (symmetryOn)
			mySymmetry.symmetricTriangle(*oldPoints).draw();
			++oldPoints;
			}

			(*firstFbo).unbindFramebuffer();
			}
			else if (currLayer == 1){
			(*secondFbo).bindFramebuffer();

			for (auto oldPoints = myTriangles.begin(); oldPoints != myTriangles.end();) {
			oldPoints->draw();
			if (symmetryOn)
			mySymmetry.symmetricTriangle(*oldPoints).draw();
			++oldPoints;
			}

			(*secondFbo).unbindFramebuffer();
			}
			*/
			myTriangles.clear();
		}
	}
}
//Need to add color background to parameter
void Illustrator::saveCurrentFbo(){
	if (activeDrawings > 0){
		return;
	}
	//Check if the active drawing is occuring

	if (myTimeMachine[(*mLayerList).back()].size() == 100){
		myTimeMachine[(*mLayerList).back()].pop_back();
	}

	std::shared_ptr<gl::Fbo> tempFbo;


	gl::Fbo::Format format;
	tempFbo = gl::Fbo::create((*(*mLayerList).back()).getSize().x
	, (*(*mLayerList).back()).getSize().y
	, format);

	(*tempFbo).bindFramebuffer();

	//glClearColor(background.r, background.g, background.b, 0.0);
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	gl::color(1.0, 1.0, 1.0, 1.0);
	gl::draw((*(*mLayerList).back()).getColorTexture());
	//gl::drawSolidCircle(vec2(500, 500), 100);
	(*tempFbo).unbindFramebuffer();



	/*
		std::shared_ptr<gl::Fbo> tempFbo;
		//*tempFbo = (*(*mLayerList).back());
		gl::Fbo::Format format;
		tempFbo = gl::Fbo::create(1920, 1080, format);

		
		*/
		//Place current fbo on list


		myTimeMachine[(*mLayerList).back()].emplace_front(tempFbo);
		/*
		undoOrder.push((*mLayerList).back());

		if (undoCount < 3){
			undoCount++;
		}
		*/
	}


void Illustrator::undoDraw(Color background){
	

	if (myTimeMachine[(*mLayerList).back()].size() != 0){

		//gl::Fbo tempFbo = (*myTimeMachine[(*mLayerList).back()].front());

		(*(*mLayerList).back()).bindFramebuffer();

		glClearColor(background.r, background.g, background.b, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);
		gl::color(1.0, 1.0, 1.0, 1.0);

		gl::draw((*myTimeMachine[(*mLayerList).back()].front()).getColorTexture());

		(*(*mLayerList).back()).unbindFramebuffer();


		myTimeMachine[(*mLayerList).back()].pop_front();
	}


	/*
	if (!(undoOrder.empty())){

		undoOrder.pop();
		if (undoCount > 0){
			undoCount--;
		}
		return tempFbo;
	}
	else{
		return (*(*mLayerList).back());
	}
	*/

}

void Illustrator::clearTimeMachine(){
	//Working
	myTimeMachine.clear();
	for (auto layers : (*mLayerList)) {
		std::list<std::shared_ptr<gl::Fbo>> storedFbo;
		//storedFbo.emplace_back(*layers);
		myTimeMachine.insert(make_pair(layers, storedFbo));
	}
	/* not working for some reason
	for (auto machines : myTimeMachine){
		machines.second.clear();
	}
	*/
}


#endif