/* gameplaySP
 *
 * Copyright (C) 2006 Exophase <exophase@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "common.h"

#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <linux/kd.h>
// Special thanks to psp298 for the analog->dpad code!

void trigger_key(u32 key)
{
  u32 p1_cnt = io_registers[REG_P1CNT];
  u32 p1;

  if((p1_cnt >> 14) & 0x01)
  {
    u32 key_intersection = (p1_cnt & key) & 0x3FF;

    if(p1_cnt >> 15)
    {
      if(key_intersection == (p1_cnt & 0x3FF))
        raise_interrupt(IRQ_KEYPAD);
    }
    else
    {
      if(key_intersection)
        raise_interrupt(IRQ_KEYPAD);
    }
  }
}

u32 key = 0;

u32 gamepad_config_map[16] =
{
  BUTTON_ID_MENU,
  BUTTON_ID_A,
  BUTTON_ID_B,
  BUTTON_ID_START,
  BUTTON_ID_L,
  BUTTON_ID_R,
  BUTTON_ID_DOWN,
  BUTTON_ID_LEFT,
  BUTTON_ID_UP,
  BUTTON_ID_RIGHT,
  BUTTON_ID_SELECT,
  BUTTON_ID_START,
  BUTTON_ID_UP,
  BUTTON_ID_DOWN,
  BUTTON_ID_LEFT,
  BUTTON_ID_RIGHT
};

u32 global_enable_analog = 1;
u32 analog_sensitivity_level = 4;

typedef enum
{
  BUTTON_NOT_HELD,
  BUTTON_HELD_INITIAL,
  BUTTON_HELD_REPEAT
} button_repeat_state_type;


// These define autorepeat values (in microseconds), tweak as necessary.

#define BUTTON_REPEAT_START    200000
#define BUTTON_REPEAT_CONTINUE 50000

button_repeat_state_type button_repeat_state = BUTTON_NOT_HELD;
u32 button_repeat = 0;
gui_action_type cursor_repeat = CURSOR_NONE;


#ifdef PSP_BUILD

#define PSP_ALL_BUTTON_MASK 0xFFFF

gui_action_type get_gui_input()
{
  SceCtrlData ctrl_data;
  gui_action_type new_button = CURSOR_NONE;
  u32 new_buttons;

  static u32 last_buttons = 0;
  static u64 button_repeat_timestamp;

  delay_us(25000);

  sceCtrlPeekBufferPositive(&ctrl_data, 1);
  ctrl_data.Buttons &= PSP_ALL_BUTTON_MASK;
  new_buttons = (last_buttons ^ ctrl_data.Buttons) & ctrl_data.Buttons;
  last_buttons = ctrl_data.Buttons;

  if(new_buttons & PSP_CTRL_LEFT)
    new_button = CURSOR_LEFT;

  if(new_buttons & PSP_CTRL_RIGHT)
    new_button = CURSOR_RIGHT;

  if(new_buttons & PSP_CTRL_UP)
    new_button = CURSOR_UP;

  if(new_buttons & PSP_CTRL_DOWN)
    new_button = CURSOR_DOWN;

  if(new_buttons & PSP_CTRL_START)
    new_button = CURSOR_SELECT;

  if(new_buttons & PSP_CTRL_CIRCLE)
    new_button = CURSOR_SELECT;

  if(new_buttons & PSP_CTRL_CROSS)
    new_button = CURSOR_EXIT;

  if(new_buttons & PSP_CTRL_SQUARE)
    new_button = CURSOR_BACK;

  if(new_button != CURSOR_NONE)
  {
    get_ticks_us(&button_repeat_timestamp);
    button_repeat_state = BUTTON_HELD_INITIAL;
    button_repeat = new_buttons;
    cursor_repeat = new_button;
  }
  else
  {
    if(ctrl_data.Buttons & button_repeat)
    {
      u64 new_ticks;
      get_ticks_us(&new_ticks);

      if(button_repeat_state == BUTTON_HELD_INITIAL)
      {
        if((new_ticks - button_repeat_timestamp) >
         BUTTON_REPEAT_START)
        {
          new_button = cursor_repeat;
          button_repeat_timestamp = new_ticks;
          button_repeat_state = BUTTON_HELD_REPEAT;
        }
      }

      if(button_repeat_state == BUTTON_HELD_REPEAT)
      {
        if((new_ticks - button_repeat_timestamp) >
         BUTTON_REPEAT_CONTINUE)
        {
          new_button = cursor_repeat;
          button_repeat_timestamp = new_ticks;
        }
      }
    }
  }

  return new_button;
}

#define PSP_CTRL_ANALOG_UP    (1 << 28)
#define PSP_CTRL_ANALOG_DOWN  (1 << 29)
#define PSP_CTRL_ANALOG_LEFT  (1 << 30)
#define PSP_CTRL_ANALOG_RIGHT (1 << 31)

u32 button_psp_mask_to_config[] =
{
  PSP_CTRL_TRIANGLE,
  PSP_CTRL_CIRCLE,
  PSP_CTRL_CROSS,
  PSP_CTRL_SQUARE,
  PSP_CTRL_LTRIGGER,
  PSP_CTRL_RTRIGGER,
  PSP_CTRL_DOWN,
  PSP_CTRL_LEFT,
  PSP_CTRL_UP,
  PSP_CTRL_RIGHT,
  PSP_CTRL_SELECT,
  PSP_CTRL_START,
  PSP_CTRL_ANALOG_UP,
  PSP_CTRL_ANALOG_DOWN,
  PSP_CTRL_ANALOG_LEFT,
  PSP_CTRL_ANALOG_RIGHT
};

u32 button_id_to_gba_mask[] =
{
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_LEFT,
  BUTTON_RIGHT,
  BUTTON_A,
  BUTTON_B,
  BUTTON_L,
  BUTTON_R,
  BUTTON_START,
  BUTTON_SELECT,
  BUTTON_NONE,
  BUTTON_NONE,
  BUTTON_NONE,
  BUTTON_NONE
};

gui_action_type get_gui_input_fs_hold(u32 button_id)
{
  gui_action_type new_button = get_gui_input();
  if((last_buttons & button_psp_mask_to_config[button_id]) == 0)
    return CURSOR_BACK;

  return new_button;
}

u32 rapidfire_flag = 1;

u32 update_input()
{
  SceCtrlData ctrl_data;
  u32 buttons;
  u32 non_repeat_buttons;
  u32 button_id;
  u32 i;
  u32 new_key = 0;
  u32 analog_sensitivity = 92 - (analog_sensitivity_level * 4);
  u32 inv_analog_sensitivity = 256 - analog_sensitivity;

  sceCtrlPeekBufferPositive(&ctrl_data, 1);

  buttons = ctrl_data.Buttons;

  if(global_enable_analog)
  {
    if(ctrl_data.Lx < analog_sensitivity)
      buttons |= PSP_CTRL_ANALOG_LEFT;

    if(ctrl_data.Lx > inv_analog_sensitivity)
      buttons |= PSP_CTRL_ANALOG_RIGHT;

    if(ctrl_data.Ly < analog_sensitivity)
      buttons |= PSP_CTRL_ANALOG_UP;

    if(ctrl_data.Ly > inv_analog_sensitivity)
      buttons |= PSP_CTRL_ANALOG_DOWN;
  }

  non_repeat_buttons = (last_buttons ^ buttons) & buttons;
  last_buttons = buttons;

  for(i = 0; i < 16; i++)
  {
    if(non_repeat_buttons & button_psp_mask_to_config[i])
      button_id = gamepad_config_map[i];
    else
      button_id = BUTTON_ID_NONE;

    switch(button_id)
    {
      case BUTTON_ID_MENU:
      {
        u16 *screen_copy = copy_screen();
        u32 ret_val = menu(screen_copy);
        free(screen_copy);

        return ret_val;
      }

      case BUTTON_ID_LOADSTATE:
      {
        u8 current_savestate_filename[512];
        get_savestate_filename_noshot(savestate_slot,
         current_savestate_filename);
        load_state(current_savestate_filename);
        return 1;
      }

      case BUTTON_ID_SAVESTATE:
      {
        u8 current_savestate_filename[512];
        u16 *current_screen = copy_screen();
        get_savestate_filename_noshot(savestate_slot,
         current_savestate_filename);
        save_state(current_savestate_filename, current_screen);
        free(current_screen);
        return 0;
      }

      case BUTTON_ID_FASTFORWARD:
        print_string("FASTFORWARD", 0xFFFF, 0x0000, 0, 50);
        synchronize_flag ^= 1;
        return 0;
    }

    if(buttons & button_psp_mask_to_config[i])
    {
      button_id = gamepad_config_map[i];
      if(button_id < BUTTON_ID_MENU)
      {
        new_key |= button_id_to_gba_mask[button_id];
      }
      else

      if((button_id >= BUTTON_ID_RAPIDFIRE_A) &&
       (button_id <= BUTTON_ID_RAPIDFIRE_L))
      {
        rapidfire_flag ^= 1;
        if(rapidfire_flag)
        {
          new_key |= button_id_to_gba_mask[button_id -
           BUTTON_ID_RAPIDFIRE_A + BUTTON_ID_A];
        }
        else
        {
          new_key &= ~button_id_to_gba_mask[button_id -
           BUTTON_ID_RAPIDFIRE_A + BUTTON_ID_A];
        }
      }
    }
  }

  if((new_key | key) != key)
    trigger_key(new_key);

  key = new_key;

  io_registers[REG_P1] = (~key) & 0x3FF;

  return 0;
}

void init_input()
{
  sceCtrlSetSamplingCycle(0);
  sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
}

#endif


extern u32 gp2x_fps_debug;
#ifdef GP2X_BUILD

/* GP2X SDL requires a user made input method */
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "gp2x/gp2x.h"


