#include "Game.h"

#include <iostream>
#include <string>

void Game::initWindow()
{
	window.create(sf::VideoMode(850, 850), "Chess");
	window.setFramerateLimit(60);
}

void Game::initVariables()
{
	textures[0][0].loadFromFile("pieces.png", sf::IntRect(531.25f, 0.0f, 106.25f, 106.25f));
	textures[0][1].loadFromFile("pieces.png", sf::IntRect(318.75f, 0.0f, 106.25f, 106.25f));
	textures[0][2].loadFromFile("pieces.png", sf::IntRect(212.5f, 0.0f, 106.25f, 106.25f));
	textures[0][3].loadFromFile("pieces.png", sf::IntRect(425.0f, 0.0f, 106.25f, 106.25f));
	textures[0][4].loadFromFile("pieces.png", sf::IntRect(0.0f, 0.0f, 106.25f, 106.25f));
	textures[0][5].loadFromFile("pieces.png", sf::IntRect(106.25f, 0.0f, 106.25f, 106.25f));

	textures[1][0].loadFromFile("pieces.png", sf::IntRect(531.25f, 106.25f, 106.25f, 106.25f));
	textures[1][1].loadFromFile("pieces.png", sf::IntRect(318.75f, 106.25f, 106.25f, 106.25f));
	textures[1][2].loadFromFile("pieces.png", sf::IntRect(212.5f, 106.25f, 106.25f, 106.25f));
	textures[1][3].loadFromFile("pieces.png", sf::IntRect(425.0f, 106.25f, 106.25f, 106.25f));
	textures[1][4].loadFromFile("pieces.png", sf::IntRect(0.0f, 106.25f, 106.25f, 106.25f));
	textures[1][5].loadFromFile("pieces.png", sf::IntRect(106.25f, 106.25f, 106.25f, 106.25f));

	chessboard_texture.loadFromFile("chessboard.png");
	chessboard.setTexture(chessboard_texture);

	circle.setRadius(25.0f);
	circle.setFillColor(sf::Color(30,52,52,205));
	circle.setOrigin(25.0f, 25.0f);
	
	promotion_box_texture[0].loadFromFile("pieces.png", sf::IntRect(106.25f, 0.0f, 4.0f * 106.25f, 106.25f));
	promotion_box_texture[1].loadFromFile("pieces.png", sf::IntRect(106.25f, 106.25f, 4.0f * 106.25f, 106.25f));
	promotion_box.setTexture(promotion_box_texture[0]);
	promotion_box.setPosition(2.0f * 106.25f, 3.0f * 106.25f);

	endgame_box_texture[0].loadFromFile("whitewins.png");
	endgame_box_texture[1].loadFromFile("blackwins.png");
	endgame_box_texture[2].loadFromFile("draw.png");

	endgame_box.setTexture(endgame_box_texture[0]);
	endgame_box.setOrigin(250.0f, 90.0f);
	endgame_box.setPosition(4.0f * 106.25f, 3.5f * 106.25f);

	promotion_background_texture.loadFromFile("promotionbackground.png");
	promotion_background.setTexture(promotion_background_texture);
	promotion_background.setOrigin(250.0f, 90.0f);
	promotion_background.setPosition(4.0f * 106.25f, 3.5f * 106.25f);

	FEN_map[0][0] = 'P';
	FEN_map[0][1] = 'N';
	FEN_map[0][2] = 'B';
	FEN_map[0][3] = 'R';
	FEN_map[0][4] = 'K';
	FEN_map[0][5] = 'Q';

	FEN_map[1][0] = 'p';
	FEN_map[1][1] = 'n';
	FEN_map[1][2] = 'b';
	FEN_map[1][3] = 'r';
	FEN_map[1][4] = 'k';
	FEN_map[1][5] = 'q';

	numbers[0] = '1';
	numbers[1] = '2';
	numbers[2] = '3';
	numbers[3] = '4';
	numbers[4] = '5';
	numbers[5] = '6';
	numbers[6] = '7';
	numbers[7] = '8';

	letters[0] = 'a';
	letters[1] = 'b';
	letters[2] = 'c';
	letters[3] = 'd';
	letters[4] = 'e';
	letters[5] = 'f';
	letters[6] = 'g';
	letters[7] = 'h';

	for (int i = 0; i < 64; i++)
	{
		sprites_to_display[i] = nullptr;
	}

	// "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" "k6n/8/8/8/8/8/8/K6N b - - 0 0"
	std::string starting_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

	spawnWithFEN(starting_fen); 

	isMove = false;
	isPromotion = false;
	isEndgame = false;
	isScrolling = false;
	setPiecesPositions();

	game_positions.push_back(getFEN());
	positions_iterator = game_positions.begin();
}

bool Game::isRunning() const
{
	return window.isOpen();
}

Game::Game()
{
	initWindow();
	initVariables();
}

Game::~Game()
{
}

