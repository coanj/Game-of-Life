/**
 * Conway's Game of Life simulation
 * By: James Coan
 * ©2015
 * 
 * https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life
 * 
 * This software was an exploration in using SFML media libraries while
 * also being a pretty cool exploration of Conway's Game of Life cellular 
 * automation algorithm. I also did this to explore C++ in a bit more detail.
 * 
 * ToDo:
 * 		Refine game window scale adjustments.
 * 		Add wrap around to cells when they reach the edge of the window.
 * 		Clean up UI and instructions.
 * 
 */
 
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace sf;

/**
 * DEFAULTS
 */
int fsize = 20;								//  Font size
int menX = 8, menY = -3;					//	Button Text offset
int size = 10;								//	default cell size
int xwin = 640, ywin = 480;					//	default game window size
int speed = 50000000;						//	default speed of animation
#define WAIT for(int i=0; i<speed; i++);	//	Control animation speed

/**
 * @brief Life Object
 * @details This creates an instance of the Life game
 */

class Life{
	public:
		Life(bool yes);
		void run();
	private:
		void update();
		void render();
		void scale();
		void fill();
		void blob(int x, int y);
		Texture skin;
		Sprite cell;
		Image icon;
		RenderWindow window;
		std::vector< std::vector<int> > vect;
		std::vector< std::vector<int> > tmp;
		int wide = 1;
		int tall = 1;
		float adjust;
		bool yes;
};

/**
 * @brief Menu Object
 * @details This creates an instance of the Life menu
 */

class Menu{
	public:
		Menu();
		void run();
	private:
		void processEvents();
		void update();
		void render();
		void list();
		RenderWindow mWin;
		Texture large;
		Texture small;
		Texture medium;
		Sprite sizeImg;
		Sprite w640;
		Sprite w800;
		Sprite w1024;
		Sprite h480;
		Sprite h600;
		Sprite h768;
		Sprite scale;
		Sprite cell10;
		Sprite cell9;
		Sprite cell8;
		Sprite cell7;
		Sprite cell6;
		Sprite cell5;
		Sprite cell4;
		Sprite cell3;
		Sprite cell2;
		Sprite cell1;
		Sprite start1;
		Sprite start2;
		Font mFont;
		Text wh;
		Text x640;
		Text x800;
		Text x1024;
		Text y480;
		Text y600;
		Text y768;
		Text sc;
		Text s10;
		Text s9;
		Text s8;
		Text s7;
		Text s6;
		Text s5;
		Text s4;
		Text s3;
		Text s2;
		Text s1;
		Text st1;
		Text st2;
		Text rules;
		Image icon;
};

/**
 * @brief Set Environment
 * @details Creates window for the actual game based on default settings
 * or chosen settings from the menu.
 */

Life::Life(bool yes) : window(VideoMode(xwin, ywin), "Game of Life Simulation", Style::Close){
	scale();	
	if(!icon.loadFromFile("assets/cdat")){
		return;
	}
	window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	if(!skin.loadFromFile("assets/cdat")){
		return;
	}
	
	cell.setTexture(skin);
	cell.setScale(adjust, adjust);
	std::vector< std::vector<int> > a(wide, std::vector<int>(tall,0));
	vect = a;
	tmp = a;
	if(!yes)
		fill();
}
/**
 * @brief Run
 * @details Simulation loop where updating and displaying are called
 */
void Life::run(){
	bool pause = false;
	while(window.isOpen()){
		Event event;
		while(window.pollEvent(event)){
			if(event.type == Event::Closed){
				window.close();
			}
			if(event.type == Event::MouseWheelMoved){
				speed += (event.mouseWheel.delta * -10000000);
				if(speed <= 0)
					speed = 1000;
				else if(speed >= 999999999)
					speed = 999999999;
			}
			if(event.type == Event::MouseButtonPressed){
				if(event.mouseButton.button == Mouse::Left){
					int x = ( (event.mouseButton.x) - 10 ) / (adjust*30);
					int y = ( (event.mouseButton.y) - 10 ) / (adjust*24);
					blob(x,y);
				}
				if(event.mouseButton.button == Mouse::Middle){
					int x = ( (event.mouseButton.x) - 10 ) / (adjust*30);
					int y = ( (event.mouseButton.y) - 10 ) / (adjust*24);
					if(x < wide && y < tall )
						vect[x][y] = 0;
				}
				if(event.mouseButton.button == Mouse::Right){
					if(!pause)
						pause = true;
					else
						pause = false;
				}
			}
		}
		WAIT
		if(!pause)
			update();
		render();
	}
}
/**
 * @brief blob
 * @details Create a blob of random cells at location
 * @param x x-axis position
 * @param y y-axis position
 */
