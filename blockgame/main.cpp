#include <SFML/Graphics.hpp>
#include<stdlib.h>
#include<string>
#include <algorithm>
#include <random>
#include"piece.h"
#include<iomanip>
#include<sstream>
#include <iostream>
using namespace std;

class pieceBag {
private:
	piece bagHolding[8];
public:
	void fillbag(); //put one of each piece in bag and shuffle it
	piece lookAtPlace(int); //look at piece in location int
	piece pushInAndOut(piece); //combination of push and pop
	bool isEmpty(); //check if bag is empty
};

void pieceBag::fillbag() {
	bagHolding[0] = pieceDot;
	bagHolding[1] = pieceI;
	bagHolding[2] = pieceLine;
	bagHolding[3] = pieceO;
	bagHolding[4] = pieceR;
	bagHolding[5] = piece7;
	bagHolding[6] = pieceL;
	bagHolding[7] = pieceJ;

	// Do Fisher-Yates shuffle to randomize bag easily
	for (int i = 0; i < 8 - 1; i++) {
		int j = i + rand() % (8 - i);
		swap(bagHolding[i], bagHolding[j]);
	}
}

piece pieceBag::lookAtPlace(int i) {
	return bagHolding[i];
}

piece pieceBag::pushInAndOut(piece pushMe) {
	piece popMe = bagHolding[0];
	for (int i = 0; i < 7; i++) {
		swap(bagHolding[i], bagHolding[i + 1]);
		bagHolding[7] = pushMe;
	}
	return popMe;
}

bool pieceBag::isEmpty() {
	if (bagHolding[0] == none) {
		return true;
	}
	else return false;
}

class pieceQueue {
private:
	pieceBag seen, hidden;
public:
	pieceQueue();
	piece showNext(int); //look at the piece in location int
	piece getNext(); //get the next piece in the queue and make sure the queue is good
};

pieceQueue::pieceQueue() {
	seen.fillbag();
	hidden.fillbag();
}

piece pieceQueue::showNext(int i) {
	return seen.lookAtPlace(i);
}

piece pieceQueue::getNext() {
	piece meNext = seen.pushInAndOut(hidden.pushInAndOut(none));
	if (hidden.isEmpty()) {
		hidden.fillbag();
	}
	return meNext;
}

class gameBoard {
private:
	pieceQueue pieceQ;
	int piecesDropped;
	int linesCleared;
	int score;
	bool board[8][8]; //[X][Y] where 0-7 in X represents a distance from left side and 0-7 in Y represents a height-distance
public:
	gameBoard();
	bool dropPiece(int, float);
	int checkLineClear(int); //check horiz row at V height 'int', clears row and pulls down upper rows then recurses. returns number of lines cleared.
	int getHeight(int); //find heighest occupied square in V column 'int', by index for that square
	int getPiecesDropped();
	int getLinesCleared();
	int getLevelValue(); // Level value determines what 'level' you're in and how much time you have to drop
	piece lookPieceQ(int); //check what's the piece 'int' pieces after the current piece
	bool getBoardState(int, int); //check if the square at int x, int y is full or not, used for drawing the gameboard
	sf::Time getTimeLimit();
	string getNextLevelValue();
	int getScore();
};

gameBoard::gameBoard() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			board[i][j] = false;
		}
	}
	piecesDropped = 0;
	linesCleared = 0;
	score = 0;
}

int gameBoard::getHeight(int column) {
	int maxFound = -1;
	for (int i = 7; i > -1; i--) {
		if (maxFound == -1 && board[column][i]) {
			maxFound = i;
		}
	}
	return maxFound;
}

int gameBoard::getPiecesDropped() {
	return piecesDropped;
}

int gameBoard::getLinesCleared() {
	return linesCleared;
}

int gameBoard::getLevelValue() {
	return (piecesDropped + linesCleared);
}

piece gameBoard::lookPieceQ(int i) {
	return pieceQ.showNext(i);
}

bool gameBoard::getBoardState(int x, int y) {
	return board[x][y];
}

int gameBoard::checkLineClear(int row) {
	bool fullLine = true;
	for (int i = 0; i < 8; i++) {
		if (!board[i][row]) {
			fullLine = false;
		}
	}
	if (fullLine) {
		for (int i = 0; i < 8; i++) {
			board[i][row] = false;
		}
		int tempRow = row;
		while (tempRow < 7) {
			for (int i = 0; i < 8; i++) {
				board[i][tempRow] = board[i][tempRow + 1];
			}
			tempRow++;
		}
		for (int i = 0; i < 8; i++) board[i][7] = false;
		return 1 + checkLineClear(row);
	}
	return 0;
}

