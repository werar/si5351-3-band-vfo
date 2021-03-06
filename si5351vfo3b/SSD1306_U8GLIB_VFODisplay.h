#ifndef SSD1306_U8GLIB_VFODISPLAY_H
#define SSD1306_U8GLIB_VFODISPLAY_H

/**
 * @file
 * @author  Mike Aiello N2HTT <n2htt@arrl.net>
 * @version 1.3
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This file contains class definitions for SSD1306_U8glib_VFODisplay. 
 * This class implements the methods defined in base class
 * VFODisplay, using the Google U8glib library, which may 
 * be found here:
 * 
 * https://code.google.com/p/u8glib/
 */
 
#include <Arduino.h> 
#include "VFODisplay.h"

/**
 * display page selection constants
 */
#define DISPLAY_FUNCTION_VFOS    0
#define DISPLAY_FUNCTION_FDELTA  1

/**
 * This class implements the methods defined in base class
 * VFODisplay, using the Google U8glib library.
 */
class SSD1306_U8glib_VFODisplay : public VFODisplay {
protected:   
   /**
    * pointer to externally defined SSD1306 display object
    */
#ifdef USE_SMALLER_SSD1306_128X64_BUFFER
   U8GLIB_SSD1306_128X64 *mp_display;
#else
   U8GLIB_SSD1306_128X64_2X *mp_display;   
#endif

   int             mi_displayFunc;     

   /**
    * show vfos display method 
    */
   void displayVFOScreen() {
      int ix = 0;
      int iy = 13;
      
      if (mb_show_heading_line) {
         iy = 11;
         
         // small yellow header line
         mp_display->setFont(u8g_font_6x12);
         mp_display->setPrintPos(ix,iy);
         mp_display->print(HEADING_PREFIX);
         mp_display->print(ml_freq_delta, DEC);
         
         // move to next line
         iy += 18;
      }
      
      for (int ii = 0; ii<mi_number_of_vfos; ++ii) {
         mi_displayLine = ii;
         mb_enabled = mpp_vfos[mi_displayLine]->isEnabled();
         ml_freq = mpp_vfos[mi_displayLine]->getFrequency();
         
         mp_display->setPrintPos(ix,iy);
         
         if (mi_currentVFO == mi_displayLine ) {
            mp_display->setFont(u8g_font_10x20_67_75);
         }
         else {
            mp_display->setFont(u8g_font_10x20);
         }

         formatIndicator();
         mp_display->write(ms_buffer[0]);
         
         mp_display->setFont(u8g_font_10x20);
         formatFrequencyMHz();  
         mp_display->print(ms_buffer);
         
         // move to next line
         iy += 17;
      }
   }
   
   /**
    * show vfos display method 
    */
   void displayFrequencyDeltaScreen() {
      int ix = 0;
      int iy = 31;
      
      mp_display->setPrintPos(ix,iy);
      mp_display->setFont(u8g_font_10x20_67_75);
      mp_display->write(mc_freqDelta);
      mp_display->setFont(u8g_font_10x20);
      mp_display->print(" freq=\n");
      
      iy += 17;
      mp_display->setPrintPos(ix,iy);
      mp_display->print(ml_freq_delta, DEC);
   }

    
   /**
    * refreshes and paints the display
    */
   virtual void paint() {
      // picture loop
      mp_display->firstPage();  
      do {
         switch(mi_displayFunc) {
            case DISPLAY_FUNCTION_FDELTA:
               displayFrequencyDeltaScreen();
               break;
            default:
            case DISPLAY_FUNCTION_VFOS:
               displayVFOScreen();      
               break;
         }
      } while( mp_display->nextPage() );
   }
   
public:
   /**
    * Constructor 
    * 
    * @param  vfo list
    * @param  num_vfos number of vfos to show in display
    */
   SSD1306_U8glib_VFODisplay(VFODefinition **vfos
                           , int num_vfos
                           , boolean show_header)
   : VFODisplay(vfos, num_vfos, show_header)
   , mi_displayFunc(0)
   {  
#ifdef USE_SMALLER_SSD1306_128X64_BUFFER
      mp_display = new U8GLIB_SSD1306_128X64(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);	// I2C / TWI;
#else
      mp_display = new U8GLIB_SSD1306_128X64_2X(U8G_I2C_OPT_NONE|U8G_I2C_OPT_DEV_0);	// I2C / TWI;
#endif      
      // assign default color value
      if ( mp_display->getMode() == U8G_MODE_R3G3B2 ) {
         mp_display->setColorIndex(255);     // white
      }
      else if ( mp_display->getMode() == U8G_MODE_GRAY2BIT ) {
         mp_display->setColorIndex(3);         // max intensity
      }
      else if ( mp_display->getMode() == U8G_MODE_BW ) {
         mp_display->setColorIndex(1);         // pixel on
      }
      else if ( mp_display->getMode() == U8G_MODE_HICOLOR ) {
         mp_display->setHiColorByRGB(255,255,255);
      }
   }
   /**
    * Destructor 
    */
   virtual ~SSD1306_U8glib_VFODisplay() 
   {
   }

   /**
    * display heading line and all vfos 
    * @param  f_delta current frequency change increment
    * @param  currentVFO subscript of selected vfo in list (from 0)
    */
   void showVFOs(unsigned long f_delta, short currentVFO) {
      ml_freq_delta = f_delta;
      mi_currentVFO = currentVFO;
      mi_displayFunc = DISPLAY_FUNCTION_VFOS;
      
      // repaint screen with current display
      paint();
   }
     
   /**
    * display new value of frequency change increment
    * @param  f_delta new frequency change increment
    */
   virtual void showFreqDeltaDisplay(unsigned long f_delta) {
      ml_freq_delta = f_delta;
      mi_displayFunc = DISPLAY_FUNCTION_FDELTA;
      
      // repaint screen with current display
      paint();
   }
};   
   
#endif // SSD1306_U8GLIB_VFODISPLAY_H