void Life::blob(int x, int y){
	if(x > 0 && y > 0 && x < wide-1 && y < tall ){
		vect[x][y] = 1;
		vect[x-1][y] = std::rand() % 2;
		vect[x+1][y] = std::rand() % 2;
		vect[x][y-1] = std::rand() % 2;
		vect[x][y+1] = std::rand() % 2;
		vect[x-1][y-1] = std::rand() % 2;
		vect[x+1][y+1] = std::rand() % 2;
		vect[x-1][y+1] = std::rand() % 2;
		vect[x+1][y-1] = std::rand() % 2;
	}
}
/**
 * @brief Fill
 * @details Fill environment with random occurrences of cells
 */
void Life::fill(){
	std::srand(std::time(NULL));
	for(int y = 0; y < wide; y++){
		for(int z = 0; z < tall; z++){
			int num = std::rand() % 2;
			if(num == 1){
				vect[y][z] = 1;
			}
			else{
				vect[y][z] = 0;
			}
		}
	}
}
/**
 * @brief update
 * @details update environment based on set of rules. Only one or the other should be used.
 * 			B - Birth, S - Survive
 * 			Conway:		B3/S23
 * 			HighLife:	B36/S23	<-- Used in this simulation for better patterns, described below
 * 			Build tmp environment based on current environment
 */
void Life::update(){
	for(int y = 0; y < wide; y++){
		for(int z = 0; z < tall; z++){
			/**
			 * Awareness of surrounding cells
			 */
			int count = 0;
			if(y > 0 && vect[y-1][z] == 1)
				count++;
			if(y > 0 && z < tall && vect[y-1][z+1] == 1)
				count++;
			if(z < tall && vect[y][z+1] == 1)
				count++;
			if(y < wide-1 && z < tall && vect[y+1][z+1] == 1)
				count++;
			if(y < wide-1 && vect[y+1][z] == 1)
				count++;
			if(y < wide-1 && z > 0 && vect[y+1][z-1] == 1)
				count++;
			if(z > 0 && vect[y][z-1] == 1)
				count++;
			if(y > 0 && z > 0 && vect[y-1][z-1] == 1)
				count++;
			/**
			 * @brief Highlife Rules
			 * @details B36/S23	-- Birth when 3 or 6 adjacent, survive when 2-3 adjacent, else death
			 */
			if(vect[y][z] == 1 && (count < 2 || count > 3) )			//	Death condition
				tmp[y][z] = 0;
			else if(vect[y][z] == 1 && (count >= 2 || count <= 3))		//	Survival condition
				tmp[y][z] = 1;
			else if(vect[y][z] == 0 && (count == 3 || count == 6))		//	Birth condition
				tmp[y][z] = 1;
		}
	}
	vect = tmp;	//	Make current environment equal to new environment
}
/**
 * @brief Render
 * @details Display current environment
 */
void Life::render(){
	window.clear();
	for(int x = 0; x < wide; x++){
		for(int y = 0; y < tall; y++){
			if(vect[x][y] == 1){
				cell.setPosition( (x*(adjust*30)+10) , (y*(adjust*24)+10) ) ;
			}
			window.draw(cell);
		}
	}
	window.display();
}
/**
 * @brief Scale
 * @details Scale cell size and number based on window size
 */
