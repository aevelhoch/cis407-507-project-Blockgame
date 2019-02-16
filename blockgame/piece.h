#pragma once
#ifndef PIECE_H
#define PIECE_H

// The game has 8 different pieces available, each fitting in a 2x2 square. I will represent them a la ":." or ".." or ":'"
// Piece | Shape
// Dot     "."
// I       ":"
// Line    ".."
// O       "::"
// R       ":'"
// 7       "':"
// L       ":."
// J       ".:"
// none    no piece
enum piece { pieceDot, pieceI, pieceLine, pieceO, pieceR, piece7, pieceL, pieceJ, none };

#endif