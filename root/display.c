/*
 *  display.c
 * 
 * for high-level drawing functions.
 * 
 */

#include "display.h"

#include "md380.h"
#include "version.h"
#include "tooldfu.h"
#include "config.h"
#include "gfx.h"
#include "printf.h"
#include "string.h"
#include "addl_config.h"
#include "ambe.h"
#include "usersdb.h"
#include "dmr.h"
#include "console.h"
#include "netmon.h"
#include "radiostate.h"
#include "unclear.h"
#include "etsi.h"
#include "codeplug.h"

char eye_paltab[] = {
    0xd7, 0xd8, 0xd6, 0x00, 0x88, 0x8a, 0x85, 0x00, 0xe1, 0xe2, 0xe0, 0x00, 0xff, 0xff, 0xff, 0x00,
    0xae, 0xae, 0xaf, 0x00, 0x24, 0x4e, 0x8a, 0x00, 0x5d, 0x88, 0xbb, 0x00, 0xd1, 0xd2, 0xd4, 0x00,
    0xf4, 0xf4, 0xf4, 0x00, 0x3c, 0x66, 0x9f, 0x00, 0xdb, 0xe6, 0xf3, 0x00, 0x48, 0x73, 0xaa, 0x00,
    0xb6, 0xb8, 0xb4, 0x00, 0x5e, 0x6a, 0x77, 0x00
};
char eye_pix[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x11, 0x10, 0x00, 0x00, 0x23,
    0x41, 0x11, 0x31, 0x00, 0x01, 0x14, 0x55, 0x55, 0x61, 0x00, 0x21, 0x75, 0x88, 0x59, 0x94, 0x31, 0x3a, 0x85, 0x88, 0x56,
    0x57, 0x73, 0x21, 0x86, 0x55, 0x5b, 0x67, 0x41, 0x13, 0x48, 0x66, 0x69, 0x71, 0xc1, 0x0c, 0x13, 0x47, 0x33, 0x11, 0x10,
    0x00, 0x03, 0xdc, 0xd1, 0xd0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const gfx_pal eye_pal = {14, 0, eye_paltab};
const gfx_bitmap bmp_eye = {12, 12, 6, 4, eye_pix, &eye_pal, 0};

#ifdef FW_D13_020
#define D_ICON_EYE_X 65
#define D_ICON_EYE_Y 1
#endif
#ifdef FW_S13_020
// on MD390 draw promiscous mode eye closed to S-Meter due to GPS-symbol an standard position
#define D_ICON_EYE_X 20
#define D_ICON_EYE_Y 1
#endif

void draw_eye_opt()
{
#if defined(FW_D13_020) || defined(FW_S13_020)
    // draw promiscous mode eye symbol 
    if( global_addl_config.promtg == 1 ) {
        gfx_drawbmp((char *) &bmp_eye, D_ICON_EYE_X, D_ICON_EYE_Y);
    }
#endif
}

// Takes a positive(!) integer amplitude and computes 200*log10(amp),
// centi Bel, approximtely. If the given parameter is 0 or less, this
// function returns -1.  tnx to sellibitze

int intCentibel(long ampli)
{
    if( ampli <= 0 )
        return -1; // invalid
    int log_2 = 0;
    while (ampli >= 32 * 8) {
        ampli >>= 1 + 3;
        log_2 += 1 + 3;
    }
    while (ampli >= 32) {
        ampli >>= 1;
        log_2 += 1;
    }
    // 1 <= ampli < 32
    static const short fine[] = {
        -1, 0, 60, 95, 120, 140, 156, 169,
        181, 191, 200, 208, 216, 223, 229, 235,
        243, 249, 253, 258, 262, 266, 270, 274,
        278, 281, 285, 288, 291, 294, 297, 300
    };
    return (log_2 * 301 + 2) / 5 + fine[ampli];
}

void draw_micbargraph()
{
    if( gui_opmode2 == OPM2_MENU ) {
        // case for pressing the PTT during 'Manual Dial' in 'Contacts'
        return ;
    }
    
    static int rx_active; // flag to syncronice this hook ( operatingmode == 0x11 is also on rx seeded)
    static int fullscale_offset = 0;
    static uint32_t lastframe = 0;
    static int red = 0;
    static int green = 0;

    int relative_peak_cb;
    int centibel_val;

    if( fullscale_offset == 0 ) { // init int_centibel()
        fullscale_offset = intCentibel(3000); // maybe wav max max_level
    }
    
    int is_tx = 0 ;
    int is_rx = 0 ;

    is_tx = gui_opmode1 == SCR_MODE_RX_VOICE && max_level > 10 ;
    is_rx = gui_opmode1 == SCR_MODE_RX_TERMINATOR ;

#if defined(FW_D13_020) || defined(FW_S13_020)
    {
//        uint8_t *rs = (void*)0x2001e5f0 ;
        uint8_t s = radio_status_1.m1 ;
        
        is_tx = s & 1 ;
        is_rx = s & 2 ;
    }
#endif    

    if( is_tx && max_level < 4500 ) { 
        if( lastframe < ambe_encode_frame_cnt ) { // check for new frame
            lastframe = ambe_encode_frame_cnt;
            rx_active = 1;

            relative_peak_cb = intCentibel(max_level) - fullscale_offset;
            centibel_val = relative_peak_cb;


            if( lastframe % 5 == 1 ) { // reduce drawing
                if( centibel_val < -280 ) { // limit 160 pixel bargraph 10 150 -> 140 pixel for bargraph
                    centibel_val = -280;
                } else if( centibel_val > 0 ) {
                    centibel_val = 0;
                }
                centibel_val += 280; // shift to positive
                centibel_val /= 2; // scale

                gfx_set_fg_color(0x999999);
                gfx_set_bg_color(0xff000000);
                gfx_blockfill(9, 54, 151, 66);

                // paint legend
                gfx_set_fg_color(0x0000ff);
                gfx_blockfill((-30 + 280) / 2 + 10, 67, 150, 70);
                gfx_set_fg_color(0x00ff00);
                gfx_blockfill((-130 + 280) / 2 + 10, 67, (-30 + 280) / 2 - 1 + 10, 70);
                gfx_set_fg_color(0x555555);
                gfx_blockfill(10, 67, (-130 + 280) / 2 - 1 + 10, 70);

                // set color
                if( relative_peak_cb > -3 || red > 0 ) {
                    if( red > 0 ) red--;
                    if( relative_peak_cb > -3 ) red = 30;
                    gfx_set_fg_color(0x0000ff);
                } else if( relative_peak_cb > -130 || green > 0 ) {
                    if( green > 0 ) green--;
                    if( relative_peak_cb > -130 ) green = 30;
                    gfx_set_fg_color(0x00ff00);
                } else {
                    gfx_set_fg_color(0x555555);
                }
                gfx_set_bg_color(0xff000000);
                gfx_blockfill(10, 55, centibel_val, 65);
                gfx_set_fg_color(0xff8032);
                gfx_set_bg_color(0xff000000);
            }
        }
    }

    if( is_rx && rx_active == 1 ) { // clear screen area
        gfx_set_fg_color(0xff8032);
        gfx_set_bg_color(0xff000000);
        gfx_blockfill(9, 54, 151, 70);
        rx_active = 0;
        red = 0;
        green = 0;
    }
}

#define RX_POPUP_Y_START 22 // 24
#define RX_POPUP_X_START 4  // 10

void draw_rx_screen(unsigned int bg_color)
{
    int dst;
    int src;
    int grp;
    int nameLen;
    //char *timeSlot[3];
	int primask = OS_ENTER_CRITICAL(); // for form sake
   
    dst = rst_dst ;
    src = rst_src ;
    grp = rst_grp ;

    OS_EXIT_CRITICAL(primask);

    // clear screen
    gfx_set_fg_color(bg_color);
    gfx_blockfill(0, 16, MAX_X, MAX_Y); 

    gfx_set_bg_color(bg_color);
    gfx_set_fg_color(0x000000);
    gfx_select_font(gfx_font_small);

    user_t usr ;

    if( usr_find_by_dmrid(&usr,src) == 0 ) {
        usr.callsign = "ID unknown" ;
        usr.firstname = "" ;
        usr.name = "No entry in" ;
        usr.place = "your user.bin" ;
        usr.state = "see README.md" ;
        usr.country = "on Github" ;
    }
	
    gfx_select_font(gfx_font_small);
#if defined(FW_D13_020) || defined(FW_S13_020)
    channel_info_t *ci = &current_channel_info ;
   // int ts1 = ( ci->cc_slot_flags >> 2 ) & 0x1 ;
    int ts2 = ( ci->cc_slot_flags >> 3 ) & 0x1 ;
#else
   // int ts1 = 1;
    int ts2 = 0;
#endif	
    int y_index = RX_POPUP_Y_START;

    if( grp ) {
        gfx_printf_pos( RX_POPUP_X_START, y_index, "%d->TG %d %s", src, dst, ( ts2==1 ? "TS2" : "TS1")  );        
    } else {
        gfx_printf_pos( RX_POPUP_X_START, y_index, "%d->%d %s", src, dst, ( ts2==1 ? "TS2" : "TS1")  );
    }
    y_index += GFX_FONT_SMALL_HEIGHT ;

    gfx_select_font(gfx_font_norm); // switch to large font
    gfx_printf_pos2(RX_POPUP_X_START, y_index, 10, "%s %s", usr.callsign, usr.firstname );
    y_index += GFX_FONT_NORML_HEIGHT; 

    if ( global_addl_config.userscsv > 1 && talkerAlias.length > 0 ) {		// 2017-02-19 show Talker Alias depending on setup 0=CPS 1=DB 2=TA 3=TA & DB
      // TA or TA/DB mode
      if ( talkerAlias.length > 16 ) {  
	    gfx_select_font(gfx_font_small);
		gfx_printf_pos2(RX_POPUP_X_START, y_index,10, "%s", talkerAlias.text );
	    y_index += GFX_FONT_SMALL_HEIGHT;
	  }
	  else {
		  if (talkerAlias.length < 1) {
			gfx_printf_pos2(RX_POPUP_X_START, y_index, 10, "DMRID: %d", src );
	      } else {
		    gfx_puts_pos(RX_POPUP_X_START, y_index, talkerAlias.text );
			gfx_printf_pos2(RX_POPUP_X_START, y_index, 10, "%s", talkerAlias.text );
		  }
		  y_index += GFX_FONT_NORML_HEIGHT;
	  }
	  
    } 
	else {
      // user.bin or codeplug or talkerAlias length=0
      nameLen = strlen(usr.name);
      if (nameLen > 16) {  // print in smaller font
        gfx_select_font(gfx_font_small);
        gfx_puts_pos(RX_POPUP_X_START, y_index, usr.name );
        y_index += GFX_FONT_SMALL_HEIGHT ; // previous line was in small font
      }
      else {  // print in larger font if it will fit
        gfx_puts_pos(RX_POPUP_X_START, y_index, usr.name );
        y_index += GFX_FONT_NORML_HEIGHT;
      }
	}
	
	y_index+=3;
	if ( global_addl_config.userscsv > 1 ) {
      gfx_set_fg_color(0x00FF00);
	} else {
      gfx_set_fg_color(0x0000FF);
	}
    gfx_blockfill( 1 , y_index , 156, y_index);
    gfx_set_fg_color(0x000000);
    y_index+=2;
	
	gfx_select_font(gfx_font_small);

	switch( global_addl_config.userscsv ) {
	case 0:
		gfx_puts_pos(RX_POPUP_X_START, y_index, "Userinfo: CPS mode");
		y_index += GFX_FONT_SMALL_HEIGHT ;
        break;
	
    // not implemented. I don't want to waste space for this line in user.bin mode	
	//case 1 :
	//	gfx_puts_pos(RX_POPUP_X_START, y_index, "Userinfo: UserDB mode");
    //    break;

	case 2:
		if ( talkerAlias.length > 0 ) {
            gfx_puts_pos(RX_POPUP_X_START, y_index, "Userinfo: TalkerAlias");
			y_index += GFX_FONT_SMALL_HEIGHT ;
    //    } else {
    //        gfx_puts_pos(RX_POPUP_X_START, y_index, "Userinfo: TA not rcvd!");
	    }
        break;

	// not implemented due to same reason above
	//case 3:
	//	gfx_puts_pos(RX_POPUP_X_START, y_index, "Userinfo: TA/DB mode");
    //    break;
	}
	switch( global_addl_config.userscsv ) {
	case 1:
	case 3:
	
	  if( src != 0 ) { 
         gfx_select_font(gfx_font_small);
         gfx_puts_pos(RX_POPUP_X_START, y_index, usr.place );
         y_index += GFX_FONT_SMALL_HEIGHT ;

         gfx_puts_pos(RX_POPUP_X_START, y_index, usr.state );
         y_index += GFX_FONT_SMALL_HEIGHT ;

         gfx_puts_pos(RX_POPUP_X_START, y_index, usr.country );
         y_index += GFX_FONT_SMALL_HEIGHT ;
	  }
	}
	
    gfx_select_font(gfx_font_norm);
    gfx_set_fg_color(0xff8032);
    gfx_set_bg_color(0xff000000);
}

void draw_ta_screen(unsigned int bg_color)
{
    int dst;
    int src;
    int grp ;
    
    int primask = OS_ENTER_CRITICAL(); // for form sake
    
    dst = rst_dst ;
    src = rst_src ;
    grp = rst_grp ;
    
    OS_EXIT_CRITICAL(primask);

    // clear screen
    gfx_set_fg_color(bg_color);
    gfx_blockfill(0, 16, MAX_X, MAX_Y); 

    gfx_set_bg_color(bg_color);
    gfx_set_fg_color(0x000000);
    gfx_select_font(gfx_font_small);

    user_t usr ;
    
    int y_index = RX_POPUP_Y_START;
    
    gfx_select_font(gfx_font_small);
    if( grp ) {
        gfx_printf_pos( RX_POPUP_X_START, y_index, "%d -> TG %d", src, dst );        
    } else {
        gfx_printf_pos( RX_POPUP_X_START, y_index, "%d -> %d", src, dst );
    }
    y_index += GFX_FONT_SMALL_HEIGHT ;

    if ( global_addl_config.userscsv > 1 && talkerAlias.length > 0 )		// 2017-02-19 show Talker Alias depending on setup 0=CPS 1=DB 2=TA 3=TA & DB
    {    
	gfx_select_font(gfx_font_norm);
	gfx_printf_pos2(RX_POPUP_X_START, y_index, 10, "%s", talkerAlias.text );
    }

    y_index += GFX_FONT_NORML_HEIGHT; // previous line was in big font
    
    gfx_select_font(gfx_font_small);

    if( usr_find_by_dmrid(&usr,src) == 0 )
    {
	gfx_puts_pos(RX_POPUP_X_START, y_index, "No userdb info");
    } else {
        gfx_printf_pos(RX_POPUP_X_START, y_index, "%s %s", usr.callsign, usr.firstname );
    }
    y_index += GFX_FONT_SMALL_HEIGHT ; // previous line was in small font

    y_index += GFX_FONT_SMALL_HEIGHT ;
    y_index += GFX_FONT_SMALL_HEIGHT ;
    
    gfx_puts_pos(RX_POPUP_X_START, y_index, usr.country );
    y_index += GFX_FONT_SMALL_HEIGHT ;
    
    gfx_select_font(gfx_font_norm);
    gfx_set_fg_color(0xff8032);
    gfx_set_bg_color(0xff000000);
}

/*
#include <stdlib.h>

int main(void)
{
    long peak = 6000;
    if (peak > 0) {
	int fullscale_offset = int_centibel(32767);
	int relative_peak_cb = int_centibel(peak) - fullscale_offset;
	printf("%i.%i dBFS\en", relative_peak_cb / 10,
	       abs(relative_peak_cb % 10));
    } else {
	printf("-Inf dBFS\n");
    }
    return 0;
}
*/


void draw_statusline_hook( uint32_t r0 )
{
    if( is_netmon_visible() ) {
        con_redraw();
        return ;
    }
    draw_statusline( r0 );
}

void draw_alt_statusline()
{
    //int dst;
    int src;
    //int grp;

    gfx_set_fg_color(0);
    gfx_set_bg_color(0xff8032);
    gfx_select_font(gfx_font_small);

    char mode = ' ' ;
    if( rst_voice_active ) {
        if( rst_mycall ) {
            mode = '*' ; // on my tg            
        } else {
            mode = '!' ; // on other tg
        }
    }

    user_t usr;
    src = rst_src;
    
    if( src == 0 ) {
	if ( global_addl_config.datef == 5 )
	{
	        gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "lh:");
	} else {
	        gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "TA:");
	}
    } else {
	if ( global_addl_config.datef == 6 && talkerAlias.length > 0 )				// 2017-02-18 show talker alias in status if rcvd valid
	{
		gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "TA: %s", talkerAlias.text);
	} else {										// 2017-02-18 otherwise show lastheard in status line
	        if( usr_find_by_dmrid(&usr, src) == 0 ) {
        	    gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "LH:%d->%d %c", src, rst_dst, mode);
        	} else {
        	    gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "LH:%s->%d %c", usr.callsign, rst_dst, mode);
	        }	
	}
    }
    
    gfx_set_fg_color(0);
    gfx_set_bg_color(0xff000000);
    gfx_select_font(gfx_font_norm);
}

void draw_datetime_row_hook()
{
#if defined(FW_D13_020) || defined(FW_S13_020)
    if( is_netmon_visible() ) {
        return ;
    }
    if( is_statusline_visible() || global_addl_config.datef == 6 ) {
        draw_alt_statusline();
        return ; 
    }
    draw_datetime_row();
#else
#warning please consider hooking.    
#endif    
}
/* Displays a startup demo on the device's screen, including some of
   the setting information and a picture or two. */
void display_credits()
{
    drawtext(L"MD380Tools ", 160, 20);
    drawtext(L"by KK4VCZ  ", 160, 60);
    drawtext(L"and Friends", 160, 100);
#ifdef MD380_d13_020
    drawtext(L"D13.020 KD4Z", 160, 140);
#endif
#ifdef MD380_d02_032
    drawtext(L"D02.032 KD4Z", 160, 140);
#endif
#ifdef MD380_s13_020
    drawtext(L"S13.020 KD4Z", 160, 140);
#endif
    drawascii(GIT_VERSION, 160, 180);
    drawtext(VERSIONDATE, 160, 220);
}