void Life::scale(){
	switch(size){
		case 9:
			wide = (xwin*.0369);
			tall = (ywin*.0438);
			adjust = .9;
			break;
		case 8:
			wide = (xwin*.0407);
			tall = (ywin*.0500);
			adjust = .8;
			break;
		case 7:
			wide = (xwin*.0468);
			tall = (ywin*.0583);
			adjust = .7;
			break;
		case 6:
			wide = (xwin*.0546);
			tall = (ywin*.0687);
			adjust = .6;
			break;
		case 5:
			wide = (xwin*.0656);
			tall = (ywin*.0812);
			adjust = .5;
			break;
		case 4:
			wide = (xwin*.0813);
			tall = (ywin*.1020);
			adjust = .4;
			break;
		case 3:
			wide = (xwin*.1093);
			tall = (ywin*.1354);
			adjust = .3;
			break;
		case 2:
			wide = (xwin*.1640);
			tall = (ywin*.2041);
			adjust = .2;
			break;
		case 1:
			wide = (xwin*.3265);
			tall = (ywin*.4062);
			adjust = .1;
			break;
		default:
			wide = (xwin*.0325);
			tall = (ywin*.0400);
			adjust = 1;
			break;
	}
}
/***********************************End of LIFE CLASS************************************/


/***************************************MENU CLASS***************************************/

