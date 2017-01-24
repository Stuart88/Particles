//#include "functions.h"

#include <SFML\Graphics.hpp>
#include <iostream>

//initialisation parameters
float gravity = 0;
int gcount = 0;
int playermassMultiplier = 1;
int ballmassMultiplier = 1;
int ballsAmount = 10;
bool mPressed, bPressed, pPressed, infoToggle, EfieldToggle, freezeToggle = 0;
int antigravplayer = 1;
sf::String playermassString, ballsmassString, ballsnumberString, gravString;
struct particle
{
	float mass;
	float vx;
	float vy;
	float px;
	float py;
	float Ex;
	float Ey;
}balls[101];



//FUNCTIONS

void checkCollision(sf::Sprite &ballSprite1, sf::Sprite &ballSprite2, particle &ball1, particle &ball2)
{
	sf::FloatRect boundingBox = ballSprite1.getGlobalBounds();
	sf::FloatRect boundingBox2 = ballSprite2.getGlobalBounds();

	float workingvx1 = ball1.vx;
	float workingvy1 = ball1.vy;
	float workingvx2 = ball2.vx;
	float workingvy2 = ball2.vy;
	float reducedmass = (ball1.mass - ball2.mass) / (ball1.mass + ball2.mass);
	float combinedmass = (ball1.mass + ball2.mass);
	float xDiff = ballSprite2.getPosition().x - ballSprite1.getPosition().x;
	float yDiff = ballSprite2.getPosition().y - ballSprite1.getPosition().y;
	float angle = atan2f(yDiff, xDiff);

	if (boundingBox.contains(ballSprite2.getPosition()))
	{
		//separate balls (sometimes they end up one inside the other)
		ballSprite2.setPosition(ballSprite2.getPosition().x + 6*cosf(angle), ballSprite2.getPosition().y + 6 * sinf(angle));
		//loss in energy due to collision
		ball1.vx *= 0.9;
		ball2.vx *= 0.9;
		ball1.vy *= 0.9;
		ball2.vy *= 0.9;
		//resultant momentum exchange
		ball1.vx = reducedmass*workingvx1 + (2 * workingvx2 * (ball2.mass) / combinedmass);
		ball1.vy = reducedmass*workingvy1 + (2 * workingvy2 * (ball2.mass) / combinedmass);
		ball2.vx = (2 * workingvx1 * (ball1.mass) / combinedmass) - reducedmass*workingvx2;
		ball2.vy = (2 * workingvy1 * (ball1.mass) / combinedmass) - reducedmass*workingvy2;

	}

}

void movePlayerball(sf::Sprite &ballSprite, particle &ball, const sf::RenderWindow &window)
{
	//check collision with side walls, bounce ball off
	if ((ballSprite.getPosition().x > window.getSize().x - 10) || (ballSprite.getPosition().x < 10))
	{
		ball.vx *= 0.9;
		ball.vx = -ball.vx;
	}
	if ((ballSprite.getPosition().y > window.getSize().y - 10) || (ballSprite.getPosition().y < 10))
	{
		ball.vy *= 0.9;
		ball.vy = -ball.vy;
	}

	//Fix issue of balls sticking to or sinking through walls sometimes
	if (ballSprite.getPosition().x < 10)
		ballSprite.setPosition(11, ballSprite.getPosition().y);
	if (ballSprite.getPosition().x > window.getSize().x - 10)
		ballSprite.setPosition(window.getSize().x - 10, ballSprite.getPosition().y);
	if (ballSprite.getPosition().y < 10)
		ballSprite.setPosition(ballSprite.getPosition().x, 11);
	if (ballSprite.getPosition().y > window.getSize().y - 10)
	{
		ball.vy = -abs(ball.vy);
		ball.vx *= 0.97; // friction so balls slow naturally on floor surface
	}

	//gravitational acceleration
	if (ballSprite.getPosition().y < window.getSize().y - 10)
		ball.vy = ball.vy + 0.05*gravity*antigravplayer;

	//move ball
	ballSprite.move(ball.vx, ball.vy);

}