void Game::render()
{
	window.draw(chessboard);
	if (isScrolling)
	{
		drawPosition();
	}
	else
	{
		if (isMove)
		{
			drawPossibleMoves();
		}
		drawPieces();
		if (isPromotion)
		{
			window.draw(promotion_background);
			window.draw(promotion_box);
		}
		if (isEndgame)
		{
			window.draw(endgame_box);
		}
	}
	window.display();
}

void Game::update()
{
	pollEvents();
	if (isMove)
	{
		focused->setPosition(mouse.x - dx, mouse.y - dy);
	}
}

void Game::pollEvents()
{
	while (window.pollEvent(ev))
	{
		mouse = sf::Mouse::getPosition(window);

		// pawn promotion

		if (ev.type == sf::Event::MouseButtonPressed && ev.key.code == sf::Mouse::Left && isPromotion && !isEndgame && !isScrolling)
		{
			promotionPick();
			return;
		}

		// picking a piece

		if (ev.type == sf::Event::MouseButtonPressed && ev.key.code == sf::Mouse::Left && !isMove && !isPromotion && !isEndgame && !isScrolling)
		{
			piecePick();
			return;
		}
		
		// dropping a piece

		if (ev.type == sf::Event::MouseButtonReleased && ev.key.code == sf::Mouse::Left && isMove && !isPromotion && !isEndgame && !isScrolling)
		{
			pieceDrop();
			return;
		}

		// scrolling through the game
		
		if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Left)
		{
			if (positions_iterator == game_positions.begin())
			{
				return;
			}
			isScrolling = true;
			positions_iterator--;
			drawWithFEN(*positions_iterator);
			setSpritesPositions();
			return;
		}

		if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Right && positions_iterator != game_positions.end() - 1)
		{
			isScrolling = true;
			positions_iterator++;
			if (positions_iterator == game_positions.end() - 1)
			{
				isScrolling = false;
				return;
			}
			drawWithFEN(*positions_iterator);
			setSpritesPositions();
			return;
		}

		if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Down)
		{
			isScrolling = true;
			positions_iterator = game_positions.begin();
			drawWithFEN(*positions_iterator);
			setSpritesPositions();
			return;
		}

		if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Up)
		{
			isScrolling = false;
			positions_iterator = game_positions.end() - 1;
			return;
		}
		
		// debug tools
		
		if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Space)
		{
			for (int i = 0; i < game_positions.size(); i++)
			{
				std::cout << game_positions[i] << std::endl;
			}
			std::cout << std::endl;
		}
		
		// start new game

		if (ev.type == sf::Event::KeyPressed && ev.key.code == sf::Keyboard::Enter && isEndgame)
		{
			spawnWithFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
			isMove = false;
			isPromotion = false;
			isEndgame = false;
			setPiecesPositions();
		}

		// close
		
		if (ev.type == sf::Event::Closed)
		{
			window.close();
		}
	}
}

void Game::spawnPiece(Color c, Type t, int p, bool m)
{
	if (pieces[p] != nullptr)
	{
		delete pieces[p];
		pieces[p] = nullptr;
	}

	pieces[p] = new Piece(c, t, p, m);
	int color = pieces[p]->getCol();
	int type = pieces[p]->getType();
	pieces[p]->setTexture(textures[color][type]);
}

void Game::changePlayer()
{
	if (player == White)
	{
		player = Black;
	}
	else
	{
		player = White;
	}
}

void Game::updatePossibleMoves(Color c, Type t, int p)
{
	for (int i = 0; i < 64; i++)
	{
		possible_moves[i] = false;
	}

	
	switch (t)
	{
	case Pawn:

		pawnMoves(p);

		break;
	case Knight:
		
		knightMoves(p);
	
		break;
	case Bishop:
		
		bishopMoves(p);

		break;
	case Rook:
		
		rookMoves(p);
		
		break;
	case King:
		
		kingMoves(p);

		break;
	case Queen:
		
		queenMoves(p);

		break;
	}
}

void Game::updateAttackedFields(Color c)
{
	for (int i = 0; i < 64; i++)
	{
		attacked_fields[i] = false;
	}

	for (int i = 0; i < 64; i++)
	{
		if (pieces[i] != nullptr && pieces[i]->getCol() == c)
		{
			int p = pieces[i]->getPos();

			switch (pieces[i]->getType())
			{
			case Pawn:
				
				pawnFields(p);

				break;
			case Knight:
			
				knightFields(p);

				break;
			case Bishop:
				
				bishopFields(p);

				break;
			case Rook:
				
				rookFields(p);

				break;
			case King:
				
				kingFields(p);

				break;
			case Queen:
				
				queenFields(p);

				break;
			}
		}
	}

}