Menu::Menu() : mWin(VideoMode(550, 400), "LIFE MENU", Style::Close){
	if(!icon.loadFromFile("assets/cdat")){
		return;
	}
	if(!large.loadFromFile("assets/bdat")){
		return;
	}
	if(!small.loadFromFile("assets/sbdat")){
		return;
	}
	if(!medium.loadFromFile("assets/mbdat")){
		return;
	}
	if(!mFont.loadFromFile("assets/fdat")){
		return;
	}
	mWin.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
	wh.setFont(mFont);
	wh.setStyle(Text::Bold);
	wh.setString(("\t    -WINDOW-\n WIDE      |      TALL"));
	wh.setCharacterSize(fsize-2);
	wh.setColor(Color::Black);
	wh.setPosition(40,10);
	x640.setFont(mFont);
	x640.setStyle(Text::Bold);
	x640.setString("640");
	x640.setCharacterSize(fsize);
	x640.setColor(Color::Black);
	x640.setPosition(43+menX,61+menY);
	x800.setFont(mFont);
	x800.setStyle(Text::Bold);
	x800.setString("800");
	x800.setCharacterSize(fsize);
	x800.setColor(Color::Black);
	x800.setPosition(43+menX,91+menY);
	x1024.setFont(mFont);
	x1024.setStyle(Text::Bold);
	x1024.setString("1024");
	x1024.setCharacterSize(fsize);
	x1024.setColor(Color::Black);
	x1024.setPosition(37+menX,121+menY);
	y480.setFont(mFont);
	y480.setStyle(Text::Bold);
	y480.setString("480");
	y480.setCharacterSize(fsize);
	y480.setColor(Color::Black);
	y480.setPosition(150+menX,61+menY);
	y600.setFont(mFont);
	y600.setStyle(Text::Bold);
	y600.setString("600");
	y600.setCharacterSize(fsize);
	y600.setColor(Color::Black);
	y600.setPosition(150+menX,91+menY);
	y768.setFont(mFont);
	y768.setStyle(Text::Bold);
	y768.setString("768");
	y768.setCharacterSize(fsize);
	y768.setColor(Color::Black);
	y768.setPosition(150+menX,121+menY);
	sc.setFont(mFont);
	sc.setStyle(Text::Bold);
	sc.setString("CELLULAR SCALE");
	sc.setCharacterSize(fsize+2);
	sc.setColor(Color::Black);
	sc.setPosition(36,158);
	s10.setFont(mFont);
	s10.setStyle(Text::Bold);
	s10.setString("10");
	s10.setCharacterSize(fsize);
	s10.setColor(Color::Black);
	s10.setPosition(50+menX,201+menY);
	s9.setFont(mFont);
	s9.setStyle(Text::Bold);
	s9.setString("9");
	s9.setCharacterSize(fsize);
	s9.setColor(Color::Black);
	s9.setPosition(55+menX,231+menY);
	s8.setFont(mFont);
	s8.setStyle(Text::Bold);
	s8.setString("8");
	s8.setCharacterSize(fsize);
	s8.setColor(Color::Black);
	s8.setPosition(55+menX,261+menY);
	s7.setFont(mFont);
	s7.setStyle(Text::Bold);
	s7.setString("7");
	s7.setCharacterSize(fsize);
	s7.setColor(Color::Black);
	s7.setPosition(55+menX,291+menY);
	s6.setFont(mFont);
	s6.setStyle(Text::Bold);
	s6.setString("6");
	s6.setCharacterSize(fsize);
	s6.setColor(Color::Black);
	s6.setPosition(55+menX,321+menY);
	s5.setFont(mFont);
	s5.setStyle(Text::Bold);
	s5.setString("5");
	s5.setCharacterSize(fsize);
	s5.setColor(Color::Black);
	s5.setPosition(162+menX,201+menY);
	s4.setFont(mFont);
	s4.setStyle(Text::Bold);
	s4.setString("4");
	s4.setCharacterSize(fsize);
	s4.setColor(Color::Black);
	s4.setPosition(161+menX,231+menY);
	s3.setFont(mFont);
	s3.setStyle(Text::Bold);
	s3.setString("3");
	s3.setCharacterSize(fsize);
	s3.setColor(Color::Black);
	s3.setPosition(162+menX,261+menY);
	s2.setFont(mFont);
	s2.setStyle(Text::Bold);
	s2.setString("2");
	s2.setCharacterSize(fsize);
	s2.setColor(Color::Black);
	s2.setPosition(162+menX,291+menY);
	s1.setFont(mFont);
	s1.setStyle(Text::Bold);
	s1.setString("1");
	s1.setCharacterSize(fsize);
	s1.setColor(Color::Black);
	s1.setPosition(162+menX,321+menY);
	st1.setFont(mFont);
	st1.setStyle(Text::Bold);
	st1.setString("START\nBLANK");
	st1.setCharacterSize(fsize-6);
	st1.setColor( Color(21, 73, 3, 255) );
	st1.setPosition(45,356);
	st2.setFont(mFont);
	st2.setStyle(Text::Bold);
	st2.setString("  START\nRANDOM");
	st2.setCharacterSize(fsize-6);
	st2.setColor( Color(21, 73, 3, 255) );
	st2.setPosition(142,356);
	sizeImg.setTexture(large);
	sizeImg.setPosition(10,10);
	w640.setTexture(small);
	w800.setTexture(small);
	w1024.setTexture(small);
	h480.setTexture(small);
	h600.setTexture(small);
	h768.setTexture(small);
	w640.setPosition(35,60);
	w800.setPosition(35,90);
	w1024.setPosition(35,120);
	h480.setPosition(140,60);
	h600.setPosition(140,90);
	h768.setPosition(140,120);
	scale.setTexture(large);
	scale.setPosition(10,150);
	cell10.setTexture(small);
	cell9.setTexture(small);
	cell8.setTexture(small);
	cell7.setTexture(small);
	cell6.setTexture(small);
	cell5.setTexture(small);
	cell4.setTexture(small);
	cell3.setTexture(small);
	cell2.setTexture(small);
	cell1.setTexture(small);
	cell10.setPosition(35,200);
	cell9.setPosition(35,230);
	cell8.setPosition(35,260);
	cell7.setPosition(35,290);
	cell6.setPosition(35,320);
	cell5.setPosition(140,200);
	cell4.setPosition(140,230);
	cell3.setPosition(140,260);
	cell2.setPosition(140,290);
	cell1.setPosition(140,320);
	start1.setTexture(medium);
	start1.setPosition(25,355);
	start2.setTexture(medium);
	start2.setPosition(130,355);
	list();
}
/**
 * @brief run
 * @details run function to start the menu, process the events, and render output
 */
void Menu::run(){
	while(mWin.isOpen()){
		processEvents();
		render();
	}
}
/**
 * @brief list
 * @details function for initializing rules text
 */