void moveBalls(sf::Sprite &ballSprite, particle &ball, const sf::RenderWindow &window)
{
	//check collision with side walls, bounce ball off
	if ((ballSprite.getPosition().x > window.getSize().x - 10) || (ballSprite.getPosition().x < 10))
	{
		ball.vx *= 0.9; // energy loss
		ball.vx = -ball.vx;
	}
	//check collision with ceiling and floor, bounce off
	if ((ballSprite.getPosition().y > window.getSize().y - 10) || (ballSprite.getPosition().y < 10))
	{
		ball.vy *= 0.9; // energy loss
		ball.vy = -ball.vy;
		if (ballSprite.getPosition().y > window.getSize().y -10)
		{
			ball.vx *= 0.97; // friction so balls slow naturally on floor surface
			ball.vy *= 0.9;
		}
	}
	
	//Fix issue of balls sticking to or sinking through walls sometimes
	if (ballSprite.getPosition().x < 9)//left wall
		ballSprite.setPosition(9, ballSprite.getPosition().y);
	if (ballSprite.getPosition().x > window.getSize().x - 9)//right wall
		ballSprite.setPosition(window.getSize().x - 9, ballSprite.getPosition().y);
	if (ballSprite.getPosition().y < 9)//ceiling
		ballSprite.setPosition(ballSprite.getPosition().x, 9);
	if (ballSprite.getPosition().y > window.getSize().y - 9)//floor
		ballSprite.setPosition(ballSprite.getPosition().x, window.getSize().y - 9);

	//gravitational acceleration
	if (ballSprite.getPosition().y < window.getSize().y - 12) // no gravitational pull on floor 
		ball.vy = ball.vy + 0.05*gravity;

	// stop ball completeley if moving moving very slow 
	if (abs(ball.vy) < 0.1*gcount)
		ball.vy = 0;
	if (abs(ball.vx) < 0.1)
		ball.vx = 0;
	
	//move ball
	ballSprite.move(ball.vx, ball.vy);

	
	
}

void reset(sf::Sprite &ballSprite, particle& ball, int multiplier, const sf::RenderWindow &window)
{
	//if more than 70 balls, limit this "resest circle" to the first 70. "reset2" function deals with the rest
	float multip = multiplier < 70 ? multiplier : 70;
	float ballsnum = ballsAmount < 70 ? ballsAmount : 70;

	float circumf = 20 * ballsnum;
	float radius = circumf / (2 * 3.1459);
	float xPos;
	float yPos;
	float angle = (2*3.145)/ballsnum;

	xPos = radius * sinf(angle*multip);
	yPos = radius * cosf(angle*multip);
	ball.vx = 1.0 * sinf(angle*multip);
	ball.vy = 1.0 * cosf(angle*multip);

	ballSprite.setPosition( (window.getSize().x/2) + xPos, (window.getSize().y / 2) + yPos);
	
}

void reset2(sf::Sprite &ballSprite, particle& ball, int multiplier, const sf::RenderWindow &window)
{
	//For resets when there are more than 70 balls
	//Makes smaller "reset circle" inside the larger circle of the first 70
	float multip = multiplier - 70;
	float ballsnum = ballsAmount - 70;

	float circumf = 20 * ballsnum;
	float radius = circumf / (2 * 3.1459);
	float xPos;
	float yPos;
	float angle = (2 * 3.145) / ballsnum;

	xPos = radius * sinf(angle*multip);
	yPos = radius * cosf(angle*multip);
	ball.vx = 1.0 * sinf(angle*multip);
	ball.vy = 1.0 * cosf(angle*multip);

	ballSprite.setPosition((window.getSize().x / 2) + xPos, (window.getSize().y / 2) + yPos);

}