bool Game::isCheck(Color c)
{
	int king_pos;

	Color test = oppositeColor(c);

	updateAttackedFields(oppositeColor(c)); 
	for (int i = 0; i < 64; i++)
	{
		if (pieces[i] != nullptr && pieces[i]->getCol() == c && pieces[i]->getType() == King)
		{
			king_pos = pieces[i]->getPos();
			break;
		}
	}
	if (attacked_fields[king_pos] == false)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool Game::isLegal(int p1, int p2)
{
	bool capture = false;
	Color c1 = pieces[p1]->getCol();
	Type  t1 = pieces[p1]->getType();
	bool  m1 = pieces[p1]->isMoved();

	Color c2; Type t2; bool m2;

	if (pieces[p2] != nullptr)
	{
		c2 = pieces[p2]->getCol();
		t2 = pieces[p2]->getType();
		m2 = pieces[p2]->isMoved();
		capture = true;
	}

	spawnPiece(c1, t1, p2, true);
	delete pieces[p1];
	pieces[p1] = nullptr;


	if (isCheck(c1) == false)
	{
		delete pieces[p2];
		pieces[p2] = nullptr;
		spawnPiece(c1, t1, p1, m1);
		if (capture == true)
		{
			spawnPiece(c2, t2, p2, m2);
		}
		setPiecesPositions();
		return true;
	}
	else
	{
		delete pieces[p2];
		pieces[p2] = nullptr;
		spawnPiece(c1, t1, p1, m1);
		if (capture == true)
		{
			spawnPiece(c2, t2, p2, m2);
		}
		setPiecesPositions();
		return false;
	}
}

bool Game::isCheckmate(Color c)
{
	for (int i = 0; i < 64; i++)
	{
		if (pieces[i] != nullptr && pieces[i]->getCol() == c)
		{	
			Color col = pieces[i]->getCol();
			Type type = pieces[i]->getType();
			int pos = pieces[i]->getPos();
			updatePossibleMoves(col, type, pos);
			for (int j = 0; j < 64; j++)
			{
				if (possible_moves[j] == true && isLegal(pos, j) == true)
				{
					return false;
				}
			}
		}
	}
	if (isCheck(c))
	{
		switch (c)
		{
		case White:
			endgame_status = Endgame::BLACKWINS;
			break;
		case Black:
			endgame_status = Endgame::WHITEWINS;
			break;
		}
	}
	else
	{
		endgame_status = Endgame::DRAW;
	}
	return true;
}

void Game::pawnMoves(int p)
{
	Color c = pieces[p]->getCol();
	switch (c)
	{
	case White:
		if (p - 8 >= 0)
		{
			if (pieces[p - 8] == nullptr)
			{
				possible_moves[p - 8] = true;
				if (p - 16 >= 0)
				{
					if (pieces[p]->isMoved() == false && pieces[p - 16] == nullptr)
					{
						possible_moves[p - 16] = true;
					}
				}
			}
		}
		if (p - 9 >= 0)
		{
			if (p % 8 == 0)
			{
				goto next_white;
			}
			if (pieces[p - 9] != nullptr && pieces[p - 9]->getCol() != c)
			{
				possible_moves[p - 9] = true;
			}
		}
		next_white:
		if (p - 7 >= 0)
		{
			if (p % 8 == 7)
			{
				break;
			}
			if (pieces[p - 7] != nullptr && pieces[p - 7]->getCol() != c)
			{
				possible_moves[p - 7] = true;
			}
		}
		break;
	case Black:
		if (p + 8 < 64)
		{
			if (pieces[p + 8] == nullptr)
			{
				possible_moves[p + 8] = true;
				if (pieces[p]->isMoved() == false && pieces[p + 16] == nullptr)
				{
					possible_moves[p + 16] = true;
				}
			}
		}
		if (p + 9 < 64)
		{
			if (p % 8 == 7)
			{
				goto next_black;
			}
			if (pieces[p + 9] != nullptr && pieces[p + 9]->getCol() != c)
			{
				possible_moves[p + 9] = true;
			}
		}
		next_black:
		if (p + 7 < 64)
		{
			if (p % 8 == 0)
			{
				break;
			}
			if (pieces[p + 7] != nullptr && pieces[p + 7]->getCol() != c)
			{
				possible_moves[p + 7] = true;
			}
		}
		break;
	}
}

void Game::knightMoves(int p)
{
	int patterns[8] = { -10, 6, -17, 15, -15, 17, -6, 10 }; 
	Color c = pieces[p]->getCol();

	int start = 0;
	int end = 8;

	if (p % 8 == 0)
	{
		start = 4;
	}
	if (p % 8 == 1)
	{
		start = 2;
	}
	if (p % 8 == 6)
	{
		end = 6;
	}
	if (p % 8 == 7)
	{
		end = 4;
	}

	for (int i = start; i < end; i++)
	{
		int offset = patterns[i];
		if (p + offset >= 0 && p + offset < 64)
		{
			if (pieces[p + offset] == nullptr || pieces[p + offset]->getCol() != c)
			{
				possible_moves[p + offset] = true;
			}
		}
	}
}

void Game::bishopMoves(int p)
{
	int patterns[4] = { 9, 7, -9, -7 };
	Color c = pieces[p]->getCol();
	int pos = p;
	for (int i = 0; i < 4; i++)
	{
		p = pos;
		int offset = patterns[i];
		while (p >= 0 && p < 64)
		{
			if (p + offset < 0 || p + offset > 63 || (p % 8 == 0 && (offset == -9 || offset == 7)) ||
				(p % 8 == 7 && (offset == -7 || offset == 9)))
			{
				break;
			}
			if (pieces[p + offset] != nullptr)
			{
				if (pieces[p + offset]->getCol() != c)
				{
					p = p + offset;
					possible_moves[p] = true;
					break;
				}
				break;
			}
			p = p + offset;
			possible_moves[p] = true;
		}
	}
}

void Game::rookMoves(int p)
{
	int patterns[4] = { 8, -8, 1, -1 };
	Color c = pieces[p]->getCol();
	int pos = p;
	for (int i = 0; i < 2; i++)
	{
		int offset = patterns[i];
		p = pos;
		while (p >= 0 && p < 64)
		{
			if (p + offset > 63 || p + offset < 0)
			{
				break;
			}
			if (pieces[p + offset] != nullptr)
			{
				if (pieces[p + offset]->getCol() != c)
				{
					p = p + offset;
					possible_moves[p] = true;
					break;
				}
				break;
			}
			p = p + offset;
			possible_moves[p] = true;
		}
	}
	for (int i = 2; i < 4 ; i++)
	{
		int offset = patterns[i];
		p = pos;
		while (p >= 0 && p < 64)
		{
			if ((offset == -1 && p % 8 == 0) || (offset == 1 && p % 8 == 7))
			{
				break;
			}
			if (pieces[p + offset] != nullptr)
			{
				if (pieces[p + offset]->getCol() != c)
				{
					p = p + offset;
					possible_moves[p] = true;
					break;
				}
				break;
			}
			p = p + offset;
			possible_moves[p] = true;
		}
	}
}

void Game::kingMoves(int p)
{
	int start = 0;
	int end = 8;
	int patterns[8] = { -9,-1,7,-8,8,-7,1,9 };
	if (p % 8 == 0)
	{
		start = 3;
	}
	if (p % 8 == 7)
	{
		end = 5;
	}
	Color c = pieces[p]->getCol();
	for (int i = start; i < end; i++)
	{
		int offset = patterns[i];
		if (p + offset >= 0 && p + offset < 64)
		{
			if (pieces[p + offset] == nullptr || pieces[p + offset]->getCol() != c)
			{
				possible_moves[p + offset] = true;
			}
		}
	}
	castles(p);
}

void Game::queenMoves(int p)
{
	bishopMoves(p);
	rookMoves(p);
}

void Game::castles(int p)
{
	if (pieces[p]->isMoved() == false) 
	{
		Color c = pieces[p]->getCol();
		updateAttackedFields(oppositeColor(player));
		switch (c)
		{
		case White:
			for (int i = 57; i < 60; i++)
			{
				if (pieces[i] != nullptr)
				{
					goto right;
				}
			}
			for (int i = 58; i < 60; i++)
			{
				if (attacked_fields[i] == true)
				{
					goto right;
				}
			}
			if (pieces[56] != nullptr && pieces[56]->getCol() == c && pieces[56]->isMoved() == false)
			{
				possible_moves[58] = true;
			}
		right:
			for (int i = 61; i < 63; i++)
			{
				if (pieces[i] != nullptr || attacked_fields[i] == true)
				{
					return;
				}
			}
			if (pieces[63] != nullptr && pieces[63]->getCol() == c && pieces[63]->isMoved() == false)
			{
				possible_moves[62] = true;
			}
			break;
		case Black:
			for (int i = 1; i < 4; i++)
			{
				if (pieces[i] != nullptr)
				{
					goto right2;
				}
			}
			for (int i = 2; i < 4; i++)
			{
				if (attacked_fields[i] == true)
				{
					goto right;
				}
			}
			if (pieces[0] != nullptr && pieces[0]->getCol() == c && pieces[0]->isMoved() == false)
			{
				possible_moves[2] = true;
			}
		right2:
			for (int i = 5; i < 7; i++)
			{
				if (pieces[i] != nullptr || attacked_fields[i] == true)
				{
					return;
				}
			}
			if (pieces[7] != nullptr && pieces[7]->getCol() == c && pieces[7]->isMoved() == false)
			{
				possible_moves[6] = true;
			}
			break;
		}
	}

}

void Game::pawnFields(int p)
{
	Color c = pieces[p]->getCol();
	switch (c)
	{
	case White:

		if (p - 9 >= 0 && p % 8 != 0)
		{
			attacked_fields[p - 9] = true;
		}
		if (p - 7 >= 0 && p % 8 != 7)
		{
			attacked_fields[p - 7] = true;
		}
		break;
	case Black:
		if (p + 9 < 63 && p % 8 != 7)
		{
			attacked_fields[p + 9] = true;
		}
		if (p + 7 < 63 && p % 8 != 0)
		{
			attacked_fields[p + 7] = true;
		}
		break;
	}
}

void Game::knightFields(int p)
{
	int patterns[8] = { -10, 6, -17, 15, -15, 17, -6, 10 };
	int start = 0;
	int end = 8;

	if (p % 8 == 0)
	{
		start = 4;
	}
	if (p % 8 == 1)
	{
		start = 2;
	}
	if (p % 8 == 6)
	{
		end = 6;
	}
	if (p % 8 == 7)
	{
		end = 4;
	}

	for (int i = start; i < end; i++)
	{
		int offset = patterns[i];
		if (p + offset >= 0 && p + offset < 64)
		{
			attacked_fields[p + offset] = true;
		}
	}
}

void Game::bishopFields(int p)
{
	int patterns[4] = { 9, 7, -9, -7 };
	Color c = pieces[p]->getCol();
	int pos = p;
	for (int i = 0; i < 4; i++)
	{
		p = pos;
		int offset = patterns[i];
		while (p >= 0 && p < 64)
		{
			if (p + offset < 0 || p + offset > 63 || (p % 8 == 0 && (offset == -9 || offset == 7)) ||
												     (p % 8 == 7 && (offset == -7 || offset == 9)))
			{
				break;
			}
			if (pieces[p + offset] != nullptr)
			{
				p = p + offset;
				attacked_fields[p] = true;
				break;
			}
			p = p + offset;
			attacked_fields[p] = true;
		}
	}
}

void Game::rookFields(int p)
{
	int patterns[4] = { 8, -8, 1, -1 };
	Color c = pieces[p]->getCol();
	int pos = p;
	for (int i = 0; i < 2; i++)
	{
		int offset = patterns[i];
		p = pos;
		while (p >= 0 && p < 64)
		{
			if (p + offset > 63 || p + offset < 0)
			{
				break;
			}
			if (pieces[p + offset] != nullptr)
			{
				p = p + offset;
				attacked_fields[p] = true;
				break;
			}
			p = p + offset;
			attacked_fields[p] = true;
		}
	}
	for (int i = 2; i < 4; i++)
	{
		int offset = patterns[i];
		p = pos;
		while (p >= 0 && p < 64)
		{
			if (p + offset > 63 || p + offset < 0 || (offset == -1 && p % 8 == 0) || (offset == 1 && p % 8 == 7))
			{
				break;
			}
			if (pieces[p + offset] != nullptr)
			{
				p = p + offset;
				attacked_fields[p] = true;
				break;
			}
			p = p + offset;
			attacked_fields[p] = true;
		}
	}
}

void Game::kingFields(int p)
{
	int start = 0;
	int end = 8;
	int patterns[8] = { -9,-1,7,-8,8,-7,1,9 };
	if (p % 8 == 0)
	{
		start = 4;
	}
	if (p % 8 == 7)
	{
		end = 5;
	}
	
	for (int i = start; i < end; i++)
	{
		int offset = patterns[i];
		if (p + offset >= 0 && p + offset < 64)
		{
			attacked_fields[p + offset] = true;
		}
	}
}

void Game::queenFields(int p)
{
	bishopFields(p);
	rookFields(p);
}

void Game::pawnPromotion()
{
	Color c = pieces[newP]->getCol();
	switch (c)
	{
	case White:
		promotion_box.setTexture(promotion_box_texture[0]);
		break;
	case Black:
		promotion_box.setTexture(promotion_box_texture[1]);
		break;
	}
	
	isPromotion = true;
	isMove = false;
	promotion_pos = newP;
	promotion_c = c;
	setPiecesPositions();
		
}

void Game::promotionPick()
{
	int x = floor(mouse.x / 106.25f);
	int y = floor(mouse.y / 106.25f);
	Color c = promotion_c;
	int p = promotion_pos;
	if (y == 3 && x > 1 && x < 6)
	{
		switch (x)
		{
		case 2:
			spawnPiece(c, Queen, p, false);
			break;
		case 3:
			spawnPiece(c, Bishop, p, false);
			break;
		case 4:
			spawnPiece(c, Knight, p, false);
			break;
		case 5:
			spawnPiece(c, Rook, p, false);
			break;
		}

		fen_enpassant = "-";
		halfmoves = 0;
		isPromotion = false;

		endMove();
		return;
	}
}

void Game::piecePick()
{
	int x = floor(mouse.x / 106.25f);
	int y = floor(mouse.y / 106.25f);
	int p = x + (8 * y);
	if (pieces[p] == nullptr || pieces[p]->getCol() != player)
	{
		return;
	}
	else
	{
		isMove = true;
		focused = pieces[p];
		oldX = x;
		oldY = y;
		oldP = p;
		dx = mouse.x - focused->getPosition().x;
		dy = mouse.y - focused->getPosition().y;
		updatePossibleMoves(focused->getCol(), focused->getType(), p);
	}

}

void Game::pieceDrop()
{
	int x = floor(mouse.x / 106.25f);
	int y = floor(mouse.y / 106.25f);
	newP = x + (8 * y);

	// en passant

	if (pieces[prev_pos2] != nullptr && pieces[prev_pos2]->getType() == Pawn && focused->getType() == Pawn &&
		abs(focused->getPos() - prev_pos2) == 1 && abs(prev_pos1 - prev_pos2) == 16 && abs(prev_pos2 - newP) == 8)
	{
		Color c1 = focused->getCol();
		Type t1 = focused->getType();
		bool m1 = focused->isMoved();

		Color c2 = pieces[prev_pos2]->getCol();
		Type t2 = pieces[prev_pos2]->getType();
		bool m2 = pieces[prev_pos2]->isMoved();

		spawnPiece(c1, t1, newP, true);
		delete pieces[oldP];
		pieces[oldP] = nullptr;
		delete pieces[prev_pos2];
		pieces[prev_pos2] = nullptr;

		if (isCheck(player) == true)
		{
			spawnPiece(c1, t1, oldP, m1);
			spawnPiece(c2, t2, prev_pos2, m2);
			delete pieces[newP];
			pieces[newP] = nullptr;
			isMove = false;
			focused = nullptr;
			return;
		}
		else
		{
			fen_enpassant = "-";
			halfmoves = 0;

			endMove();
			return;
		}


	}

	// when the piece is being dropped in its initial position
	// set its position back

	if (newP == oldP)
	{
		focused->setPosition(x * 106.25f, y * 106.25f);
		isMove = false;
		focused = nullptr;
		return;
	}

	Color c = focused->getCol();
	Type t = focused->getType();
	bool m = focused->isMoved();
	updatePossibleMoves(c, t, oldP);

	// when the move is against moving pattern of the piece
	// move it back at its initial position

	if (possible_moves[newP] == false)
	{
		focused->setPosition(oldX * 106.25f, oldY * 106.25f);
		isMove = false;
		focused = nullptr;
		return;
	}

	// castles

	if (focused->getType() == King && abs(newP - focused->getPos()) == 2)
	{
		delete pieces[oldP];
		pieces[oldP] = nullptr;
		focused = nullptr;

		switch (newP)
		{
		case 2:
			spawnPiece(player, King, newP, true);
			delete pieces[0];
			pieces[0] = nullptr;
			spawnPiece(player, Rook, 3, true);
			break;
		case 6:
			spawnPiece(player, King, newP, true);
			delete pieces[7];
			pieces[7] = nullptr;
			spawnPiece(player, Rook, 5, true);
			break;
		case 58:
			spawnPiece(player, King, newP, true);
			delete pieces[56];
			pieces[56] = nullptr;
			spawnPiece(player, Rook, 59, true);
			break;
		case 62:
			spawnPiece(player, King, newP, true);
			delete pieces[63];
			pieces[63] = nullptr;
			spawnPiece(player, Rook, 61, true);
			break;
		}
		fen_enpassant = "-";
		halfmoves++;

		endMove();
		return;
	}

	// check if the player is being checked after a move from 'oldP' to 'p'

	if (isLegal(oldP, newP) == false)
	{
		isMove = false;
		focused = nullptr;
		return;
	}

	// when the code reaches that point, that means the move is legal

	if (pieces[newP] != nullptr || pieces[oldP]->getType() == Pawn)
	{
		halfmoves = 0;
	}
	else
	{
		halfmoves++;
	}

	spawnPiece(c, t, newP, true);
	delete pieces[oldP];
	pieces[oldP] = nullptr;

	// pawn promotion

	if ((newP < 8 || newP >= 56) && pieces[newP]->getType() == Pawn)
	{
		pawnPromotion();
		return;
	}

	if (abs(newP - oldP) == 16 && pieces[newP]->getType() == Pawn)
	{
		switch (player)
		{
		case White:
			enpassant_square = newP + 8;
			break;
		case Black:
			enpassant_square = newP - 8;
			break;
		}
		fen_enpassant = squaretoFEN(enpassant_square);
	}
	else
	{
		fen_enpassant = "-";
	}

	endMove();
	return;
}

void Game::endGame()
{
	switch (endgame_status)
	{
	case Endgame::WHITEWINS:
		endgame_box.setTexture(endgame_box_texture[0]);
		break;
	case Endgame::BLACKWINS:
		endgame_box.setTexture(endgame_box_texture[1]);
		break;
	case Endgame::DRAW:
		endgame_box.setTexture(endgame_box_texture[2]);
		break;
	}
	isEndgame = true;
}

void Game::endMove()
{
	if (player == Black)
	{
		fullmoves++;
	}
	focused = nullptr;
	isMove = false;
	setPiecesPositions();
	changePlayer();
	prev_pos1 = oldP;
	prev_pos2 = newP;
	game_positions.push_back(getFEN());
	positions_iterator = game_positions.end() - 1;
	if (isCheckmate(player) == true)
	{
		endGame();
	}
	return;
}

Color Game::oppositeColor(Color c)
{
	if (c == White)
	{
		return Black;
	}
	else
	{
		return White;
	}
}

std::string Game::getFEN()
{
	std::string FEN = "";
	int empty_spaces = 0;
	for (int i = 0; i < 64; i++)
	{
		if (pieces[i] == nullptr)
		{
			empty_spaces++;
		}
		else
		{	
			if (empty_spaces != 0)
			{
				FEN += std::to_string(empty_spaces);
				empty_spaces = 0;
			}
			Color c = pieces[i]->getCol();
			Type t = pieces[i]->getType();
			FEN += FEN_map[c][t];
		}
		if (i % 8 == 7)
		{
			if (empty_spaces != 0)
			{
				FEN += std::to_string(empty_spaces);
				empty_spaces = 0;
			}
			if (i != 63)
			{
				FEN += '/';
			}
		}
	}

	FEN += " ";

	if (player == White)
	{
		FEN += 'w';
	}
	else
	{
		FEN += 'b';
	}

	FEN += " ";

	bool castling = false;

	if (pieces[60] != nullptr && pieces[60]->getType() == King && pieces[60]->isMoved() == false)
	{
		if (pieces[63] != nullptr && pieces[63]->getType() == Rook && pieces[63]->isMoved() == false)
		{
			FEN += 'K';
			castling = true;
		}
		if (pieces[56] != nullptr && pieces[56]->getType() == Rook && pieces[56]->isMoved() == false)
		{
			FEN += 'Q';
			castling = true;
		}
	}

	if (pieces[4] != nullptr && pieces[4]->getType() == King && pieces[4]->isMoved() == false)
	{
		if (pieces[7] != nullptr && pieces[7]->getType() == Rook && pieces[7]->isMoved() == false)
		{
			FEN += 'k';
			castling = true;
		}
		if (pieces[0] != nullptr && pieces[0]->getType() == Rook && pieces[0]->isMoved() == false)
		{
			FEN += 'q';
			castling = true;
		}
	}

	if (castling == false)
	{
		FEN += "- ";
	}
	else
	{
		FEN += " ";
	}

	FEN += fen_enpassant;
	FEN += " ";

	FEN += std::to_string(halfmoves);
	FEN += " ";
	FEN += std::to_string(fullmoves);

	return FEN;
}

std::string Game::squaretoFEN(int sqr)
{
	std::string output = "";
	int letter = sqr % 8;
	int number = (sqr - letter) / 8;
	output += letters[letter];
	output += numbers[number];
	return output;

}

void Game::spawnWithFEN(std::string fen)
{
	for (int i = 0; i < 64; i++)
	{
		pieces[i] = nullptr;
	}
	std::vector<std::string> fen_parts = splitFEN(fen);
	int spawn_pos = 0;
	for (int i = 0; i < fen_parts[0].length(); i++)
	{
		start:
		if (fen_parts[0][i] == '/')
		{
			continue;
		}
		for (int j = 0; j < 8; j++)
		{
			if (fen_parts[0][i] == numbers[j])
			{
				spawn_pos += j + 1;
				i++;
				goto start;
			}
		}
		spawnWithChar(fen_parts[0][i], spawn_pos);
		spawn_pos++;
	}

	switch (fen_parts[1][0])
	{
	case 'w':
		player = White;
		break;
	case 'b':
		player = Black;
		break;
	}

	if (fen_parts[2][0] != '-')
	{
		for (int i = 0; i < fen_parts[2].length(); i++)
		{
			switch (fen_parts[2][i])
			{
			case 'K':
				spawnPiece(White, King, 60, false);
				spawnPiece(White, Rook, 63, false);
				break;
			case 'Q':
				spawnPiece(White, King, 60, false);
				spawnPiece(White, Rook, 56, false);
				break;
			case 'q':
				spawnPiece(Black, King, 4, false);
				spawnPiece(Black, Rook, 0, false);
				break;
			case 'k':
				spawnPiece(Black, King, 4, false);
				spawnPiece(Black, Rook, 7, false);
				break;
			}
		}
	}


	if (fen_parts[3][0] != '-')
	{
		fen_enpassant = fen_parts[3];
		int target_pos = 0;
		for (int i = 0; i < 8; i++)
		{
			if (fen_parts[3][0] == letters[i])
			{
				target_pos += i;
				break;
			}
		}
		for (int i = 0; i < 8; i++)
		{
			if (fen_parts[3][1] == numbers[i])
			{
				target_pos += i * 8;
				break;
			}
		}
		switch (player)
		{
		case White:
			prev_pos1 = target_pos - 8;
			prev_pos2 = target_pos + 8;
			break;
		case Black:
			prev_pos1 = target_pos + 8;
			prev_pos2 = target_pos - 8;
			break;
		}
	}
	else
	{
		fen_enpassant = '-';
	}
	
	halfmoves = fen_parts[4][0] - 48;
	fullmoves = fen_parts[5][0] - 48;
}

void Game::spawnWithChar(char c, int p)
{
	switch (c)
	{
	case 'p':
		if (p < 8 || p > 15)
		{
			spawnPiece(Black, Pawn, p, true);
		}
		else
		{
			spawnPiece(Black, Pawn, p, false);
		}
		break;
	case 'n':
		spawnPiece(Black, Knight, p, true);
		break;
	case 'b':
		spawnPiece(Black, Bishop, p, true);
		break;
	case 'r':
		spawnPiece(Black, Rook, p, true);
		break;
	case 'q':
		spawnPiece(Black, Queen, p, true);
		break;
	case 'k':
		spawnPiece(Black, King, p, true);
		break;
	case 'P':
		if (p < 48 || p > 55)
		{
			spawnPiece(White, Pawn, p, true);
		}
		else
		{
			spawnPiece(White, Pawn, p, false);
		}
		break;
	case 'N':
		spawnPiece(White, Knight, p, true);
		break;
	case 'B':
		spawnPiece(White, Bishop, p, true);
		break;
	case 'R':
		spawnPiece(White, Rook, p, true);
		break;
	case 'Q':
		spawnPiece(White, Queen, p, true);
		break;
	case 'K':
		spawnPiece(White, King, p, true);
		break;
	}
}

std::vector<std::string> Game::splitFEN(std::string fen)
{
	std::vector<std::string> output;
	std::string part = "";
	for (int i = 0; i < fen.length(); i++)
	{
		if (i == fen.length() - 1)
		{
			part += fen[i];
			output.push_back(part);
			break;
		}
		if (fen[i] == ' ')
		{
			output.push_back(part);
			part = "";
			continue;
		}
		part += fen[i];
	}
	return output;
}

void Game::drawPieces()
{
	for (int i = 0; i < 64; i++)
	{
		if (pieces[i] != nullptr)
		{
			window.draw(*pieces[i]);
		}
	}
}

void Game::drawPossibleMoves()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (possible_moves[i + j * 8] == true)
			{
				circle.setPosition(i * 106.25f, j * 106.25f);
				circle.setPosition(circle.getPosition() + sf::Vector2f(106.25f / 2, 106.25f / 2));
				window.draw(circle);
			}
		}
	}

}

