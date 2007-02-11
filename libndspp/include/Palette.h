#ifndef Palette_h_seen
#define Palette_h_seen


namespace nds
{
  //! Representation of color
  class Color
  {
    public:
      /*! @brief Create a color using a NDS format short
      * @param c 15 bit color value. Red value is stored in the lowest 5
      *          bits, then green then blue. The bits are like this: RRRRRGGGGGBBBBB.
      */
      Color(unsigned short c);

      /*! @brief Create a color using red, green, blue
       * @param red component, allowed values 0-31.
       * @param green component, allowed values 0-31.
       * @param blue component, allowed values 0-31.
       */
      Color(unsigned int red, unsigned int green, unsigned int blue);

      //! get the red component of the color
      inline unsigned int red() const;
      //! set the red component of the color
      inline void red(unsigned int);
      //! get the green component of the color
      inline unsigned int green() const;
      //! set the green component of the color
      inline void green(unsigned int);
      //! get the blue component of the color
      inline unsigned int blue() const;
      //! set the blue component of the color
      inline void blue(unsigned int);

      /*! @brief Set the red, green and blue components.
       * @param red component
       * @param green component
       * @param blue component
       */
      void set(unsigned int red, unsigned int green, unsigned int blue);

      /*! @brief operator to cast to an unsigned short
      * @returns NDS representation of color.
      */
      inline operator unsigned short ();
      
    private:
      //! NDS format color
      unsigned short m_c;
  };
  /*! @brief Wraps the Background palette memory 
   */
  class Palette
  {
    public:
      /*! @brief Default palette (256 colors).
       * @param screen the screen for the palette. 0 for main screen, 1 for sub screen
       */
      Palette(int screen);

      /*! @brief 16 color palette.
       * @param screen the screen for the palette. 0 for main screen, 1 for sub screen
       * @param palette index of the 16 color palette (0-15)
       */
      Palette(int screen, int palette);

      /*! @brief get the color at an index.
       * @param i color index.
       * @returns Color representation of the NDS color.
       */
      inline const Color operator[] (int i) const;
      /*! @brief get reference to color at an index
       * @param i color index
       * @returns unsigned short representation of the NDS color.
       */
      inline unsigned short & operator[] (int i);

      /*! @brief Load palette data.
       * @param palette pointer to palette data
       * @param length number of colors to load. If 0 then will load however
       * many are required to fill this palette.
       */
      void load(const unsigned short * palette, unsigned int length=0);

      /*! @brief Sets all the colors in the palette to black (0,0,0)
       */
      void clear();

    protected:
      //! pointer to palette memory
      unsigned short *m_p;
      
    private:
      //! is 16 color palette or not
      bool m_16;
  };

  //! Object (sprite) palette
  class ObjectPalette: public Palette
  {
    public:
      /*! @brief Default object/sprite palette (256 colors).
       * @param screen the screen for the palette. 0 for main screen, 1 for sub screen.
       */
      ObjectPalette(int screen);
      
      /*! @brief 16 color object/sprite palette.
       * @param screen the screen for the palette. 0 for main screen, 1 for sub screen.
       * @param palette index of the 16 color palette (0-15).
       */
      ObjectPalette(int screen, int palette);
  };
  #include "Palette.inl"
};
#endif