void Efield(sf::Sprite &ballSprite1, sf::Sprite &ballSprite2, particle &ball1, particle &ball2, const sf::RenderWindow &window)
{
	float xDiff = ballSprite1.getPosition().x - ballSprite2.getPosition().x;
	float yDiff = ballSprite1.getPosition().y - ballSprite2.getPosition().y;
	float radius = 40;
	float massratio = (ball1.mass / ball2.mass);
	
	if (massratio < 1) // if ball2 heavier, reduce collision recoil on ball 2
	{
		if ((xDiff < 0) && (sqrtf(xDiff*xDiff + yDiff*yDiff) < radius))
		{
			ball1.vx -= 100 / (xDiff*xDiff + yDiff*yDiff);
			ball2.vx += 100 * massratio / (xDiff*xDiff + yDiff*yDiff);
		}
		if ((xDiff > 0) && (sqrtf(xDiff*xDiff + yDiff*yDiff) < radius))
		{
			ball1.vx += 100 / (xDiff*xDiff + yDiff*yDiff);
			ball2.vx -= 100 * massratio / (xDiff*xDiff + yDiff*yDiff);
		}
		if ((yDiff < 0) && (sqrtf(xDiff*xDiff + yDiff*yDiff) < radius))
		{
			ball1.vy -= 100 / (xDiff*xDiff + yDiff*yDiff);
			ball2.vy += 100 * massratio / (xDiff*xDiff + yDiff*yDiff);
		}
		if ((yDiff > 0) && (sqrtf(xDiff*xDiff + yDiff*yDiff) < radius))
		{
			ball1.vy += 100 / (xDiff*xDiff + yDiff*yDiff);
			ball2.vy -= 100 * massratio / (xDiff*xDiff + yDiff*yDiff);
		}
	}
	else
	{	//else reduce recoil speed of ball 1
		massratio = (ball2.mass / ball1.mass);
		if ((xDiff < 0) && (sqrtf(xDiff*xDiff + yDiff*yDiff) < radius))
		{
			ball1.vx -= 100 * (massratio) / (xDiff*xDiff + yDiff*yDiff);
			ball2.vx += 100  / (xDiff*xDiff + yDiff*yDiff);
		}
		if ((xDiff > 0) && (sqrtf(xDiff*xDiff + yDiff*yDiff) < radius))
		{
			ball1.vx += 100 * (massratio) / (xDiff*xDiff + yDiff*yDiff);
			ball2.vx -= 100 / (xDiff*xDiff + yDiff*yDiff);
		}
		if ((yDiff < 0) && (sqrtf(xDiff*xDiff + yDiff*yDiff) < radius))
		{
			ball1.vy -= 100 * (massratio) / (xDiff*xDiff + yDiff*yDiff);
			ball2.vy += 100 / (xDiff*xDiff + yDiff*yDiff);
		}
		if ((yDiff > 0) && (sqrtf(xDiff*xDiff + yDiff*yDiff) < radius))
		{
			ball1.vy += 100 * (massratio) / (xDiff*xDiff + yDiff*yDiff);
			ball2.vy -= 100 / (xDiff*xDiff + yDiff*yDiff);
		}
	}

	
}



