/*
 *  display.c
 * 
 * for high-level drawing functions.
 * Customizations:  KD4Z, NO7K
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
#include "app_menu.h"
#include "syslog.h"        // LOGB()
#include "irq_handlers.h"  // boot_flags, BOOT_FLAG_DREW_STATUSLINE
#include "lcd_driver.h"
#include "codeplug.h"
#if defined(FW_D13_020) || defined(FW_S13_020)
  #include "amenu_set_tg.h"
#else
#warning old firmware
#endif
//#include "amenu_channels.h"
#include <stdlib.h>

#define RX_POPUP_Y_START 17 // prev=22 // orig=24
#define RX_POPUP_X_START 4  // 10
#define FIRSTNAME_BUFSIZE 30
#define COUNTRY_BUFSIZE 20
#define STATE_BUFSIZE 20

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

static const char *const countries[] = {
	"AD,Andorra",
	"AR,Argentina",
	"AT,Austria",
	"AU,Australia",
	"BA,Bosnia and Hercegovi",
	"BB,Barbados",
	"BE,Belgium",
	"BG,Bulgaria",
	"BR,Brazil",
	"BZ,Belize",
	"CA,Canada",
	"CH,Switzerland",
	"CL,Chile",
	"CN,China",
	"CO,Columbia",
	"CR,Costa Rica"
	"CY,Cyprus",
	"CZ,Czech Republic",
	"DE,Germany",
	"DK,Denmark",
	"DM,Dominica",
	"DO,Dominican Republic",
	"EC,Ecuador",
	"EE,Estonia",
	"ES,Spain",
	"FI,Finland",
	"FR,France",
	"GR,Greece",
	"GT,Guatemala",
	"HK,Hong Kong",
	"HR,Croatia",
	"HT,Haiti",
	"HU,Hungary",
	"ID,Indonesia",
	"IE,Ireland",
	"IL,Israel",
	"IN,India",
	"IT,Italy",
	"JP,Japan",
	"KR,Korea",
	"KW,Kuwait",
	"LI,Liechtenstein",
	"LU,Luxemburg",
	"LV,Latvia",
	"ME,Montenegro",
	"MK,Macedonia",
	"MT,Malta",
	"MX,Mexico",
	"MY,Malaysia",
	"NL,Netherlands",
	"NO,Norway",
	"NZ,New Zealand",
	"PA,Pananma",
	"PH,Philippines",
	"PL,Poland",
	"PT,Portugal",
	"QA,Qatar",
	"RE,Reunion",
	"RO,Romainia",
	"RS,Serbia",
	"RU,Russia",
	"SE,Sweden",
	"SG,Singapore",
	"SI,Slovenia",
	"SK,Slovakia",
	"TH,Thailand",
	"TR,Turkey",
	"TT,Trinidad and Tobago",
	"TW,Taiwan",
	"UA,Ukraine",
	"UK,UK",
	"US,USA",
	"UY,Uruguay",
	"VE,Venezuela",
	"ZA,South Africa",
};

static const char *const states[] = {
	"AB,Alberta",
	"ACT,Austrailian Capital T.",
	"AK,Alaska",
	"AL,Alabama",
	"AN,Antwerpen",
	"AR,Arkansas",
	"AZ,Arizona",
	"BB,Brandenburg",
	"BC,British Columbia",
	"BE,Berlin",
	"BW,Baden-Wurttemberg",
	"BY,Bavaria",
	"CA,California",
	"CO,Colorada",
	"CT,Connecticut",
	"DC,District of Columbia",
	"DE,Delaware",
	"DR,Drenthe",
	"FD,Flevoland",
	"FL,Florida",
	"FR,Friesland",
	"GA,Georgia",
	"GE,Gelderland",
	"GR,Groningen",
	"HB,Bremen",
	"HE,Hessen",
	"HH,Hamburg",
	"HI,Hawaii",
	"IA,Iowa",
	"ID,Idaho",
	"IF,Ile-de-France",
	"IL,Illinois",
	"IN,Indiana",
	"KS,Kansas",
	"KY,Kentucky",
	"LA,Louisiana",
	"LI,Limburg",
	"MA,Massachusetts",
	"MB,Manitoba",
	"MD,Maryland",
	"ME,Maine",
	"MI,Michigan",
	"MN,Minnesota",
	"MO,Missouri",
	"MS,Mississippi",
	"MT,Montana",
	"MV,Mecklenburg-Vorpommern",
	"N-B,North Brabant",
	"N-H,North Holland",
	"NB,New Brunswick",
	"NC,North Carolina",
	"ND,North Dakota",
	"NE,Nebraska",
	"NH,New Hampshire",
	"NI,Lower Saxony",
	"NJ,New Jersey",
	"NL,Newfoundland",
	"NM,New Mexico",
	"NS,Nova Scotia",
	"NSW,New South Wales",
	"NT,Northern Territory",
	"NV,Nevada",
	"NW,North Rhine-Westphalia",
	"NY,New York",
	"O-V,Oost-Vlaanderen",
	"OA,All Others",
	"OH,Ohio",
	"OK,Oklahoma",
	"ON,Ontario",
	"OR,Oregon",
	"OV,Overijssel",
	"PA,Pennsylvania",
	"PE,Prince Edward Is.",
	"PR,Puerto Rico",
	"QC,Quebec",
	"QLD,Queensland",
	"RI,Rhode Island",
	"RP,Rhineland-Palatinate",
	"SA,South Australia",
	"SC,South Carolina",
	"SD,South Dakota",
	"SH,Schleswig-Holstein",
	"SK,Saskatchewan",
	"SL,Saarland",
	"SN,Saxony",
	"ST,Saxony-Anhalt",
	"TAS,Tasmania",
	"TH,Thuringia",
	"TN,Tennessee",
	"TX,Texas",
	"UT,Utah",
	"UTR,Utrecht",
	"VA,Virginia",
	"VAN,Antwerp",
	"VB,Vlaams-Brabant",
	"VBR,Flemish Brabant",
	"VIC,Victoria",
	"VLI,Limburg",
	"VOV,East Flanders",
	"VT,Vermont",
	"VWV,West Flanders",
	"WA,Washington",
	"WAU,Western Australia",
	"WBR,Walloon Brabant",
	"WHT,Hainaut",
	"WI,Wisconsin",
	"WLG,Leige",
	"WLX,Luxembourg",
	"WNA,Namur",
	"WV,West Virginia",
	"WY,Wyoming",
	"YT,Yukon",
	"ZE,Zeeland",
	"ZH,South Holland",
};

#define ARRAY_SIZE(x) (sizeof x / sizeof x[0])

int abbrevs_sorted(const char *const a[], int size) {
	int hasSortIssue = 1;
	for (int i = 1; i < size; i++)
		if (strcmp(a[i-1], a[i]) >= 0) {
			hasSortIssue = 0;
			//printf("Sort problem, these need to be swapped: %s and %s \n",a[i-1],a[i]);
			return hasSortIssue;
			
		}
	return hasSortIssue;
}

const char *lookupAbbrev(char *abbrev, const char *const abbrevs[], int length) {
	int left = 0;
	int right = length - 1;
	static int sorted;
	static int order_checked;

	if (!sorted) {
		if (!order_checked) {
			sorted = abbrevs_sorted(countries, ARRAY_SIZE(countries));
			sorted &= abbrevs_sorted(states, ARRAY_SIZE(states));
			order_checked = 1;
		}
		if (!sorted)
			return "unsorted";
	}

	while (left <= right) {
		int middle = left + (right-left)/2;
		const char *p, *q;
		for (p=abbrev, q=abbrevs[middle]; *q != 0; p++, q++) {
			if (*p < *q) {
				if (*p == 0 && *q == ',')
					return q + 1;
				right = middle - 1;
				break;
			}
			if (*p > *q) {
				left = middle + 1;
				break;
			}
		}
	}

	return abbrev;
}


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

//#d efine RX_POPUP_Y_START 24
//#d efine RX_POPUP_X_START 10

void draw_txt(char* testStr, int x, int y, char font){
	char c=0;
	int maxLen=16;
	uint16_t fg_color = 0, bg_color = 0;
#if defined(FW_D13_020) || defined(FW_S13_020)
	Menu_GetColours(SEL_FLAG_NONE, &fg_color, &bg_color);
#endif
	while( ((c=*testStr)!=0)  && maxLen>0)
	{ x = LCD_DrawCharAt( c, x, y, fg_color, bg_color, font);
		//++i; // character index and limiting counter
	    ++testStr; 
		// (in rare cases, some of the leading text may be OVERWRITTEN below)
		maxLen--;
	}		 
}

char *get_firstname(user_t *up, char *buf, int buflen) {
// Thanks to Dale Farnsworth dale@farnsworth.org for providing this little ditty.
    if (*up->firstname != 0)
	return up->firstname;

    char *p = buf;
    char *q = up->name;
    for (int i = 0; i < buflen-1 && *q != ' ' && *q != 0; i++)
	*p++ = *q++;

    *p = 0;

    return buf;
}

#define ARRAY_SIZE(x) (sizeof x / sizeof x[0])

char *lookup_country(user_t *up, char *buf) {
	const char *p = lookupAbbrev(up->country, countries, ARRAY_SIZE(countries));
	strcpy(buf, p);
	return buf;
}

char *lookup_state(user_t *up, char *buf) {
	const char *p = lookupAbbrev(up->state, states, ARRAY_SIZE(states));
	strcpy(buf, p);
	return buf;
}



#if defined(FW_D13_020) || defined(FW_S13_020)
#define __PTT_LASTHEARD
#define __PTT_LASTHEARD_DOWN
#endif

#if defined(__PTT_LASTHEARD)
static int lh_painted = 0;
static uint32_t stopwatch_cnt = 0;

void draw_tx_screen_layout() 
{
#if defined(FW_D13_020) || defined(FW_S13_020)
	lcd_context_t dc;
	int sel_flags = SEL_FLAG_NONE;
	int src;
	int dst;
	user_t usr;
    src = rst_src;
	dst = rst_dst ;
    char firstname_buf[FIRSTNAME_BUFSIZE];
    char *firstname;
	char state_buf[STATE_BUFSIZE];
	int ptt_milliseconds = 0;
	int ch_to = 0;
	int secs_display = 0;
	
	channel_info_t *ci = &current_channel_info;
  	ch_to = ci->unk8==0 ? 999 : ci->unk8 * 15;
		
	Menu_GetColours( sel_flags, &dc.fg_color, &dc.bg_color );
 
	dc.x = 17;
	dc.y = 20;
	// font options
	//dc.font = LCD_OPT_FONT_6x12;
	//dc.font = LCD_OPT_FONT_8x8;
	//dc.font = LCD_OPT_FONT_8x16;
	//dc.font = LCD_OPT_FONT_12x24;

	// fill the screen with the background color
	if (lh_painted == 0) {
		LCD_FillRect( 0, 15, LCD_SCREEN_WIDTH-1, LCD_SCREEN_HEIGHT-1, dc.bg_color );
		lh_painted = 1;
	}
	ptt_milliseconds = ReadStopwatch_ms(&stopwatch_cnt);


#if defined(__PTT_LASTHEARD_DOWN)
		secs_display = ch_to - (ptt_milliseconds/1000);
#else
		secs_display = ptt_milliseconds/1000;
#endif
	
	if ( dst > 0 ) {
		dc.font = LCD_OPT_FONT_8x8;
		LCD_Printf( &dc, " TG: %d \r",dst);
	} else {
		dc.font = LCD_OPT_FONT_12x24;
		LCD_Printf( &dc, "     PTT\r");
		LCD_Printf( &dc, "     %d\r",secs_display);
#if defined(__PTT_LASTHEARD_DOWN)
		LCD_Printf( &dc, " SECS REMAIN\r");	
#else
		LCD_Printf( &dc, "   SECONDS\r");
#endif
	}
 	if( usr_find_by_dmrid(&usr, src) > 0 ) {
	
			firstname = get_firstname(&usr, firstname_buf, FIRSTNAME_BUFSIZE);
			dc.font = LCD_OPT_FONT_12x24;
			LCD_Printf( &dc, "%s %d\r", usr.callsign,secs_display); 
			dc.y =  dc.y - 2;
			LCD_Printf( &dc, " %s\r", firstname); 
			dc.font = LCD_OPT_FONT_8x16;
			LCD_Printf( &dc, "%s\r", usr.place); 
			LCD_Printf( &dc, "%s\r", lookup_state(&usr, state_buf));
			LCD_Printf( &dc, "%s\r", lookup_country(&usr, state_buf));
	}
#endif
}
void draw_micbargraph()
{
	//
	// New bargraph routine extended with Last Heard information de KD4Z
	//
 
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
        uint8_t s = radio_status_1.m1 ;
        is_tx = s & 1 ;
        is_rx = s & 2 ;
    }
#endif    
	//if( is_tx && tx_active == 0 ) {
	//	tx_active = 1;
	//	//gfx_blockfill(0, 0, 159, 127);  // clear whole block area of screen
	//}
	
    if( is_tx && max_level < 4500 ) { 
		if (stopwatch_cnt==0)
			StartStopwatch(&stopwatch_cnt);
		
	
        if( lastframe < ambe_encode_frame_cnt ) { // check for new frame
            lastframe = ambe_encode_frame_cnt;
            rx_active = 1;

            relative_peak_cb = intCentibel(max_level) - fullscale_offset;
            centibel_val = relative_peak_cb;

            if( lastframe % 5 == 1 ) { // reduce drawing
 
				if( centibel_val < -240 ) { // limit 120 pixel bargraph 5 125 -> 120 pixel for virtical bargraph
                    centibel_val = -240;
                } else if( centibel_val > 0 ) {
                    centibel_val = 0;
                }
                centibel_val += 240; // shift to positive
                centibel_val /= 2; // scale
				
				 
				draw_tx_screen_layout();
				lh_painted = 1;
				
                gfx_set_bg_color(0x000000);  //black		// light grey = 0xff000000
				//if( centibel_val > 0 ) {
				gfx_set_fg_color(0x000000);			//gfx_set_fg_color(0x00FF00)  high green  0x0000FF Red   BBGGRR
				gfx_blockfill(7, 15, 14, 127);      // clear area of bar before repainting again
				//}
	            // paint legend
  
 				// 3-6 legend 7-14 bar  
 				gfx_set_fg_color(0x0000ff);  // red
                gfx_blockfill(3, 15, 6, 30);                           // 135,67,150,70
                gfx_set_fg_color(0x00ff00);  // green
                gfx_blockfill(3, 31, 6, 70);     // 85,67,134,70
                gfx_set_fg_color(0x555555);  // grey
                gfx_blockfill(3, 71, 6,127);						// 10,67,84,70
				
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
               	// paint the VU bar	
				if (centibel_val > 126)
					centibel_val = 126;
				
                gfx_blockfill(7, (139 - centibel_val), 14, 120);			// left margin vertical
	  			
            }
        }
    }

    if( is_rx && rx_active == 1 ) { // clear screen area
        //gfx_set_fg_color(0xff8032);
        //gfx_set_bg_color(0xff000000);
        //gfx_blockfill(3, 15, 159, 127);
        rx_active = 0;
        red = 0;
        green = 0;
		lh_painted = 0;
		stopwatch_cnt = 0;
		//draw_rx_screen(0x888888);
		LCD_FillRect( 0,0, LCD_SCREEN_WIDTH-1, LCD_SCREEN_HEIGHT-1, LCD_COLOR_MD380_BKGND_BLUE );
#if defined(FW_D13_020) || defined(FW_S13_020)
		channel_num = 0;
#endif		
	}  // end of clear screen block
	
}


#else
void draw_micbargraph()
{
	//
	// original mic bargraph routine
	//
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
        uint8_t s = radio_status_1.m1 ;
        is_tx = s & 1 ;
        is_rx = s & 2 ;
		channel_info_t *ci = &current_channel_info ; 
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


#endif 





void draw_rx_screen(unsigned int bg_color)
{
	#define FULLNAME_MAX_LARGEFONT_CHARS 16
	#define CITY_MAX_LARGEFONT_CHARS 14
	#define STATECOUNTRY_MAX_LARGEFONT_CHARS 14
	
    static int dst;
    int src;
    int grp;
   
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
    char firstname_buf[FIRSTNAME_BUFSIZE];
	char country_buf[COUNTRY_BUFSIZE];
	char state_buf[STATE_BUFSIZE];
	
    if( usr_find_by_dmrid(&usr,src) == 0 ) {
		if( src==4000 ) {
			usr.callsign = "Message" ;
            usr.firstname = "from" ;
            usr.name = "Server" ;
            usr.place = "" ;
            usr.state = "" ;
            usr.country = "";
		}
		else {
            usr.callsign = "ID" ;
            usr.firstname = "not found" ;
            usr.name = "in user.bin." ;
            usr.place = "Update with" ;
            usr.state = "glvusers," ;
            usr.country = "then flashdb";
		}
    }
	
    gfx_select_font(gfx_font_small);
#if defined(FW_D13_020) || defined(FW_S13_020)
    channel_info_t *ci = &current_channel_info ;
    int ts2 = ( ci->cc_slot_flags >> 3 ) & 0x1 ;
	int cc = ( ci->cc_slot_flags >> 4 ) & 0xf ;
  
#else
   // int ts1 = 1;
    int ts2 = 0;
	int cc = 0;
#endif	
    int y_index = RX_POPUP_Y_START;

	if (global_addl_config.mode_color == 1) { gfx_set_fg_color(0xffffff); gfx_set_bg_color(0xff4f32);}
    if( grp ) {
        gfx_printf_pos( RX_POPUP_X_START, y_index, "%d-TG %d %s CC:%d", src, dst, ( ts2==1 ? "T2" : "T1"),cc );        
    } else {
        gfx_printf_pos( RX_POPUP_X_START, y_index, "%d-%d %s CC:%d", src, dst, ( ts2==1 ? "T2" : "T1"),cc );
    }
	gfx_set_fg_color(0x000000); gfx_set_bg_color(bg_color);
    y_index += GFX_FONT_SMALL_HEIGHT ;
	y_index--;
	gfx_select_font(gfx_font_norm); // switch to large font
	
	int nameLen = strlen(usr.name);
	
	if (strlen(usr.firstname) > 0)  {  // have real nickname, display it as before
		gfx_printf_pos2(RX_POPUP_X_START, y_index, 10, "%s-%s", usr.callsign, usr.firstname );
	} else {
		char *firstname = get_firstname(&usr, firstname_buf, FIRSTNAME_BUFSIZE);
		if (strcmp(usr.firstname, firstname) != 0  && strlen(usr.firstname)>0) {
			// do this if nickname is different than firstname 
			gfx_printf_pos2(RX_POPUP_X_START, y_index, 10, "%s-%s", usr.callsign, firstname );
			//gfx_printf_pos2(RX_POPUP_X_START, y_index, 10, "%d %d", strlen(usr.firstname), strlen(firstname) );
		} else if (nameLen > FULLNAME_MAX_LARGEFONT_CHARS) {
			// or fullname is going to be in small font
			gfx_printf_pos2(RX_POPUP_X_START, y_index, 10, "%s-%s", usr.callsign, firstname );
			
		} else { 
			// do this if fullname will be in large font, no need to display firstname
			gfx_printf_pos2(RX_POPUP_X_START, y_index, 10, "%s", usr.callsign);
		}
	}
    y_index += GFX_FONT_NORML_HEIGHT; 

    if ( global_addl_config.userscsv > 1 && talkerAlias.length > 0 ) {		// 2017-02-19 show Talker Alias depending on setup 0=CPS 1=DB 2=TA 3=TA & DB
      // TA or TA/DB mode
      if ( talkerAlias.length > FULLNAME_MAX_LARGEFONT_CHARS ) {  
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
      y_index++;
      if (nameLen > FULLNAME_MAX_LARGEFONT_CHARS) {  // print in smaller font
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
      gfx_set_fg_color(0x005500);
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

	case 2:
		if ( talkerAlias.length > 0 ) {
            gfx_puts_pos(RX_POPUP_X_START, y_index, "Userinfo: TalkerAlias");
			y_index += GFX_FONT_SMALL_HEIGHT ;
	    }
        break;
	}
	switch( global_addl_config.userscsv ) {
	case 1:
	case 3:
	
		if( src != 0 ) { 
			// city
			if ( strlen(usr.place) > CITY_MAX_LARGEFONT_CHARS) { 
				gfx_select_font(gfx_font_small);
				gfx_puts_pos(RX_POPUP_X_START, y_index, usr.place );  
				y_index += GFX_FONT_SMALL_HEIGHT ;
			} else {
				gfx_select_font(gfx_font_norm);
				gfx_puts_pos(RX_POPUP_X_START, y_index, usr.place );  
				y_index += GFX_FONT_NORML_HEIGHT;
			}
			// state/province and country
			// something in oem firmware is blocking end of line, so we lose a few chars at end and bottom of screen
			char *state = lookup_state(&usr, state_buf);
			char *country = lookup_country(&usr, country_buf);
			if ( (strlen(state) + strlen(country)) > STATECOUNTRY_MAX_LARGEFONT_CHARS) {  
				gfx_select_font(gfx_font_small);
				gfx_puts_pos(RX_POPUP_X_START, y_index, state );
				y_index += GFX_FONT_SMALL_HEIGHT ;
				gfx_puts_pos(RX_POPUP_X_START, y_index, country );		
			} else {
				gfx_select_font(gfx_font_norm);
				gfx_printf_pos2(RX_POPUP_X_START, y_index, 10, "%s %s", state, country );			
			}
          
		}
	  
	}
	
    gfx_select_font(gfx_font_norm);
    gfx_set_fg_color(0xff8032);
    gfx_set_bg_color(0xff0000);
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
    y_index += GFX_FONT_SMALL_HEIGHT ;  // 12
    
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
   if( ! (boot_flags & BOOT_FLAG_DREW_STATUSLINE) )
    { LOGB("t=%d: draw_stat\n", (int)IRQ_dwSysTickCounter ); // 4383(!) SysTicks after power-on
    }
   boot_flags |= BOOT_FLAG_DREW_STATUSLINE; 

# if (CONFIG_APP_MENU)
    // If the screen is occupied by the optional 'red button menu', 
    // update or even redraw it completely:
    if( Menu_DrawIfVisible(AM_CALLER_STATUSLINE_HOOK) )  
     { return; // the menu covers the entire screen, so don't draw anything else
     }
    // NOTE: draw_statusline_hook() isn't called when the squelch
    //       is 'open' in FM, i.e. when the channel is BUSY .
    // Of course we don't want to be tyrannized by the radio like that.
    // It's THE OPERATOR'S decision what to do and when to invoke the menu,
    // not the radio's. 
    // Fixed by also calling Menu_DrawIfVisible() from other places .
# endif // CONFIG_APP_MENU ?

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
    user_t dst;
    src = rst_src;
    char firstname_buf[FIRSTNAME_BUFSIZE];
    char *firstname;
    if( src == 0 ) {
		if ( global_addl_config.datef == 5 )
		{
	        gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "LH:");
		} else {
	        gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "TA:");
		}
    } else {
		if ( global_addl_config.datef == 6 && talkerAlias.length > 0 )				// 2017-02-18 show talker alias in status if rcvd valid
		{
			gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "TA: %s", talkerAlias.text);
		} else {										// 2017-02-18 otherwise show lastheard in status line
			if( usr_find_by_dmrid(&usr, src) == 0 ) {
					if( usr_find_by_dmrid(&usr, rst_dst) != 0 ) {
						firstname = get_firstname(&usr, firstname_buf, FIRSTNAME_BUFSIZE);
						gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "LH:%d>%s %s %c", src, usr.callsign, firstname, mode);
					} else  {
						gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "LH:%d>%d %c", src, rst_dst, mode);
					}
			} else {
	                if( usr_find_by_dmrid(&dst, rst_dst) != 0 ) {
						firstname = get_firstname(&dst, firstname_buf, FIRSTNAME_BUFSIZE);
						gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "LH:%s %s>%s %c", usr.callsign, firstname, dst.callsign, mode);
					} else  {
						firstname = get_firstname(&usr, firstname_buf, FIRSTNAME_BUFSIZE);
						gfx_printf_pos2(RX_POPUP_X_START, 96, 157, "LH:%s %s>%d %c", usr.callsign, firstname, rst_dst, mode);
					}
			}
		}
    }

    gfx_set_fg_color(0);
    gfx_set_bg_color(0xff000000);
    gfx_select_font(gfx_font_norm);
}

#if defined(FW_D13_020) || defined(FW_S13_020)
void draw_adhoc_statusline()
{
//	int x = RX_POPUP_X_START + 36;							// 36=standard position aligned with channel info
	int x = RX_POPUP_X_START + 35;
//	int y = 55;									// 55=standard position from top
	int y = 53;
	int top_y = 17;									// upper status below fw statusline

	gfx_set_fg_color(0x000000);
	gfx_set_bg_color(0xff8032);
	gfx_select_font(gfx_font_small);

	char top_status[25];								// top status line
	char bot_status[25];								// bottom status line

	char ch_rx[10];
	char ch_tx[10];
	char freq_rx[10];
	char freq_tx[10];

	//char ch_mode[3];								// DMR / FM / FM-N / FM-W
	//char ch_wide[2];								// DMR / FM / FM-N / FM-W
	//char ch_rpt[4];									// [-R] / [+R] repeater shift
	char dmr_cc[2];									// [CC1] color code
	//char dmr_compact[5];								// [1|2| ... CC/TS prefix
	char ch_offset[4];								// repeater offset
	//char ch_tmp[10];								// temp
	char ch_cc[1];									// temp CC

	char fm_bw_stat[2];								// |N or |W
	char mic_gain_stat[3];								// off, 3dB, 6dB
	char fm_sql[3];									// CTS oder DCS
	char tg_fill[7];								// talkgroup space filler

	char ch_tone_type[2];								// N=none D=DCS 0-9=CTS
	long ch_rxfreq = 0;
	long ch_txfreq = 0;
	//float ch_freqoff = 0;
     
	
	strncpy(ch_rx, current_channel_info_E.rxFreq.text, 12);				// read RX frequency from codeplug
	strncpy(ch_tx, current_channel_info_E.txFreq.text, 12);				// read TX frequency from codeplug

	strncpy(freq_rx, current_channel_info_E.rxFreq.text, 12);				// read RX frequency from codeplug
	strncpy(freq_tx, current_channel_info_E.txFreq.text, 12);				// read TX frequency from codeplug

	//strcat(ch_rx, '\0');
	//strcat(ch_tx, '\0');
	//strcat(freq_rx, '\0');
	//strcat(freq_tx, '\0');
	freq_rx[8] = '\0';
	freq_tx[8] = '\0';
	
	strncpy(ch_tone_type, current_channel_info_E.EncTone.text, 1);
	ch_tone_type[1] = '\0';

	ch_rxfreq = atol(ch_rx);
	ch_txfreq = atol(ch_tx);
	//sprintf(ch_rxfreq, ch_rx);
	//sprintf(ch_txfreq, ch_tx);

	//ch_freqoff = ((ch_rxfreq - ch_txfreq) / 100000);
	BOOL fIsSimplex = (ch_rxfreq == ch_txfreq);
	user_t usr;									// reference user DB

	//========================================================================================================================//
	// First build general mode independent status info 				// RPT shift and Mic gain
	//========================================================================================================================//

	if (strcmp(ch_rx, ch_tx) == 0) {
		if (global_addl_config.mode_stat != 3) {				// if MODE/CC compact display set in config
			strcpy(ch_offset, "|S  ");
		} else {
			strcpy(ch_offset, "|S ");
		}
	} else if (strcmp(ch_rx, ch_tx) > 0) {
		if (global_addl_config.mode_stat != 3) {				// if MODE/CC compact display set in config
			strcpy(ch_offset, "|-R|");
		} else {
			strcpy(ch_offset, "|-R");
		}
	} else {
		if (global_addl_config.mode_stat != 3) {				// if MODE/CC compact display set in config
			strcpy(ch_offset, "|+R|");
		} else {
			strcpy(ch_offset, "|+R");
		}
	}

	if (global_addl_config.mode_stat > 1) {						// if MODE/CC/gain display set in config for both modes (DMR/FM)
		if (global_addl_config.mic_gain == 0) {
			strcpy(mic_gain_stat, "|0dB");
		} else if (global_addl_config.mic_gain == 1) {
			strcpy(mic_gain_stat, "|3dB");
		} else if (global_addl_config.mic_gain == 2) {
			strcpy(mic_gain_stat, "|6dB");
		}
	} else {
		strcpy(mic_gain_stat, "    ");						// blank if now mic gain display status selected
        }

//	BOOL fIsAnalog = current_channel_info_E.bIsAnalog;
	BOOL fIsDigital = current_channel_info_E.bIsDigital;
	BOOL fIsCTSvalid = (strlen(current_channel_info_E.EncTone.text) <= 6 );
	
	// the top statusline is build by the following strings:
	// -----------------------------------------------------
	//      |         DMR  |-R|
	//      |         DMR |-R[1|2|2623445]  --- [n|n|   = 5 DMR compact mode
	//      |         DMR  |-R| [CC1]
	//      |         FM |N|-R| [CTS]
	//      |         FM |N|-R| [DCS]
	//                 !  !  !    !
	//                 !  !  !    ! 
	//                 !  !  !    +------------- [CCn]    = 5
	//                 !  !  +------------------ |-R|     = 4
	//                 !  +--------------------- |N or |W = 2
	//                 +------------------------ Mode     = 3

	//========================================================================================================================//
	if (!fIsDigital) {								// DMR channel active
	//========================================================================================================================//
		int ch_cc = current_channel_info_E.CC;					// current color code
		int ch_ts = current_channel_info_E.Slot;				// current timeslot
		int tgNum = (ad_hoc_tg_channel ? ad_hoc_talkgroup : (((int)contact.id_h<<16) | ((int)contact.id_m<<8) | (int)contact.id_l));	// current talkgroup
		int callType = (ad_hoc_tg_channel ? ad_hoc_call_type : contact.type);	// current calltype
		sprintf(dmr_cc, "%s", ch_cc);  // or sprintf(dmr_cc, (char*)ch_cc);

		// build the top statusline -------------------------------------------------------------------
		if (global_addl_config.mode_stat != 3) {				// if MODE/CC compact display set in config
		strcpy(top_status, "DMR ");						// init DMR string
		} else {
		strcpy(top_status, "DMR");						// init DMR string compact
		}

		strcat(top_status, ch_offset);						// DMR + repeaterstatus

		if (global_addl_config.mode_stat != 3) {				// if MODE/CC compact display set in config
			strcat(top_status, " |CC");					// DMR [-R] [CCn]
		} else {
			strcat(top_status, "");					// DMR [-R] [CCn] in compact mode
		}

		// build some spaces between [CC|TS|TG] and db-Status -----------------------------------------
		if (tgNum > 999999) {
			strcpy(tg_fill, "");
		} else if (tgNum > 99999) {
			strcpy(tg_fill, "");
		} else if (tgNum > 9999) {
			strcpy(tg_fill, "");
		} else if (tgNum > 999) {
			strcpy(tg_fill, "");
		} else if (tgNum > 99) {
			strcpy(tg_fill, "");
		} else if (tgNum > 9) {
			strcpy(tg_fill, " ");
		} else {
			strcpy(tg_fill, "  ");
		}

		// ... the remaining info about DCS/CTS/CC is build dynamically during output

		// build the bottom statusline ----------------------------------------------------------------
		strcpy(bot_status, "TS:");						// init bottom string
		// ... the remaining info about TG/adhoc TG/private ID is build dynamically during output


		if (global_addl_config.mode_stat != 0) { 
			if (global_addl_config.mode_color == 1) { gfx_set_fg_color(0xffffff); gfx_set_bg_color(0xff4f32);}
				if (global_addl_config.mode_stat != 3) {					// if MODE/CC compact display set in config
					gfx_printf_pos2(x, top_y, 120, "%s%d%s ", top_status, ch_cc, mic_gain_stat);
				} else {
					gfx_printf_pos2(x, top_y, 120, "%s%d|%d|%s%s%d%s%s   ", top_status, ch_cc, ch_ts, (ad_hoc_tg_channel ? "A":""), (callType == CONTACT_GROUP || callType == CONTACT_GROUP2 ? "" : "P"), tgNum, tg_fill, mic_gain_stat);
				}
			gfx_set_fg_color(0x000000);
			gfx_set_bg_color(0xff8032);
		}

		if (global_addl_config.chan_stat != 0) {
  		    if (usr_find_by_dmrid(&usr, tgNum) == 0) {
			if (global_addl_config.chan_color == 1) { gfx_set_fg_color(0x261162); gfx_set_bg_color(0xff9f32);}

			if (global_addl_config.chan_stat == 1) {						// show TS / TG / CTS / DCS status
				if (global_addl_config.mode_stat != 3) {					// if MODE/CC compact display set in config 
					gfx_printf_pos2(x, y, 120, "%s%d %s%s:%d          ", bot_status, ch_ts, (ad_hoc_tg_channel ? "Ad" : ""), (callType == CONTACT_GROUP || callType == CONTACT_GROUP2 ? "TG" : "Priv"), tgNum);
				} else {
					if (global_addl_config.chan_stat != 4) {		// top=compact - bottom not rx/tx, so show rx, or if 3 = tx
						gfx_printf_pos2(x, y, 120, "%s:%s MHz   ", (global_addl_config.chan_stat == 3 ? "TX" : "RX"), (global_addl_config.chan_stat == 3 ? freq_tx : freq_rx) );
					} else {

						if (fIsSimplex) {
							gfx_printf_pos2(x, y - 3, 120, "-- Simplex -- " );
							gfx_printf_pos2(x, y + 10, 120, "%s MHz   ", freq_tx );
						} else {
							gfx_printf_pos2(x, y, 120, "RX:%s MHz   ", freq_rx );
							gfx_printf_pos2(x, y + 10, 120, "TX:%s MHz   ", freq_tx );
						}
					}
				}
			} else {
				if (global_addl_config.chan_stat != 4) {
					gfx_printf_pos2(x, y, 120, "%s:%s MHz   ", (global_addl_config.chan_stat == 3 ? "TX" : "RX"), (global_addl_config.chan_stat == 3 ? freq_tx : freq_rx) );
				} else {
						if (fIsSimplex) {
							gfx_printf_pos2(x, y - 3, 120, "-- Simplex -- " );
							gfx_printf_pos2(x, y + 10, 120, "%s MHz   ", freq_tx );
						} else {
							gfx_printf_pos2(x, y, 120, "RX:%s MHz   ", freq_rx );
							gfx_printf_pos2(x, y + 10, 120, "TX:%s MHz   ", freq_tx );
						}
				}
			}

		    } else {
			if (global_addl_config.chan_color == 1) { gfx_set_fg_color(0x261162); gfx_set_bg_color(0xff9f32);}
			if (global_addl_config.chan_stat == 1) { 
				gfx_printf_pos2(x, y, 120, "%s%d %s%s:%s          ", bot_status, ch_ts, (ad_hoc_tg_channel ? "Ad" : ""), (callType == CONTACT_GROUP || callType == CONTACT_GROUP2 ? "TG" : "Priv"), usr.callsign);
			} else {
					if (global_addl_config.chan_stat < 4) {
						gfx_printf_pos2(x, y, 120, "%s:%s MHz   ", (global_addl_config.chan_stat == 3 ? "TX" : "RX"), (global_addl_config.chan_stat == 3 ? freq_tx : freq_rx) );
					} else {
						if (fIsSimplex) {
							gfx_printf_pos2(x, y - 3, 120, "-- Simplex -- " );
							gfx_printf_pos2(x, y + 10, 120, "%s MHz   ", freq_tx );
						} else {
							gfx_printf_pos2(x, y, 120, "RX:%s MHz   ", freq_rx );
							gfx_printf_pos2(x, y + 10, 120, "TX:%s MHz   ", freq_tx );
						}
					}
			}
		    }
		}
	//========================================================================================================================//
	}	 				// analog channel active
	//========================================================================================================================//
	else {
		strcpy(bot_status,"");
		if ( *ch_tone_type == 'N') {
			strcpy(fm_sql, "Off");
			strcpy(bot_status, "TX:");					// init bottom string
			strcat(bot_status, ch_tx);					// concat tx frequency
			strcat(bot_status, "MHz");
			strcpy(tg_fill, "   ");
		} else if ( *ch_tone_type == 'D')  {
			strcpy(fm_sql, "DCS");
			strcpy(bot_status, fm_sql);					// init bottom string
			strcat(bot_status, ":");
			strcat(bot_status, current_channel_info_E.EncTone.text);	// add DCS code
			strcpy(tg_fill, "");
		} else {
			
			//if (strlen(current_channel_info_E.EncTone.text) > 5 ) {
			if (! fIsCTSvalid ) {		
				strcpy(fm_sql, "");			
				strcpy(bot_status, fm_sql);
			} else {	
				strcpy(fm_sql, "CTS");
				strcpy(bot_status, fm_sql);					// init bottom string
				strcat(bot_status, ":");
				strcat(bot_status, current_channel_info_E.EncTone.text);	// add CTS tone freq
				strcat(bot_status, "Hz");					// add CTS tone freq
			}
			strcpy(tg_fill, "");
		}

		if (global_addl_config.mode_stat != 3) {				// if MODE/CC compact display set in config
			strcpy(top_status, "FM ");					// init FM string
		} else {
			strcpy(top_status, "FM");					// init FM string
		}
		if (global_addl_config.fm_bw == 0) { strcpy(fm_bw_stat, "|N"); } else { strcpy(fm_bw_stat, "|W"); }

		strcat(top_status, fm_bw_stat);						// |N or |W
		strcat(top_status, ch_offset);						// |-R| or |=>| simplex
		//strcpy(fm_sql, "C");
		if (global_addl_config.mode_stat != 3) {				// if MODE/CC compact display set in config
			strcat(top_status, "|   ");					// space
			strcat(top_status, fm_sql);					// add the tone type to status
		} else {	
			if (*ch_tone_type != 'N')  {	// if MODE/CC compact display set in config (and CTCSS not set to None)
				if (fIsCTSvalid) {
					strcat(top_status, "|");					// less space in compact mode
					strcat(top_status, fm_sql);					// add the tone type to status
					strcat(top_status, ":");
					strcat(top_status, current_channel_info_E.EncTone.text);// add DCS/CTS tone to topstatus in compact mode
				}
				
			} else {
				strcat(top_status, "  ");
			}
		}
		//strcat(top_status, "]");						// Tone squelch status close bracket
	
		if (global_addl_config.mode_stat != 0) { 
			if (global_addl_config.mode_color == 1) { gfx_set_fg_color(0xffffff); gfx_set_bg_color(0xff4f32);}
			gfx_printf_pos2(x, top_y, 120, "%s%s%s ", top_status, tg_fill, mic_gain_stat);
			gfx_set_fg_color(0x000000);
			gfx_set_bg_color(0xff8032);
		}

		if (global_addl_config.chan_stat != 0) { 
			if (global_addl_config.chan_color == 1) { gfx_set_fg_color(0x261162); gfx_set_bg_color(0xff9f32);}

				if (global_addl_config.chan_stat == 1) { 		// 1=show Status CC/CTS/DCS Info
					if (global_addl_config.mode_stat != 3) {	// if MODE/CC compact display set in config
						gfx_printf_pos2(x, y, 120, "%s                  ", bot_status);
					} else {
						if (global_addl_config.chan_stat != 4) {
							gfx_printf_pos2(x, y, 120, "%s:%s MHz     ", (global_addl_config.chan_stat == 3 ? "TX" : "RX"), (global_addl_config.chan_stat == 3 ? freq_tx : freq_rx) );
						} else {
												
							if (fIsSimplex) {
								gfx_printf_pos2(x, y - 3, 120, "-- Simplex -- " );
								gfx_printf_pos2(x, y + 10, 120, "%s MHz   ", freq_tx );
							} else {
								gfx_printf_pos2(x, y, 120, "RX:%s MHz   ", freq_rx );
								gfx_printf_pos2(x, y + 10, 120, "TX:%s MHz   ", freq_tx );
							}
							
						}
					}
				} else {
					if (global_addl_config.chan_stat != 4) {
						gfx_printf_pos2(x, y, 120, "%s:%s MHz     ", (global_addl_config.chan_stat == 3 ? "TX" : "RX"), (global_addl_config.chan_stat == 3 ? freq_tx : freq_rx) );
					} else {
						if (fIsSimplex) {
							gfx_printf_pos2(x, y - 3, 120, "-- Simplex -- " );
							gfx_printf_pos2(x, y + 10, 120, "%s MHz   ", freq_tx );
						} else {
							gfx_printf_pos2(x, y, 120, "RX:%s MHz   ", freq_rx );
							gfx_printf_pos2(x, y + 10, 120, "TX:%s MHz   ", freq_tx );
						}
					}
				}
			}
		}
	//========================================================================================================================//
	gfx_set_fg_color(0x000000);
	gfx_set_bg_color(0xff0000);
	gfx_select_font(gfx_font_norm);
}
#endif

void draw_datetime_row_hook()
{
# if (CONFIG_APP_MENU)
    // If the screen is occupied by the optional 'red button menu', 
    // update or even redraw it completely:
    if( Menu_DrawIfVisible(AM_CALLER_DATETIME_HOOK) )  
     { return; // the menu covers the entire screen, so don't draw anything else
     }
# endif

#if defined(FW_D13_020) || defined(FW_S13_020)
    if( is_netmon_visible() ) {
        return ;
    }
    //if( global_addl_config.mode_stat != 0 || global_addl_config.chan_stat != 0 ) {
	draw_adhoc_statusline(); 
    //}	
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
