/*
ArkanoidRemakeSDL
a sample of object-oriented game programming with C++. 
version 1.0, May 2016

Copyright (c) 2016 Giovanni Paolo Vigano'

The source code of the SDL library used by ArkanoidRemakeSDL can be found here:
https://www.libsdl.org/
---
ArkanoidRemakeSDL source code is licensed with the same zlib license:
(http://www.gzip.org/zlib/zlib_license.html)

This software is provided 'as-is', without any express or implied warranty. In 
no event will the authors be held liable for any damages arising from the use of
this software.

Permission is granted to anyone to use this software for any purpose, including 
commercial applications, and to alter it and redistribute it freely, subject to 
the following restrictions:

    1. The origin of this software must not be misrepresented; you must not 
    claim that you wrote the original software. If you use this software in a 
    product, an acknowledgment in the product documentation would be appreciated
    but is not required.

    2. Altered source versions must be plainly marked as such, and must not be
    misrepresented as being the original software.

    3. This notice may not be removed or altered from any source distribution.
*/


/// @brief BlockMatrix class definition
/// @file BlockMatrix.h
/// @author Giovanni Paolo Vigano'

#pragma once

#ifndef BLOCKMATRIX_H
#define BLOCKMATRIX_H

#include <Block.h>
#include <vector>

/// @addtogroup ArkanoidRemakeGame
/// @{

/// @brief Matrix with the wall blocks of the game.
/// @details This class stores a grid of blocks (see Block class),
/// providing methods and operators to access its elements.
class BlockMatrix
{
  unsigned int mCols;
  unsigned int mRows;
  std::vector<Block> mBlockList;
public:

  /// Default constructor
  BlockMatrix();

  /// Contructor: call Init()
  BlockMatrix(unsigned int rows, unsigned int cols);
  
  /// Initialize the matrix to hold the given rows and columns of blocks
  void Init(unsigned int rows, unsigned int cols);

  /// Access a block in the matrix using its row and column
  Block& operator()(unsigned int row, unsigned int col);

  /// Get the number of columns of blocks
  unsigned int Cols() const { return mCols; }

  /// Get the number of rows of blocks
  unsigned int Rows() const { return mRows; }

  /// Count blocks that are still visible (not completely destroyed)
  unsigned int CountVisibleBlocks() const;
};


///@}

#endif//BLOCKMATRIX_H