bool gameBoard::dropPiece(int dropZone, float scoreModifier) {
	bool dropSuccessful = true;
	piece dropMe = pieceQ.getNext();
	int dropZoneRightHeight = -1;
	if (!(dropMe == pieceDot) && !(dropMe == pieceI) && dropZone == 7) {
		dropZone--;
	}
	int dropZoneLeftHeight = getHeight(dropZone);
	if (dropZone != 7) dropZoneRightHeight = getHeight(dropZone + 1);
	int dropZoneDiff = dropZoneLeftHeight - dropZoneRightHeight;
	switch (dropMe) {
	case pieceDot:
		if (dropZoneLeftHeight == 7) dropSuccessful = false;
		else {
			board[dropZone][dropZoneLeftHeight + 1] = true;
		}
		break;
	case pieceI:
		if (dropZoneLeftHeight == 7) dropSuccessful = false;
		else {
			board[dropZone][dropZoneLeftHeight + 1] = true;
			if (dropZoneLeftHeight + 2 <= 7) board[dropZone][dropZoneLeftHeight + 2] = true;
		}
		break;
	case pieceLine:
		if (dropZoneDiff >= 0) { //drop zone is flat or left side is higher
			if (dropZoneLeftHeight == 7) dropSuccessful = false;
			else {
				board[dropZone][dropZoneLeftHeight + 1] = true;
				board[dropZone + 1][dropZoneLeftHeight + 1] = true;
			}
		}
		else { //drop zone right side is higher
			if (dropZoneRightHeight == 7) dropSuccessful = false;
			else {
				board[dropZone][dropZoneRightHeight + 1] = true;
				board[dropZone + 1][dropZoneRightHeight + 1] = true;
			}
		}
		break;
	case pieceO:
		if (dropZoneDiff >= 0) { //drop zone is flat or left side is higher
			if (dropZoneLeftHeight == 7) dropSuccessful = false;
			else {
				board[dropZone][dropZoneLeftHeight + 1] = true;
				board[dropZone + 1][dropZoneLeftHeight + 1] = true;
				if (dropZoneLeftHeight + 2 <= 7) {
					board[dropZone][dropZoneLeftHeight + 2] = true;
					board[dropZone + 1][dropZoneLeftHeight + 2] = true;
				}
			}
		}
		else { //drop zone right side is higher
			if (dropZoneRightHeight == 7) dropSuccessful = false;
			else {
				board[dropZone][dropZoneRightHeight + 1] = true;
				board[dropZone + 1][dropZoneRightHeight + 1] = true;
				if (dropZoneRightHeight + 2 <= 7) {
					board[dropZone][dropZoneRightHeight + 2] = true;
					board[dropZone + 1][dropZoneRightHeight + 2] = true;
				}
			}
		}
		break;
	case pieceL:
		if (dropZoneDiff >= 0) { //drop zone is flat or left side is higher
			if (dropZoneLeftHeight == 7) dropSuccessful = false;
			else {
				board[dropZone][dropZoneLeftHeight + 1] = true;
				board[dropZone + 1][dropZoneLeftHeight + 1] = true;
				if (dropZoneLeftHeight + 2 <= 7) {
					board[dropZone][dropZoneLeftHeight + 2] = true;
				}
			}
		}
		else { //drop zone right side is higher
			if (dropZoneRightHeight == 7) dropSuccessful = false;
			else {
				board[dropZone][dropZoneRightHeight + 1] = true;
				board[dropZone + 1][dropZoneRightHeight + 1] = true;
				if (dropZoneRightHeight + 2 <= 7) {
					board[dropZone][dropZoneRightHeight + 2] = true;
				}
			}
		}
		break;
	case pieceJ:
		if (dropZoneDiff >= 0) { //drop zone is flat or left side is higher
			if (dropZoneLeftHeight == 7) dropSuccessful = false;
			else {
				board[dropZone][dropZoneLeftHeight + 1] = true;
				board[dropZone + 1][dropZoneLeftHeight + 1] = true;
				if (dropZoneLeftHeight + 2 <= 7) {
					board[dropZone + 1][dropZoneLeftHeight + 2] = true;
				}
			}
		}
		else { //drop zone right side is higher
			if (dropZoneRightHeight == 7) dropSuccessful = false;
			else {
				board[dropZone][dropZoneRightHeight + 1] = true;
				board[dropZone + 1][dropZoneRightHeight + 1] = true;
				if (dropZoneRightHeight + 2 <= 7) {
					board[dropZone + 1][dropZoneRightHeight + 2] = true;
				}
			}
		}
		break;
	case pieceR:
		if (dropZoneDiff >= 0) { //drop zone is flat or left side is higher
			if (dropZoneLeftHeight == 7) dropSuccessful = false;
			else {
				board[dropZone][dropZoneLeftHeight + 1] = true;
				if (dropZoneLeftHeight + 2 <= 7) {
					board[dropZone][dropZoneLeftHeight + 2] = true;
					board[dropZone + 1][dropZoneLeftHeight + 2] = true;
				}
			}
		}
		else { //drop zone right side is higher by at least 1
			if (dropZoneRightHeight == 7) dropSuccessful = false;
			else {
				board[dropZone][dropZoneRightHeight + 1] = true;
				board[dropZone + 1][dropZoneRightHeight + 1] = true;
				board[dropZone][dropZoneRightHeight] = true;
			}
		}
		break;
	case piece7:
		if (dropZoneDiff <= 0) { //drop zone is flat or right side is higher
			if (dropZoneRightHeight == 7) dropSuccessful = false;
			else {
				board[dropZone + 1][dropZoneRightHeight + 1] = true;
				if (dropZoneRightHeight + 2 <= 7) {
					board[dropZone][dropZoneRightHeight + 2] = true;
					board[dropZone + 1][dropZoneRightHeight + 2] = true;
				}
			}
		}
		else { //drop zone left side is higher by at least 1
			if (dropZoneLeftHeight == 7) dropSuccessful = false;
			else {
				board[dropZone][dropZoneLeftHeight + 1] = true;
				board[dropZone + 1][dropZoneLeftHeight + 1] = true;
				board[dropZone + 1][dropZoneLeftHeight] = true;
			}
		}
		break;
	}
	//check line clear & clear lines, update dropped pieces
	int lowerHeight = dropZoneLeftHeight;
	if (dropZoneRightHeight < lowerHeight) lowerHeight = dropZoneRightHeight;
	int prevLines = linesCleared;
	for (int i = lowerHeight; i < 8; i++) {
		linesCleared += checkLineClear(i);
	}
	piecesDropped++;
	score += (100 / getTimeLimit().asSeconds()) * scoreModifier; // score for dropping a piece
	int lineDelta = linesCleared - prevLines;
	if (lineDelta == 1) {
		score += (1000 / getTimeLimit().asSeconds());
	}
	else if (lineDelta == 2) {
		score += (4000 / getTimeLimit().asSeconds());
	}
	return dropSuccessful;
}