u32 gpsp_gp2x_joystick_read(void)
{
  u32 value = (gpsp_gp2x_memregs[0x1198 >> 1] & 0x00FF);

  if(value == 0xFD)
   value = 0xFA;
  if(value == 0xF7)
   value = 0xEB;
  if(value == 0xDF)
   value = 0xAF;
  if(value == 0x7F)
   value = 0xBE;

  return ~((gpsp_gp2x_memregs[0x1184 >> 1] & 0xFF00) | value |
   (gpsp_gp2x_memregs[0x1186 >> 1] << 16));
}

u32 gp2x_joy_map(u32 buttons)
{
  u32 mapped_buttons = BUTTON_NONE;

  if(buttons & GP2X_L)
    mapped_buttons |= BUTTON_L;

  if(buttons & GP2X_R)
    mapped_buttons |= BUTTON_R;

  if(buttons & GP2X_START)
    mapped_buttons |= BUTTON_START;

  if(buttons & GP2X_SELECT)
    mapped_buttons |= BUTTON_SELECT;

  if(buttons & GP2X_X)
    mapped_buttons |= BUTTON_B;

  if(buttons & GP2X_B)
    mapped_buttons |= BUTTON_A;

  if(buttons & GP2X_UP)
    mapped_buttons |= BUTTON_UP;

  if(buttons & GP2X_DOWN)
    mapped_buttons |= BUTTON_DOWN;

  if(buttons & GP2X_LEFT)
    mapped_buttons |= BUTTON_LEFT;

  if(buttons & GP2X_RIGHT)
    mapped_buttons |= BUTTON_RIGHT;

  return mapped_buttons;
}

