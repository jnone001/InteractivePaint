#include "Illustrator.h"

namespace drawing {
	Illustrator::Illustrator() {}
	Illustrator::Illustrator(Brush* brush, std::vector<std::shared_ptr<gl::Fbo>>* layerList)
	{
		mLayerList = layerList;
		mBrush = brush;
		activeDrawings = 0;

		//Create Map for all Fbo's in Program 
		for (auto layers : (*mLayerList))
		{
			std::list<std::shared_ptr<gl::Fbo>> storedFbo;
			myTimeMachine.insert(make_pair(layers, storedFbo));
		}
	}

	int Illustrator::getActiveDrawings()
	{
		return activeDrawings;
	}

	void Illustrator::drawActiveShapes()
	{
		for (auto& activePoint : myActiveCircles)
		{
			activePoint.second.draw();
			if ((*(*mBrush).getSymmetry()).getSymmetryOn()) (*(*mBrush).getSymmetry()).symmetricCircle(activePoint.second).draw();
		}
		for (auto& activePoint : myActiveRectangles)
		{
			activePoint.second.draw();
			if ((*(*mBrush).getSymmetry()).getSymmetryOn()) (*(*mBrush).getSymmetry()).symmetricRectangle(activePoint.second).draw();
		}
		for (auto& activePoint : myActiveTriangles)
		{
			activePoint.second.draw();
			if ((*(*mBrush).getSymmetry()).getSymmetryOn()) (*(*mBrush).getSymmetry()).symmetricTriangle(activePoint.second).draw();
		}
		for (auto& activePoint : myActivePointsEraser)
		{
			activePoint.second.draw();
		}

		for (auto& activePoint : myActiveCirclesEraser)
		{
			activePoint.second.draw();
		}
	}