//MAIN
int main()
{ 
	//create window
	sf::RenderWindow window(sf::VideoMode(800, 600), "Particles");
	window.setFramerateLimit(60);

	//create text
	sf::Font font;
	font.loadFromFile("kongtext.ttf");
	sf::Text texts[9];
	for (int i = 0;i < 9;++i)
	{
		texts[i].setFont(font);
		texts[i].setCharacterSize(10);
		texts[i].setPosition(5, 5 + i * 12);
	}
	//rename text boxes for ease of coding
	sf::Text &playermassText = texts[0];
	sf::Text &ballsamountText = texts[1];
	sf::Text &ballsmassText = texts[2];
	sf::Text &gravityText = texts[3];
	sf::Text &infotoggleText = texts[4];
	sf::Text &infoText = texts[5];
	sf::Text &antigravText = texts[6];
	sf::Text &EfieldText = texts[7];
	sf::Text &freezeText = texts[8];
	
	//permanent text box strings and positions
	infotoggleText.setString("\nPress I to toggle info");
	infoText.setString("\n\n\n.....Particles in a box simulator.....\n\nMild energy loss on collision with side walls or with other particles\nMild friction on floor surface\n\nAccelerate red player ball with arrow keys\n\nPress P to increase player ball mass (max 1000)\nPress M to increase dummy balls mass (max 1000)\nPress B to increase number of balls (max 100)\nPress G to set gravity\nPress F to toggle 'Free' player ball (anti-gravity)\nPress S to freeze dummy balls\nPress Q to toggle electric fields around balls\nPress R to reset balls to centre\n\nPress Esc to quit");
	antigravText.setPosition(5, window.getSize().y - 50);
	antigravText.setString("Anti-grav player: On");
	EfieldText.setPosition(5, window.getSize().y - 62);
	EfieldText.setString("Electric fields: On");
	freezeText.setPosition(5, window.getSize().y - 74);
	freezeText.setString("Freeze dummy balls: On");

	//create balls and initialise positions
	sf::Texture ballsTexture;
	ballsTexture.loadFromFile("images/bulletSprites.png");
	sf::Sprite ballSprites[101];
	for (int i = 0; i < 101; ++i)
	{
		if (i == 0)
		{
			ballSprites[i].setTexture(ballsTexture);
			ballSprites[i].setTextureRect(sf::IntRect(173, 112, 11, 11));
			ballSprites[i].setOrigin(5, 5);
			ballSprites[i].setPosition(400, 300);
			ballSprites[i].setScale(3, 3);
		}
		else
		{
			ballSprites[i].setTexture(ballsTexture);
			ballSprites[i].setTextureRect(sf::IntRect(203, 112, 11, 11));
			ballSprites[i].setOrigin(5, 5);
			ballSprites[i].setPosition( 9 + (0.9* window.getSize().x/ballsAmount)*i , window.getSize().y -20);
		}		
	}
	
	//define ball[0] (sprite) as player, for ease of coding
	sf::Sprite &playerSprite = ballSprites[0];
	particle &player = balls[0];

	//set mass of player ball
	player.mass = playermassMultiplier * 100;
	//initialise ball masses
	for (int i = 1; i < ballsAmount; ++i)
	{
		balls[i].mass = ballmassMultiplier * 100;
	}
	
	

	//MAIN RUNNING WINDOW
	while (window.isOpen())
	{

		//if mass or number of balls changed, reassign masses before new collisions happen
		if (mPressed == 1 || bPressed == 1 || pPressed == 1)
		{
			for (int i = 1; i < ballsAmount; ++i)
			{
				balls[i].mass = ballmassMultiplier * 100;
			}
			
			player.mass = playermassMultiplier * 100;
			
			mPressed = 0;
			bPressed = 0;
			pPressed = 0;
		}

		//limit max velocities (some particle collisions have chaotic results)
		for (int i = 0;i < ballsAmount;++i)
		{
			if (balls[i].vx > 10)
				balls[i].vx = 10;
			if (balls[i].vy > 10)
				balls[i].vy = 10;
		}
		
		

		//check collisions
		for (int i = 0; i < ballsAmount; ++i)
		{
			for (int j = 0; j < ballsAmount; ++j)
			{
				if (j <= i)
					continue;
				checkCollision(ballSprites[i], ballSprites[j], balls[i], balls[j]);
			}
		}

		//move balls (update ball positions)
		if (freezeToggle == 0)
		{			
			for (int i = 0; i < ballsAmount; ++i)
			{
				if (i == 0) //"0" index is player ball - has different movement if anti-grav turned on so keep separate
					movePlayerball(playerSprite, balls[i], window);
				else
					moveBalls(ballSprites[i], balls[i], window);
			}
		}
		else //freeze all other balls if freeze toggle = 1
		{
			for (int i = 1; i < ballsAmount; ++i)
			{
				balls[i].vx = 0;
				balls[i].vy = 0;
			}
			movePlayerball(playerSprite, balls[0], window);
		}

		

		// E field effects
		if (EfieldToggle == 1)
		{
			for (int i = 0; i < ballsAmount; ++i)
			{
				for (int j = 0; j < ballsAmount; ++j)
				{
					if (j <= i)
						continue;
					Efield(ballSprites[i], ballSprites[j], balls[i], balls[j], window);
				}
			}
		}

		//moving player
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
			player.vx -= 0.1;              
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) 
			player.vx += 0.1;
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
			player.vy -= 0.1 + 0.5*gcount*antigravplayer; //boost acceleration to counteract gravity
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
			player.vy += 0.1;

		
		//events, key presses, clicks, etc
		sf::Event event;
		while (window.pollEvent(event))
		{
			//close window
			if (event.type == sf::Event::Closed)
				window.close();
			//key presses
			if (event.type == sf::Event::KeyPressed)
			{
				//quit
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
					window.close();

				//reset balls to centre
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::R))
				{
					playerSprite.setPosition(window.getSize().x / 2, 20 );
					player.vx = 0;
					player.vy = 0;
					for (int i = 1; i < ballsAmount;++i)
					{
						if (i<70)
							reset(ballSprites[i], balls[i], i, window);
						else
							reset2(ballSprites[i], balls[i], i, window);
					}
				}

				//gravity toggle
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::G))
				{
					++gcount;
					if (gcount > 5)
						gcount = 0;
					if (gcount == 1)//reset gravity after the '0' setting 
						gravity = 9.81;
					gravity *= gcount;
				}

				//balls toggle
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
				{
					++ballsAmount;
					if (ballsAmount > 101)
						ballsAmount = 2;

					//resets balls to middle (avoids new balls being drawn over each other on first creation)
					playerSprite.setPosition(window.getSize().x / 2, 20);
					player.vx = 0;
					player.vy = 0;
					for (int i = 1; i < ballsAmount;++i)
					{
						if(i<70)
							reset(ballSprites[i], balls[i], i, window);
						else
							reset2(ballSprites[i], balls[i], i, window);
					}

					bPressed = 1;
				}

				//mass toggle
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
				{
					ballmassMultiplier += 1;
					if (ballmassMultiplier > 10)
						ballmassMultiplier = 1;
					
					mPressed = 1;
				}

				//player mass toggle
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
				{
					playermassMultiplier += 1;
					if (playermassMultiplier > 10)
						playermassMultiplier = 1;

					pPressed = 1;
				}

				// anti-grav player toggle
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::F))
				{
					if (antigravplayer == 0)
						antigravplayer = 1;
					else
						antigravplayer = 0;
				}

				// electric field toggle
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
				{
					if (EfieldToggle == 0)
						EfieldToggle = 1;
					else
						EfieldToggle = 0;
				}

				// freeze toggle
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
				{
					if (freezeToggle == 0)
						freezeToggle = 1;
					else
						freezeToggle = 0;
				}

				// info toggle
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
				{
					if (infoToggle == 0)
						infoToggle = 1;
					else
						infoToggle = 0;
				}

				

			}
				
		}

		//update text (in-game info)
		playermassString = "Player mass: " + std::to_string(playermassMultiplier * 100);
		playermassText.setString(playermassString);
		ballsnumberString = "Number of dummy balls: " + std::to_string(ballsAmount - 1);
		ballsamountText.setString(ballsnumberString);
		ballsmassString = "Dummy balls mass: " + std::to_string(ballmassMultiplier * 100);
		ballsmassText.setString(ballsmassString);
		gravString = "Gravity = " + std::to_string(gcount) + "g";
		gravityText.setString(gravString);


		//draw display, sprites etc...
		window.clear();
		window.draw(playerSprite);
		//draw balls
		for (int i = 1; i < ballsAmount; ++i)
		{
			window.draw(ballSprites[i]);
		}
		//draw updated text
		for (int i = 0; i < 5;++i)
		{
			window.draw(texts[i]);
		}
		//draw info text if toggled on
		if (infoToggle == 1)
			window.draw(infoText);

		//draw E-field text if toggled on
		if (EfieldToggle == 1)
			window.draw(EfieldText);

		//draw 'anti-grav' text if toggle on
		if (antigravplayer == 0)
			window.draw(antigravText);

		//draw frozen balls text
		if (freezeToggle == 1)
			window.draw(freezeText);

		window.display();
		
	}


	return 0;

}