gui_action_type get_gui_input()
{
  gui_action_type new_button = CURSOR_NONE;
  u32 buttons = gpsp_gp2x_joystick_read();
  u32 new_buttons;

  static u32 last_buttons = 0;
  static u64 button_repeat_timestamp;

  delay_us(25000);

  new_buttons = (last_buttons ^ buttons) & buttons;
  last_buttons = buttons;

  if(new_buttons & GP2X_A)
    new_button = CURSOR_BACK;

  if(new_buttons & GP2X_X)
    new_button = CURSOR_EXIT;

  if(new_buttons & GP2X_B)
    new_button = CURSOR_SELECT;

  if(new_buttons & GP2X_UP)
    new_button = CURSOR_UP;

  if(new_buttons & GP2X_DOWN)
    new_button = CURSOR_DOWN;

  if(new_buttons & GP2X_LEFT)
    new_button = CURSOR_LEFT;

  if(new_buttons & GP2X_RIGHT)
    new_button = CURSOR_RIGHT;


  if(new_button != CURSOR_NONE)
  {
    get_ticks_us(&button_repeat_timestamp);
    button_repeat_state = BUTTON_HELD_INITIAL;
    button_repeat = new_buttons;
    cursor_repeat = new_button;
  }
  else
  {
    if(buttons & button_repeat)
    {
      u64 new_ticks;
      get_ticks_us(&new_ticks);

      if(button_repeat_state == BUTTON_HELD_INITIAL)
      {
        if((new_ticks - button_repeat_timestamp) >
         BUTTON_REPEAT_START)
        {
          new_button = cursor_repeat;
          button_repeat_timestamp = new_ticks;
          button_repeat_state = BUTTON_HELD_REPEAT;
        }
      }

      if(button_repeat_state == BUTTON_HELD_REPEAT)
      {
        if((new_ticks - button_repeat_timestamp) >
         BUTTON_REPEAT_CONTINUE)
        {
          new_button = cursor_repeat;
          button_repeat_timestamp = new_ticks;
        }
      }
    }
  }

  return new_button;
}