void Menu::list(){
	rules.setFont(mFont);
	rules.setCharacterSize(fsize-6);
	rules.setColor(Color::Green);
	rules.setPosition(245,10);
	rules.scale(1,.97);
	std::string ruleText= 	"                       <GAME OF LIFE RULES>\n"
							"************************************************\n"
							"1-  Any Cell with fewer than two live neighbors,\n"
							"\t  dies due to underpopulation.\n"
							"2-  Any Cell with two or three neighbors lives.\n"
							"3-  Any cell with more than three neighbors\n"
							"\t  dies, due to overcrowding.\n"
							"4-  Dead Cells with three or six neighbors\n"
							"\t  becomes living due to reproduction.\n"
							"************************************************\n"
							"                             <INSTRUCTIONS>\n"
							"************************************************\n"
							"1-  Pick a width and height for the size of the\n"
							"\t  window\n"
							"2-  Choose a Cell size\n"
							"3-  Choose to start with a blank screen or\n"
							"\t  randomly filled\n"
							"************************************************\n"
							"                                   <CONTROLS>\n"
							"************************************************\n"
							"PAUSE  = \tMouse right click\n"
							"BLOB     = \tMouse Left click\n"
							"DELETE = \tMouse left click on one Cell\n"
							"************************************************";
	
	rules.setString(ruleText);
}
/**
 * @brief processEvents
 * @details function that controls all events in menu. Closing the window, as well
 *          as enabling the ability to click on buttons. Initializes and calls Life
 */
void Menu::processEvents(){
	Event event;
	while(mWin.pollEvent(event)){
		if(event.type == Event::Closed)
			mWin.close();
		if (event.type == Event::MouseButtonPressed){
			if (event.mouseButton.button == Mouse::Left){
				if (w640.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					xwin = 640;
					x640.setColor(Color::Red);
					x800.setColor(Color::Black);
					x1024.setColor(Color::Black);
				}
				if (w800.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					xwin = 800;
					x640.setColor(Color::Black);
					x800.setColor(Color::Red);
					x1024.setColor(Color::Black);
				}
				if (w1024.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					xwin = 1024;
					x640.setColor(Color::Black);
					x800.setColor(Color::Black);
					x1024.setColor(Color::Red);
				}
				if (h480.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					ywin = 480;
					y480.setColor(Color::Red);
					y600.setColor(Color::Black);
					y768.setColor(Color::Black);
				}
				if (h600.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					ywin = 600;
					y480.setColor(Color::Black);
					y600.setColor(Color::Red);
					y768.setColor(Color::Black);
				}
				if (h768.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					ywin = 768;
					y480.setColor(Color::Black);
					y600.setColor(Color::Black);
					y768.setColor(Color::Red);
				}
				if (cell10.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					size = 10;
					s10.setColor(Color::Red);	s9.setColor(Color::Black);	s8.setColor(Color::Black);	s7.setColor(Color::Black);	s6.setColor(Color::Black);
					s5.setColor(Color::Black);	s4.setColor(Color::Black); 	s3.setColor(Color::Black); 	s2.setColor(Color::Black);	s1.setColor(Color::Black);
				}
				if (cell9.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					size = 9;
					s10.setColor(Color::Black);	s9.setColor(Color::Red);	s8.setColor(Color::Black);	s7.setColor(Color::Black);	s6.setColor(Color::Black);
					s5.setColor(Color::Black);	s4.setColor(Color::Black); 	s3.setColor(Color::Black); 	s2.setColor(Color::Black);	s1.setColor(Color::Black);
				}
				if (cell8.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					size = 8;
					s10.setColor(Color::Black);	s9.setColor(Color::Black);	s8.setColor(Color::Red);	s7.setColor(Color::Black);	s6.setColor(Color::Black);
					s5.setColor(Color::Black);	s4.setColor(Color::Black); 	s3.setColor(Color::Black); 	s2.setColor(Color::Black);	s1.setColor(Color::Black);
				}
				if (cell7.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					size = 7;
					s10.setColor(Color::Black);	s9.setColor(Color::Black);	s8.setColor(Color::Black);	s7.setColor(Color::Red);	s6.setColor(Color::Black);
					s5.setColor(Color::Black);	s4.setColor(Color::Black); 	s3.setColor(Color::Black); 	s2.setColor(Color::Black);	s1.setColor(Color::Black);
				}
				if (cell6.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					size = 6;
					s10.setColor(Color::Black);	s9.setColor(Color::Black);	s8.setColor(Color::Black);	s7.setColor(Color::Black);	s6.setColor(Color::Red);
					s5.setColor(Color::Black);	s4.setColor(Color::Black); 	s3.setColor(Color::Black); 	s2.setColor(Color::Black);	s1.setColor(Color::Black);
				}
				if (cell5.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					size = 5;
					s10.setColor(Color::Black);	s9.setColor(Color::Black);	s8.setColor(Color::Black);	s7.setColor(Color::Black);	s6.setColor(Color::Black);
					s5.setColor(Color::Red);	s4.setColor(Color::Black); 	s3.setColor(Color::Black); 	s2.setColor(Color::Black);	s1.setColor(Color::Black);
				}
				if (cell4.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					size = 4;
					s10.setColor(Color::Black);	s9.setColor(Color::Black);	s8.setColor(Color::Black);	s7.setColor(Color::Black);	s6.setColor(Color::Black);
					s5.setColor(Color::Black);	s4.setColor(Color::Red); 	s3.setColor(Color::Black); 	s2.setColor(Color::Black);	s1.setColor(Color::Black);
				}
				if (cell3.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					size = 3;
					s10.setColor(Color::Black);	s9.setColor(Color::Black);	s8.setColor(Color::Black);	s7.setColor(Color::Black);	s6.setColor(Color::Black);
					s5.setColor(Color::Black);	s4.setColor(Color::Black); 	s3.setColor(Color::Red); 	s2.setColor(Color::Black);	s1.setColor(Color::Black);
				}
				if (cell2.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					size = 2;
					s10.setColor(Color::Black);	s9.setColor(Color::Black);	s8.setColor(Color::Black);	s7.setColor(Color::Black);	s6.setColor(Color::Black);
					s5.setColor(Color::Black);	s4.setColor(Color::Black); 	s3.setColor(Color::Black); 	s2.setColor(Color::Red);	s1.setColor(Color::Black);
				}
				if (cell1.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					size = 1;
					s10.setColor(Color::Black);	s9.setColor(Color::Black);	s8.setColor(Color::Black);	s7.setColor(Color::Black);	s6.setColor(Color::Black);
					s5.setColor(Color::Black);	s4.setColor(Color::Black); 	s3.setColor(Color::Black); 	s2.setColor(Color::Black);	s1.setColor(Color::Red);
				}
				if (start1.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					Life life(true);
					life.run();
				}
				if (start2.getGlobalBounds().contains(mWin.mapPixelToCoords(Mouse::getPosition(mWin)))){
					Life life(false);
					life.run();
				}
			}
		}
		
	}
}
/**
 * @brief render
 * @details function for clearing, loading and displaying everything in the window
 */
