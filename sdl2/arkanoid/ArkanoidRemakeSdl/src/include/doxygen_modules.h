// Modules definition for automatic documentation generation (doxygen)

/** @mainpage Arkanoid Remake SDL
* @author Giovanni Paolo Vigano'
* @version 1.0
* @date 2015-2016
*
Arkanoid Remake implemented with SDL.
 The game application is an instance of the classe  @c ArkanoidRemakeSdl derived from a generic class @c  ArkanoidRemake
that defines the behavior of the application independent from the graphic platform chosen.
An abstract interface @c App is defined for a generic application, implemented in @c ArkanoidRemake and @c ArkanoidRemakeSdl
 classes.
The @c ArkanoidRemakeSdl class delegates the application window management to the SDL toolkit.
@note All the resources  used in this game (images, sounds and musics) are royalty free and/or edited by the author.
@note This game is based on the SDL2 library.
@see ArkanoidRemakeSdl
@see ArkanoidRemake
@see App
@see sdltk
*/

/// @defgroup AppModule Application
/// @brief Application interface definition.
/// @author Giovanni Paolo Vigano'

  
/// @defgroup ArkanoidRemakeGame Arkanoid Remake
/// @brief  Game definition for an Arkanoid remake
/// @author Giovanni Paolo Vigano'


/// @defgroup ArkanoidRemakeGameSdl Arkanoid Remake SDL
/// @brief Implementation of an Arkanoid remake game using SDL.
/// @details The definition of the Arkanoid remake game is decomposed into a set of classes,
/// each one defining a specific layer (and inheriting from the previous one):<BR>
/// @li @c App: the base class, it defines the generic application interface
/// @li @c ArkanoidRemake: game definition for an Arkanoid remake
/// @li @c ArkanoidRemakeSdl: Implementation of an Arkanoid remake game using SDL
/// The ArkanoidRemakeSdl class delegates the application window management to the SDL toolkit (sdttk).
/// @author Giovanni Paolo Vigano'


/// @defgroup sdltk SDL toolkit
/// @brief Implementation of an application using SDL
/// @details SDLTK: (SDL toolkit) implements graphics and sound, manages
///  the keyboard and mouse input. This library is definitely neither complete
/// nor optimized, and anyone with a bit of programming experience with SDL
/// could do better (for the author this was the first programming experience
/// with SDL). This simple library was built with the intention of hiding all
/// implementation details, including internal data structures of SDL, to allow
/// focusing on the operating logic while exploring the code of the
/// application, without having to learn SDL (or any other library that could
/// take its place). This approach would allow to use the interface implemented
/// here with SDL to create a new version based on a different library, ideally
/// without changes to the code of the other modules.
/// In this module a sdltk namespace is defined, with the definition
/// of the main SdlApp class and some auxiliary classes and definitions.
/// @author Giovanni Paolo Vigano'

  

/// @addtogroup ArkanoidRemakeGameSdl
///@{ 
  /// @addtogroup ArkanoidRemakeGame
  ///@{ 
    /// @addtogroup AppModule
    ///@{ 
    ///@}
  ///@}
  /// @addtogroup sdltk
  ///@{ 
  ///@}
///@}