u32 update_input()
{
  u32 gp2x_keys = gpsp_gp2x_joystick_read();

  if((gp2x_keys & GP2X_L) && (gp2x_keys & GP2X_R) &&
   (gp2x_keys & GP2X_VOL_DOWN) && (gp2x_keys & GP2X_VOL_UP))
  {
    gp2x_fps_debug = !gp2x_fps_debug;
  }
  else

  if((gp2x_keys & GP2X_VOL_DOWN) && (gp2x_keys & GP2X_VOL_UP))
  {
    u16 *screen_copy = copy_screen();
    u32 ret_val = menu(screen_copy);
    free(screen_copy);

    return ret_val;
  }

  if(gp2x_keys & GP2X_VOL_DOWN)
    gp2x_sound_volume(0);

  if(gp2x_keys & GP2X_VOL_UP)
    gp2x_sound_volume(1);

  key = gp2x_joy_map(gp2x_keys);
  trigger_key(key);

  io_registers[REG_P1] = (~key) & 0x3FF;

  return 0;
}

void init_input()
{

}

#endif

static int consoleFd = -1;
static struct termios old;
//char *keynames = {""};
#define  KEYCODE(a) (a & 0x7f)

#define KH_KEY_MENU    50
#define KH_KEY_REWIND  17
#define KH_KEY_PLAY    32
#define KH_KEY_FORWARD 33

#define KH_KEY_ACTION  28
#define KH_KEY_HOLD    35

#define KH_WHEEL_L     38
#define KH_WHEEL_R     19
int START_INPUT(void)
{
	  struct termios new;
FILE *fp;
  
	    if ((consoleFd = open("/dev/console",O_RDONLY | O_NONBLOCK)) < 0)
		        fprintf(stderr, "Could not open /dev/console");

	      if (tcgetattr(consoleFd, &old) < 0)
		          fprintf(stderr, "Could not save old termios");

	        new = old;

		  new.c_lflag    &= ~(ICANON | ECHO  | ISIG);
		    new.c_iflag    &= ~(ISTRIP | IGNCR | ICRNL | INLCR | IXOFF | IXON | BRKINT);
		      new.c_cc[VMIN]  = 0;
		        new.c_cc[VTIME] = 0;

			  if (tcsetattr(consoleFd, TCSAFLUSH, &new) < 0)
				      exit(0);

			    if (ioctl(consoleFd, KDSKBMODE, K_MEDIUMRAW) < 0)
				        exit(0);

			      return 0;
}

