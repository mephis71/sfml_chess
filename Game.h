#pragma once
#include <SFML/Graphics.hpp>

#include "Piece.h"

enum class Endgame
{
	WHITEWINS,BLACKWINS,DRAW
};


class Game
{
private:

	sf::RenderWindow window;
	sf::Event ev;

	void initWindow();
	void initVariables();

	sf::Texture textures[2][6];
	sf::Sprite chessboard;
	sf::Texture chessboard_texture;
	sf::CircleShape circle;
	sf::Sprite promotion_box;
	sf::Texture promotion_box_texture[2];
	sf::Sprite promotion_background;
	sf::Texture promotion_background_texture;
	sf::Sprite endgame_box;
	sf::Texture endgame_box_texture[3];
	sf::Sprite* sprites_to_display[64];

	Piece* pieces[64];

	sf::Vector2i mouse;
	int dx, dy;
	int oldX, oldY, oldP;
	int newP;
	Color player;
	bool isMove, isPromotion, isEndgame, isScrolling;
	Piece* focused;
	bool possible_moves[64];
	bool attacked_fields[64];
	int prev_pos1, prev_pos2;
	int promotion_pos;
	Color promotion_c;

	char FEN_map[2][6];
	char letters[8];
	char numbers[8];
	int halfmoves;
	int fullmoves;
	int enpassant_square;
	std::string fen_enpassant;

	Endgame endgame_status;

	std::vector<std::string> game_positions;
	std::vector<std::string>::iterator positions_iterator;


public:

	Game();
	~Game();

	void update();

	void render();
	void setPiecesPositions();

	void drawPieces();
	void drawPossibleMoves();

	void pollEvents();

	bool isRunning() const;

	// Game functions

	void spawnPiece(Color c, Type t, int p, bool m);
	void changePlayer();
	void updatePossibleMoves(Color c, Type t, int p);
	void updateAttackedFields(Color c);
	bool isCheck(Color c);
	bool isLegal(int p1, int p2);
	bool isCheckmate(Color c);

	void pawnMoves(int p);
	void knightMoves(int p);
	void bishopMoves(int p);
	void rookMoves(int p);
	void kingMoves(int p);
	void queenMoves(int p);
	void castles(int p);

	void pawnFields(int p);
	void knightFields(int p);
	void bishopFields(int p);
	void rookFields(int p);
	void kingFields(int p);
	void queenFields(int p);

	void pawnPromotion();
	void promotionPick();
	void piecePick();
	void pieceDrop();
	void endGame();
	void endMove();

	Color oppositeColor(Color c);

	std::string getFEN();
	std::string squaretoFEN(int sqr);
	void spawnWithFEN(std::string fen);
	void spawnWithChar(char c, int p);
	std::vector<std::string> splitFEN(std::string fen);

	void drawWithFEN(std::string fen);
	void drawWithChar(char c, int p);
	void setSpritesPositions();
	void drawPosition();


};