	/*Bresenhams Line Algorithm for smooth lines*/
	void Illustrator::missedPoints(int xi, int yi, int xf, int yf, TouchPoint& points)
	{
		int dx, dy, x, y, d, ds, dt;
		//Find Abs value of difference between x and y coords of two points
		dx = abs(xi - xf);
		dy = abs(yi - yf);
		//Ceck which coordinates have bigger range x or y
		if (dx >= dy)
		{
			d = 2 * dy - dx;
			ds = 2 * dy;
			dt = 2 * (dy - dx);
			//Figure out which point to use as starting point
			if (xi < xf)
			{
				x = xi;
				y = yi;
			}
			else
			{
				x = xf;
				y = yf;
				xf = xi;
				yf = yi;
			}

			//Would be starting point passed to method
			points.addPoint(vec2(x, y));

			while (x < xf)
			{
				if (d < 0)
				{
					d += ds;
				}
				else
				{
					if (y < yf)
					{
						y++;
						d += dt;
					}
					else
					{
						y--;
						d += dt;
					}
				}

				x++;

				points.addPoint(vec2(x, y));
			}
		}
		else
		{
			d = 2 * dx - dy;
			ds = 2 * dx;
			dt = 2 * (dx - dy);
			//Figure out which point to use as starting point
			if (yi < yf)
			{
				x = xi;
				y = yi;
			}
			else
			{
				x = xf;
				y = yf;
				xf = xi;
				yf = yi;
			}

			points.addPoint(vec2(x, y));

			while (y < yf)
			{
				if (d < 0)
				{
					d += ds;
				}
				else
				{
					if (x > xf)
					{
						x--;
						d += dt;
					}
					else
					{
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

		if ((*mBrush).getEraserMode())
		{
			activeDrawings++;
			myActivePoints.insert(make_pair(myId, TouchPoint(myPos, (*mBrush).getColor(), (*mBrush).getLineSize() * 2)));
			bool tempBool = false;
			myActiveCirclesEraser.insert(make_pair(myId, TouchCircle(myPos, (*mBrush).getLineSize() * 2, Color(0.0, 0.0, 0.0), 1, tempBool)));
		}
		else
		{
			switch ((*mBrush).getShape())
			{
			case Shape::Shape::Line:
				activeDrawings++;
				if ((*mBrush).getRandColor())
				{
					ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, (*mBrush).getAlphaColor());
					myActivePoints.insert(make_pair(myId, TouchPoint(myPos, newColor, (*mBrush).getLineSize())));
				}
				else
				{
					myActivePoints.insert(make_pair(myId, TouchPoint(myPos, (*mBrush).getColor(), (*mBrush).getLineSize())));
				}
				break;

			case Shape::Shape::Circle:
				activeDrawings++;
				if ((*mBrush).getRandColor())
				{
					ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, (*mBrush).getAlphaColor());
					bool filledShapes = (*mBrush).getFilledShapes();
					myActiveCircles.insert(make_pair(myId, TouchCircle(myPos, 30.0f, newColor, (*mBrush).getLineSize(), filledShapes)));
				}
				else
				{
					bool filledShapes = (*mBrush).getFilledShapes();
					myActiveCircles.insert(make_pair(myId, TouchCircle(myPos, 30.0f, (*mBrush).getColor(), (*mBrush).getLineSize(), filledShapes)));
				}
				break;

			case Shape::Shape::Rectangle:
				activeDrawings++;
				if ((*mBrush).getRandColor())
				{
					ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, (*mBrush).getAlphaColor());
					bool filledShapes = (*mBrush).getFilledShapes();
					myActiveRectangles.insert(make_pair(myId, TouchRectangle(myPos.x, myPos.y, myPos.x, myPos.y, newColor, (*mBrush).getLineSize(), filledShapes)));
				}
				else
				{
					bool filledShapes = (*mBrush).getFilledShapes();
					myActiveRectangles.insert(make_pair(myId, TouchRectangle(myPos.x, myPos.y, myPos.x, myPos.y, (*mBrush).getColor(), (*mBrush).getLineSize(), filledShapes)));
				}

			case Shape::Shape::Triangle:
				activeDrawings++;
				if ((*mBrush).getRandColor())
				{
					ColorA newColor(CM_HSV, Rand::randFloat(), 0.5f, 1.0f, (*mBrush).getAlphaColor());
					bool filledShapes = (*mBrush).getFilledShapes();
					myActiveTriangles.insert(make_pair(myId, TouchTriangle(myPos, myPos, myPos, myPos, newColor, (*mBrush).getLineSize(), filledShapes)));
				}
				else
				{
					bool filledShapes = (*mBrush).getFilledShapes();
					myActiveTriangles.insert(make_pair(myId, TouchTriangle(myPos, myPos, myPos, myPos, (*mBrush).getColor(), (*mBrush).getLineSize(), filledShapes)));
				}
			}
		}
	}

	void Illustrator::movingTouchShapes(uint32_t myId, vec2 myPos, vec2 prevPos)
	{
		if ((*mBrush).getEraserMode())
		{
			if (myActivePoints.find(myId) == myActivePoints.end()) return;

			missedPoints(prevPos.x, prevPos.y, myPos.x, myPos.y, myActivePoints[myId]);
			myPoints.push_back(myActivePoints[myId]);
			myActivePoints[myId].clearPoints();

			myActiveCirclesEraser[myId].changePosition(myPos);
			//Draws to the layer at the end of the list. Which is drawn on 'top'
			(*(*mLayerList).back()).bindFramebuffer();

			glEnable(GL_BLEND);
			glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);

			for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();)
			{
				oldPoints->draw();
				if ((*(*mBrush).getSymmetry()).getSymmetryOn()) (*((*mBrush).getSymmetry())).symmetricLine(*oldPoints).draw();
				++oldPoints;
			}

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glBlendEquation(GL_FUNC_ADD);

			(*(*mLayerList).back()).unbindFramebuffer();

			myPoints.clear();
		}
		else
		{
			switch ((*mBrush).getShape())
			{
			case Shape::Shape::Line:
				if (myActivePoints.find(myId) == myActivePoints.end()) return;

				missedPoints(prevPos.x, prevPos.y, myPos.x, myPos.y, myActivePoints[myId]);
				myPoints.push_back(myActivePoints[myId]);
				myActivePoints[myId].clearPoints();

				//Draws to the layer at the end of the list. Which is drawn on 'top'
				(*(*mLayerList).back()).bindFramebuffer();
				for (auto oldPoints = myPoints.begin(); oldPoints != myPoints.end();)
				{
					oldPoints->draw();
					if ((*(*mBrush).getSymmetry()).getSymmetryOn()) (*((*mBrush).getSymmetry())).symmetricLine(*oldPoints).draw();
					++oldPoints;
				}
				(*(*mLayerList).back()).unbindFramebuffer();
				myPoints.clear();
				break;

			case Shape::Shape::Circle:

				if (myActiveCircles.find(myId) == myActiveCircles.end()) return;
				float a;
				float b;
				float c;
				a = myPos.x - myActiveCircles[myId].getCenterX();
				b = myPos.y - myActiveCircles[myId].getCenterY();
				a = a * a;
				b = b * b;
				c = sqrt(a + b);

				myActiveCircles[myId].changeRadius(c);
				break;

			case Shape::Shape::Rectangle:
			{
				if (myActiveRectangles.find(myId) == myActiveRectangles.end()) return;
				myActiveRectangles[myId].changePoint(myPos.x, myPos.y);
			}
			break;
			case Shape::Shape::Triangle:
			{
				if (myActiveTriangles.find(myId) == myActiveTriangles.end()) return;
				myActiveTriangles[myId].changeSize(myPos.x, myPos.y);
			}
			break;
			}
		}
	}

	void Illustrator::endTouchShapes(uint32_t myId)
	{
		if ((*mBrush).getEraserMode())
		{
			if (myActivePoints.find(myId) == myActivePoints.end()) return;

			activeDrawings--;
			myActivePoints.erase(myId);
			myActiveCirclesEraser.erase(myId);
		}

		switch ((*mBrush).getShape())
		{
		case Shape::Shape::Line:
		{
			if (myActivePoints.find(myId) == myActivePoints.end()) return;

			activeDrawings--;
			myActivePoints.erase(myId);
		}
		case Shape::Shape::Circle:
		{
			if (myActiveCircles.find(myId) == myActiveCircles.end()) return;

			activeDrawings--;

			myCircles.push_back(myActiveCircles[myId]);

			//Draws to the layer at the end of the list. Which is drawn on 'top'
			(*(*mLayerList).back()).bindFramebuffer();
			for (auto oldPoints = myCircles.begin(); oldPoints != myCircles.end();)
			{
				oldPoints->draw();
				if ((*((*mBrush).getSymmetry())).getSymmetryOn()) (*((*mBrush).getSymmetry())).symmetricCircle(*oldPoints).draw();
				++oldPoints;
			}
			(*(*mLayerList).back()).unbindFramebuffer();
			myActiveCircles.erase(myId);

			myCircles.clear();
		}
		case Shape::Shape::Rectangle:
		{
			if (myActiveRectangles.find(myId) == myActiveRectangles.end()) return;

			activeDrawings--;

			myRectangles.push_back(myActiveRectangles[myId]);

			//Draws to the layer at the end of the list. Which is drawn on 'top'
			(*(*mLayerList).back()).bindFramebuffer();
			for (auto oldPoints = myRectangles.begin(); oldPoints != myRectangles.end();)
			{
				oldPoints->draw();
				if ((*((*mBrush).getSymmetry())).getSymmetryOn()) (*((*mBrush).getSymmetry())).symmetricRectangle(*oldPoints).draw();
				++oldPoints;
			}
			(*(*mLayerList).back()).unbindFramebuffer();
			myActiveRectangles.erase(myId);

			myRectangles.clear();
		}
		case Shape::Shape::Triangle:
		{
			if (myActiveTriangles.find(myId) == myActiveTriangles.end()) return;

			activeDrawings--;

			myTriangles.push_back(myActiveTriangles[myId]);
			myActiveTriangles.erase(myId);
			//Draws to the layer at the end of the list. Which is drawn on 'top'
			(*(*mLayerList).back()).bindFramebuffer();
			for (auto oldPoints = myTriangles.begin(); oldPoints != myTriangles.end();)
			{
				oldPoints->draw();
				if ((*((*mBrush).getSymmetry())).getSymmetryOn()) (*((*mBrush).getSymmetry())).symmetricTriangle(*oldPoints).draw();
				++oldPoints;
			}
			(*(*mLayerList).back()).unbindFramebuffer();
			myActiveTriangles.erase(myId);

			myTriangles.clear();
		}
		}
	}

	//Need to add color background to parameter
	void Illustrator::saveCurrentFbo()
	{
		if (activeDrawings > 0)
		{
			return;
		}
		//Check if the active drawing is occuring

		if (myTimeMachine[(*mLayerList).back()].size() == 100)
		{
			myTimeMachine[(*mLayerList).back()].pop_back();
		}

		std::shared_ptr<gl::Fbo> tempFbo;

		gl::Fbo::Format format;
		tempFbo = gl::Fbo::create((*(*mLayerList).back()).getSize().x
			, (*(*mLayerList).back()).getSize().y
			, format);

		(*tempFbo).bindFramebuffer();

		glClearColor(1.0, 1.0, 1.0, 0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		gl::color(1.0, 1.0, 1.0, 1.0);
		gl::draw((*(*mLayerList).back()).getColorTexture());
		(*tempFbo).unbindFramebuffer();

		//Place current fbo on list
		myTimeMachine[(*mLayerList).back()].emplace_front(tempFbo);
	}

	void Illustrator::undoDraw(Color background)
	{
		if (myTimeMachine[(*mLayerList).back()].size() != 0)
		{
			(*(*mLayerList).back()).bindFramebuffer();

			glClearColor(background.r, background.g, background.b, 0.0);
			glClear(GL_COLOR_BUFFER_BIT);
			gl::color(1.0, 1.0, 1.0, 1.0);

			gl::draw((*myTimeMachine[(*mLayerList).back()].front()).getColorTexture());

			(*(*mLayerList).back()).unbindFramebuffer();

			myTimeMachine[(*mLayerList).back()].pop_front();
		}
	}

	void Illustrator::clearTimeMachine()
	{
		//Working
		myTimeMachine.clear();
		for (auto layers : (*mLayerList))
		{
			std::list<std::shared_ptr<gl::Fbo>> storedFbo;
			myTimeMachine.insert(make_pair(layers, storedFbo));
		}
	}
}