void STOP_INPUT(void)
{
	  if (tcsetattr(consoleFd, TCSAFLUSH, &old) < 0)
		      fprintf(stderr, "Could not reset old termios");

	    if (consoleFd > 2)
		        if (close(consoleFd) < 0)
				      fprintf(stderr, "Could not close console");
}


int GET_KEY(void)
{
	  int c = 0;

	    if (read(consoleFd, &c, 1) != 1)
		        c = -1;

	      return c;
}
int inputstarted=1;
u32 mappedbuttons=0;
u32 update_input() {

	if (inputstarted) { START_INPUT();  inputstarted=0; }
  u32 mapped_buttons = BUTTON_NONE;
	u32 k,in,st,key;
	unsigned char touch,button;
        in = inl(0x7000C140);
        st = (in & 0xff000000) >> 24;
        if(st == 0xc0)
        {       // touched
                touch = (in & 0x007F0000 ) >> 16;
                touch+=6;
                touch/=12;
                if(touch>7) touch=0;
                k=touch;//touch_map[touch]);
	  switch(k)
	  {
		  case 0:
    			  mapped_buttons |= BUTTON_UP;
			  break;
		  case 1:
    			  mapped_buttons |= BUTTON_A;
			  break;
		  case 2:
    			  mapped_buttons |= BUTTON_RIGHT;
			  break;
		  case 3:
    			  mapped_buttons |= BUTTON_START;
			  break;
		  case 4:
    			  mapped_buttons |= BUTTON_DOWN;
			  break;
		  case 5:
    			  mapped_buttons |= BUTTON_SELECT;
			  break;
		  case 6:
    			  mapped_buttons |= BUTTON_LEFT;
			  break;
		  case 7:
    			  mapped_buttons |= BUTTON_B;
			  break;
	  }
  }
  k=GET_KEY();
    while (k!=-1)
    {
    int up=k & 0x80;
    int lk;
    k=KEYCODE(k);
//    if (k == KH_KEY_MENU) { STOP_INPUT(); exit(0); }
    if (up ==0) { 
          u16 *screen_copy;
	    switch (k)
	    {
		case KH_KEY_MENU: 
    			  mappedbuttons |= BUTTON_UP;
    			  mappedbuttons |= BUTTON_B;
		    break;
		case KH_KEY_REWIND:
    			  mappedbuttons |= BUTTON_L;
				  // KERIPO MOD
    			  //mappedbuttons |= BUTTON_B;
		    break;
		case KH_KEY_ACTION:
    			  mappedbuttons |= BUTTON_A;
		    break;
		case KH_KEY_FORWARD:
    			  mappedbuttons |= BUTTON_R;
				  // KERIPO MOD
    			  //mappedbuttons |= BUTTON_B;
		    break;
		case KH_KEY_PLAY:
    			  mappedbuttons |= BUTTON_DOWN;
    			  mappedbuttons |= BUTTON_B;
		    break;
		case KH_KEY_HOLD:
          screen_copy = copy_screen();
          u32 ret_val = menu(screen_copy);
          free(screen_copy);
		    break;
	    }
	    
    }
    else
    {
	    switch (k)
	    {
		case KH_KEY_MENU: 
    			  mappedbuttons &=~BUTTON_UP;
    			  mappedbuttons &=~BUTTON_B;
		    break;
		case KH_KEY_REWIND:
				  // KERIPO MOD
    			  //mappedbuttons &=~BUTTON_LEFT;
    			  //mappedbuttons &=~BUTTON_B;
				  mappedbuttons &=~BUTTON_L;
		    break;
		case KH_KEY_ACTION:
    			  mappedbuttons &=~BUTTON_A;
				  // KERIPO MOD
    			  //gp2x_fps_debug = !gp2x_fps_debug;
		    break;
		case KH_KEY_FORWARD:
    			  // KERIPO MOD
				  //mappedbuttons &=~BUTTON_RIGHT;
    			  //mappedbuttons &=~BUTTON_B;
				  mappedbuttons &=~BUTTON_R;
		    break;
		case KH_KEY_PLAY:
    			  mappedbuttons &=~BUTTON_DOWN;
    			  mappedbuttons &=~BUTTON_B;
		    break;
		case KH_KEY_HOLD:
		    break;
	    }
    }
  k=GET_KEY();
  
    }
  key=mapped_buttons;
  key|=mappedbuttons;
  trigger_key(key);
  io_registers[REG_P1] = (~key) & 0x3FF;
//            gui_action = CURSOR_EXIT;
	return 0;
};
void init_input() {
/*  if(buttons & GP2X_L)
    mapped_buttons |= BUTTON_L;

  if(buttons & GP2X_R)
    mapped_buttons |= BUTTON_R;*/

};