void Game::drawWithFEN(std::string fen)
{
	for (int i = 0; i < 64; i++)
	{
		if (sprites_to_display[i] != nullptr)
		{
			delete sprites_to_display[i];
		}
	}
	
	for (int i = 0; i < 64; i++)
	{
		sprites_to_display[i] = nullptr;
	}

	fen = splitFEN(fen)[0];

	int draw_pos = 0;
	for (int i = 0; i < fen.length(); i++)
	{
	start:
		if (fen[i] == '/')
		{
			continue;
		}
		for (int j = 0; j < 8; j++)
		{
			if (fen[i] == numbers[j])
			{
				draw_pos += j + 1;
				i++;
				goto start;
			}
		}
		drawWithChar(fen[i], draw_pos);
		draw_pos++;
	}
}

void Game::drawWithChar(char c, int p)
{
	sprites_to_display[p] = new sf::Sprite;
	switch (c)
	{
	case 'p':
		sprites_to_display[p]->setTexture(textures[1][0]);
		break;
	case 'n':
		sprites_to_display[p]->setTexture(textures[1][1]);
		break;
	case 'b':
		sprites_to_display[p]->setTexture(textures[1][2]);
		break;
	case 'r':
		sprites_to_display[p]->setTexture(textures[1][3]);
		break;
	case 'q':
		sprites_to_display[p]->setTexture(textures[1][5]);
		break;
	case 'k':
		sprites_to_display[p]->setTexture(textures[1][4]);
		break;
	case 'P':
		sprites_to_display[p]->setTexture(textures[0][0]);
		break;
	case 'N':
		sprites_to_display[p]->setTexture(textures[0][1]);
		break;
	case 'B':
		sprites_to_display[p]->setTexture(textures[0][2]);
		break;
	case 'R':
		sprites_to_display[p]->setTexture(textures[0][3]);
		break;
	case 'Q':
		sprites_to_display[p]->setTexture(textures[0][5]);
		break;
	case 'K':
		sprites_to_display[p]->setTexture(textures[0][4]);
		break;
	}
}

void Game::setSpritesPositions()
{	
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (sprites_to_display[i + j * 8] != nullptr)
			{
				sprites_to_display[i + j * 8]->setPosition(i * 106.25f, j * 106.25f);
			}
		}
	}
}

void Game::drawPosition()
{
	for (int i = 0; i < 64; i++)
	{
		if (sprites_to_display[i] != nullptr)
		{
			window.draw(*sprites_to_display[i]);
		}
	}
}

void Game::setPiecesPositions()
{
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (pieces[i + j * 8] != nullptr)
			{
				pieces[i + j * 8]->setPosition(i * 106.25f, j * 106.25f);
			}
		}
	}
}