void Menu::render(){
	mWin.clear();
	mWin.draw(sizeImg);
	mWin.draw(w640);
	mWin.draw(w800);
	mWin.draw(w1024);
	mWin.draw(h480);
	mWin.draw(h600);
	mWin.draw(h768);
	mWin.draw(scale);
	mWin.draw(cell10);
	mWin.draw(cell9);
	mWin.draw(cell8);
	mWin.draw(cell7);
	mWin.draw(cell6);
	mWin.draw(cell5);
	mWin.draw(cell4);
	mWin.draw(cell3);
	mWin.draw(cell2);
	mWin.draw(cell1);	
	mWin.draw(start1);
	mWin.draw(start2);
	mWin.draw(wh);
	mWin.draw(x640);
	mWin.draw(x800);
	mWin.draw(x1024);
	mWin.draw(y480);
	mWin.draw(y600);
	mWin.draw(y768);
	mWin.draw(sc);
	mWin.draw(s10);
	mWin.draw(s9);
	mWin.draw(s8);
	mWin.draw(s7);
	mWin.draw(s6);
	mWin.draw(s5);
	mWin.draw(s4);
	mWin.draw(s3);
	mWin.draw(s2);
	mWin.draw(s1);
	mWin.draw(st1);
	mWin.draw(st2);
	mWin.draw(rules);
	mWin.display();
}
/***********************************End of MENU CLASS************************************/

int main(){
	Menu menu;
	menu.run();
	return EXIT_SUCCESS;
}