gui_action_type get_gui_input()
{
	if (inputstarted) { START_INPUT();  inputstarted=0; }
  gui_action_type gui_action = CURSOR_NONE;
  int k=GET_KEY();
    if (k!=-1)
    {
    int up=k & 0x80;
    int lk;
    k=KEYCODE(k);
//    if (k == KH_KEY_MENU) { STOP_INPUT(); exit(0); }
    if (up ==0) { 
	    switch (k)
	    {
		case KH_KEY_MENU: 
            gui_action = CURSOR_UP;
		    break;
		case KH_KEY_REWIND:
            gui_action = CURSOR_LEFT;
		    break;
		case KH_KEY_ACTION:
            gui_action = CURSOR_SELECT;
		    break;
		case KH_KEY_FORWARD:
            gui_action = CURSOR_RIGHT;
		    break;
		case KH_KEY_PLAY:
            gui_action = CURSOR_DOWN;
		    break;
		case KH_KEY_HOLD:
            gui_action = CURSOR_EXIT;
		    break;
	    }

    }
    k=GET_KEY(); 
    }
    return gui_action;
}
#ifdef PC_BUILD

u32 key_map(SDLKey key_sym)
{
  switch(key_sym)
  {
    case SDLK_LSHIFT:
      return BUTTON_L;

    case SDLK_x:
      return BUTTON_R;

    case SDLK_DOWN:
      return BUTTON_DOWN;

    case SDLK_UP:
      return BUTTON_UP;

    case SDLK_LEFT:
      return BUTTON_LEFT;

    case SDLK_RIGHT:
      return BUTTON_RIGHT;

    case SDLK_RETURN:
      return BUTTON_START;

    case SDLK_RSHIFT:
      return BUTTON_SELECT;

    case SDLK_LCTRL:
      return BUTTON_B;

    case SDLK_LALT:
      return BUTTON_A;

    default:
      return BUTTON_NONE;
  }
}

u32 joy_map(u32 button)
{
  switch(button)
  {
    case 4:
      return BUTTON_L;

    case 5:
      return BUTTON_R;

    case 9:
      return BUTTON_START;

    case 8:
      return BUTTON_SELECT;

    case 0:
      return BUTTON_B;

    case 1:
      return BUTTON_A;

    default:
      return BUTTON_NONE;
  }
}

gui_action_type get_gui_input()
{
  SDL_Event event;
  gui_action_type gui_action = CURSOR_NONE;

  delay_us(30000);

  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_QUIT:
        quit();

      case SDL_KEYDOWN:
      {
        switch(event.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            gui_action = CURSOR_EXIT;
            break;

          case SDLK_DOWN:
            gui_action = CURSOR_DOWN;
            break;

          case SDLK_UP:
            gui_action = CURSOR_UP;
            break;

          case SDLK_LEFT:
            gui_action = CURSOR_LEFT;
            break;

          case SDLK_RIGHT:
            gui_action = CURSOR_RIGHT;
            break;

          case SDLK_RETURN:
            gui_action = CURSOR_SELECT;
            break;

          case SDLK_BACKSPACE:
            gui_action = CURSOR_BACK;
            break;
        }
        break;
      }
    }
  }

  return gui_action;
}