sf::Time gameBoard::getTimeLimit() {
	int levelValue = getLevelValue();
	if (levelValue < 50) return sf::seconds(10);
	else if (levelValue < 100) return sf::seconds(9);
	else if (levelValue < 200) return sf::seconds(8);
	else if (levelValue < 300) return sf::seconds(7);
	else if (levelValue < 400) return sf::seconds(6);
	else if (levelValue < 500) return sf::seconds(5);
	else if (levelValue < 999) return sf::seconds(3);
	else return sf::seconds(1);
}

string gameBoard::getNextLevelValue() {
	int levelValue = getLevelValue();
	if (levelValue < 50) return "50";
	else if (levelValue < 100) return "100";
	else if (levelValue < 200) return "200";
	else if (levelValue < 300) return "300";
	else if (levelValue < 400) return "400";
	else if (levelValue < 500) return "500";
	else if (levelValue < 999) return "999";
	else return "MAX";
}

int gameBoard::getScore() {
	return score;
}

string checkPiece(piece checkMe) {
	switch (checkMe) {
	case pieceDot: return ".";
	case pieceI: return "I";
	case pieceLine: return "_";
	case pieceO: return "O";
	case pieceR: return "r";
	case piece7: return "7";
	case pieceL: return "L";
	case pieceJ: return "J";
	default: return "";
	}
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(800, 600), "BlockGame Prototype", sf::Style::Close);
	window.setKeyRepeatEnabled(false);

	sf::Font font;
	if (!font.loadFromFile("resources/sansation.ttf")) {
		window.close();
	}

	// Stats
	sf::Text scoreText;
	scoreText.setFont(font);
	scoreText.setCharacterSize(50);
	scoreText.setPosition(50, 50);
	scoreText.setString("0");
	scoreText.setFillColor(sf::Color::Black);

	sf::Text levelValueText;
	levelValueText.setFont(font);
	levelValueText.setCharacterSize(70);
	levelValueText.setPosition(650, 400);
	levelValueText.setString("1");
	levelValueText.setFillColor(sf::Color::Black);

	sf::Text nextLevelValueText;
	nextLevelValueText.setFont(font);
	nextLevelValueText.setCharacterSize(70);
	nextLevelValueText.setPosition(650, 480);
	nextLevelValueText.setString("666");
	nextLevelValueText.setFillColor(sf::Color::Black);

	// Next queue
	sf::Text nextPiece1;
	nextPiece1.setFont(font);
	nextPiece1.setCharacterSize(100);
	nextPiece1.setPosition(720, 40);
	nextPiece1.setString("L");
	nextPiece1.setFillColor(sf::Color::Black);

	sf::Text nextPiece2;
	nextPiece2.setFont(font);
	nextPiece2.setCharacterSize(60);
	nextPiece2.setPosition(730, 170);
	nextPiece2.setString("R");
	nextPiece2.setFillColor(sf::Color::Black);

	sf::Text nextPiece3;
	nextPiece3.setFont(font);
	nextPiece3.setCharacterSize(60);
	nextPiece3.setPosition(740, 250);
	nextPiece3.setString("J");
	nextPiece3.setFillColor(sf::Color::Black);

	// Timers
	sf::Text timeElapsedText;
	timeElapsedText.setFont(font);
	timeElapsedText.setCharacterSize(60);
	timeElapsedText.setPosition(20, 530);
	timeElapsedText.setString("temp");
	timeElapsedText.setFillColor(sf::Color::Black);

	// Strikes
	int strikes = 0;
	sf::Text strikesText;
	strikesText.setFont(font);
	strikesText.setCharacterSize(60);
	strikesText.setPosition(20, 430);
	strikesText.setString("");
	strikesText.setFillColor(sf::Color::Red);

	sf::Time timeElapsed = sf::seconds(0); // total time elapsed in the game
	sf::Time timeLeft = sf::seconds(10); // time left before player gets a strike
	sf::Time animationTimer = sf::seconds(0); // used to advance animation states, reset it and animate every .1s
	sf::Clock clock;

	sf::Texture backGround;
	sf::Texture block;
	sf::Texture youLose;
	if (!backGround.loadFromFile("resources/gameboard.png") || !block.loadFromFile("resources/block.png") || !youLose.loadFromFile("resources/losescreen.png")) {
		window.close();
	}

	sf::Sprite bgSprite;
	bgSprite.setTexture(backGround);
	bgSprite.setPosition(0,0);

	sf::Sprite loseScreen;
	loseScreen.setTexture(youLose);
	bgSprite.setPosition(0, 0);

	static const int startBoardX = 200;
	static const int boardXStep = 50;
	static const int startBoardY = 450;
	static const int boardYStep = -50;
	sf::Sprite boardBlock[8][8];
	
	for (int x = 0; x < 8; x++) {
		for (int y = 0; y < 8; y++) {
			boardBlock[x][y].setTexture(block);
			boardBlock[x][y].setPosition(startBoardX + (x * boardXStep), startBoardY + (y * boardYStep));
		}
	}

	static const int timerBarXPos = 176;
	static const int timerBarYPos = 100;
	sf::RectangleShape timerBar(sf::Vector2f(20.f, 400.f));
	timerBar.setFillColor(sf::Color(0,0,0));
	timerBar.setPosition(timerBarXPos, timerBarYPos);

	int lastScore = 0;
	bool isPlaying = true;
	gameBoard game;
	
	while (window.isOpen())
	{
		if (isPlaying){
			sf::Event event;
			while (window.pollEvent(event))
			{
				switch (event.type) {
					case sf::Event::Closed:
						window.close();
						break;
					case sf::Event::KeyPressed:

						switch (event.key.code) {
							case sf::Keyboard::Z:
								isPlaying = game.dropPiece(0, (timeLeft.asSeconds() / game.getTimeLimit().asSeconds()));
								timeLeft = game.getTimeLimit();
								break;
							case sf::Keyboard::S:
								isPlaying = game.dropPiece(1, (timeLeft.asSeconds() / game.getTimeLimit().asSeconds()));
								timeLeft = game.getTimeLimit();
								break;
							case sf::Keyboard::X:
								isPlaying = game.dropPiece(2, (timeLeft.asSeconds() / game.getTimeLimit().asSeconds()));
								timeLeft = game.getTimeLimit();
								break;
							case sf::Keyboard::D:
								isPlaying = game.dropPiece(3, (timeLeft.asSeconds() / game.getTimeLimit().asSeconds()));
								timeLeft = game.getTimeLimit();
								break;
							case sf::Keyboard::C:
								isPlaying = game.dropPiece(4, (timeLeft.asSeconds() / game.getTimeLimit().asSeconds()));
								timeLeft = game.getTimeLimit();
								break;
							case sf::Keyboard::F:
								isPlaying = game.dropPiece(5, (timeLeft.asSeconds() / game.getTimeLimit().asSeconds()));
								timeLeft = game.getTimeLimit();
								break;
							case sf::Keyboard::V:
								isPlaying = game.dropPiece(6, (timeLeft.asSeconds() / game.getTimeLimit().asSeconds()));
								timeLeft = game.getTimeLimit();
								break;
							case sf::Keyboard::G:
								isPlaying = game.dropPiece(7, (timeLeft.asSeconds() / game.getTimeLimit().asSeconds()));
								timeLeft = game.getTimeLimit();
								break;

						}
					default:
						break;
				}
			}
			window.clear();
			window.draw(bgSprite);

			// get and draw stats
			scoreText.setString(to_string(game.getScore()));
			window.draw(scoreText);
			levelValueText.setString(to_string(game.getLevelValue()));
			window.draw(levelValueText);
			nextLevelValueText.setString(game.getNextLevelValue());
			window.draw(nextLevelValueText);

			// get and draw next pieces
			nextPiece1.setString(checkPiece(game.lookPieceQ(0)));
			window.draw(nextPiece1);
			nextPiece2.setString(checkPiece(game.lookPieceQ(1)));
			window.draw(nextPiece2);
			nextPiece3.setString(checkPiece(game.lookPieceQ(2)));
			window.draw(nextPiece3);

			// get and draw timers
			sf::Time delta = clock.restart();
			timeElapsed += delta; // time elapsed
			stringstream tempTime;
			tempTime << fixed << setprecision(1) << timeElapsed.asSeconds();
			timeElapsedText.setString(tempTime.str());
			window.draw(timeElapsedText);
			timeLeft -= delta; // time left
			if (timeLeft.asSeconds() < 0) { // first check for strike
				strikes++;
				if (strikes > 3) {
					isPlaying = false;
				}
				timeLeft = game.getTimeLimit();
			}
			tempTime.str(string());
			tempTime << fixed << setprecision(1) << timeLeft.asSeconds();

			// get and draw strikes
			if (strikes == 0)	strikesText.setString("");
			else if (strikes == 1) strikesText.setString("X");
			else if (strikes == 2) strikesText.setString("XX");
			else strikesText.setString("XXX");
			window.draw(strikesText);

			// get and draw board
			for (int x = 0; x < 8; x++) {
				for (int y = 0; y < 8; y++) {
					if (game.getBoardState(x,y)) window.draw(boardBlock[x][y]);
				}
			}

			// get and draw timer bar
			float percentTimeLeft = timeLeft.asSeconds() / game.getTimeLimit().asSeconds();
			int percentTimerBarLength = static_cast<int>(400 * percentTimeLeft);
			timerBar.setSize(sf::Vector2f(20.f, percentTimerBarLength));
			timerBar.setPosition(timerBarXPos, (timerBarYPos + 400 - percentTimerBarLength));
			timerBar.setFillColor(sf::Color(255 * (1 - percentTimeLeft), 0, 0));
			window.draw(timerBar);

			// display everything
			window.display();

			lastScore = game.getScore(); // store last score so we can display score on gameover screen
		}
		else {
			game = gameBoard();
			sf::Event otherEvent;
			while (window.pollEvent(otherEvent))
			{
				if (otherEvent.type == sf::Event::Closed)
					window.close();
				if ((otherEvent.type == sf::Event::KeyPressed) && (otherEvent.key.code == sf::Keyboard::R)) {
					strikes = 0;
					timeElapsed = sf::seconds(0);
					clock.restart();
					isPlaying = true;
					scoreText.setPosition(50, 50);
				}
			}
			window.clear();
			window.draw(loseScreen);
			scoreText.setString("Final\nScore:\n" + to_string(lastScore));
			scoreText.setPosition(50, 200);
			window.draw(scoreText);
			window.display();
		}
	}

	return 0;
}