// FIXME: Not implemented properly for x86 version.

gui_action_type get_gui_input_fs_hold(u32 button_id)
{
  return get_gui_input();
}

u32 update_input()
{
  SDL_Event event;

  while(SDL_PollEvent(&event))
  {
    switch(event.type)
    {
      case SDL_QUIT:
        quit();

      case SDL_KEYDOWN:
      {
        if(event.key.keysym.sym == SDLK_ESCAPE)
        {
          quit();
        }

        if(event.key.keysym.sym == SDLK_BACKSPACE)
        {
          u16 *screen_copy = copy_screen();
          u32 ret_val = menu(screen_copy);
          free(screen_copy);

          return ret_val;
        }
        else

        if(event.key.keysym.sym == SDLK_F1)
        {
          debug_on();
        }
        else

        if(event.key.keysym.sym == SDLK_F2)
        {
          FILE *fp = fopen("palette_ram.bin", "wb");
          printf("writing palette RAM\n");
          fwrite(palette_ram, 1024, 1, fp);
          fclose(fp);
          printf("writing palette VRAM\n");
          fp = fopen("vram.bin", "wb");
          fwrite(vram, 1024 * 96, 1, fp);
          fclose(fp);
          printf("writing palette OAM RAM\n");
          fp = fopen("oam_ram.bin", "wb");
          fwrite(oam_ram, 1024, 1, fp);
          fclose(fp);
          printf("writing palette I/O registers\n");
          fp = fopen("io_registers.bin", "wb");
          fwrite(io_registers, 1024, 1, fp);
          fclose(fp);
        }
        else

        if(event.key.keysym.sym == SDLK_F3)
        {
          dump_translation_cache();
        }
        else

        if(event.key.keysym.sym == SDLK_F5)
        {
          u8 current_savestate_filename[512];
          u16 *current_screen = copy_screen();
          get_savestate_filename_noshot(savestate_slot,
           current_savestate_filename);
          save_state(current_savestate_filename, current_screen);
          free(current_screen);
        }
        else

        if(event.key.keysym.sym == SDLK_F7)
        {
          u8 current_savestate_filename[512];
          get_savestate_filename_noshot(savestate_slot,
           current_savestate_filename);
          load_state(current_savestate_filename);
          debug_on();
          return 1;
        }
        else

        if(event.key.keysym.sym == SDLK_BACKQUOTE)
        {
          synchronize_flag ^= 1;
        }
        else
        {
          key |= key_map(event.key.keysym.sym);
          trigger_key(key);
        }

        break;
      }

      case SDL_KEYUP:
      {
        key &= ~(key_map(event.key.keysym.sym));
        break;
      }

      case SDL_JOYBUTTONDOWN:
      {
        key |= joy_map(event.jbutton.button);
        trigger_key(key);
        break;
      }

      case SDL_JOYBUTTONUP:
      {
        key &= ~(joy_map(event.jbutton.button));
        break;
      }
    }
  }

  io_registers[REG_P1] = (~key) & 0x3FF;

  return 0;
}

void init_input()
{
  u32 joystick_count = SDL_NumJoysticks();

  if(joystick_count > 0)
  {
    SDL_JoystickOpen(0);
    SDL_JoystickEventState(SDL_ENABLE);
  }
}

#endif


#define input_savestate_builder(type)                                         \
void input_##type##_savestate(file_tag_type savestate_file)                   \
{                                                                             \
  file_##type##_variable(savestate_file, key);                                \
}                                                                             \

input_savestate_builder(read);
input_savestate_builder(write_mem);

