const int8_t metaspr_box_16_16_data[]={

	  0,  0,0x0f,0,
	  8,  0,0x0f,0,
	  0,  8,0x0f,0,
	  8,  8,0x0f,0,
	(int8_t)0x80

};

const int8_t metaspr_enemy_bot_walk_00_data[]={

	 12,  5,0x05,3,
	- 4,  5,0x08,3,
	  4,  0,0x0f,1,
	(int8_t)0x80

};

const int8_t metaspr_enemy_bot_walk_01_data[]={

	 12,  5,0x04,3,
	- 4,  5,0x0a,3,
	  4,  0,0x0f,1,
	(int8_t)0x80

};

const int8_t metaspr_player_walk_00_data[]={

	  1,  0,0x0d,1,
	  9,  0,0x0d,1,
	  1,  8,0x07,1,
	  9,  8,0x07,1,

	  6, 16,0x0f,0,
	 14, 24,0x05,2,
	- 2, 24,0x08,2,
	(int8_t)0x80

};

const int8_t metaspr_player_walk_01_data[]={

	  1,  0,0x0d,1,
	  9,  0,0x0d,1,
	  1,  8,0x07,1,
	  9,  8,0x07,1,

	  6, 16,0x0f,0,
	 14, 24,0x04,2,
	- 2, 24,0x0a,2,
	(int8_t)0x80

};

const int8_t metaspr_player_idle_00_data[]={

	  1,  0,0x0d,1,
	  9,  0,0x0d,1,
	  1,  8,0x07,1,
	  9,  8,0x07,1,

	  6, 16,0x0f,0,
	 14, 24,0x05,2,
	- 2, 24,0x0a,2,
	(int8_t)0x80

};

const int8_t metaspr_player_walk_00__1_data[]={

	 11,  0,0x0d,1|OAM_FLIP_H,
	  3,  0,0x0d,1|OAM_FLIP_H,
	 11,  8,0x07,1|OAM_FLIP_H,
	  3,  8,0x07,1|OAM_FLIP_H,

	  6, 16,0x0f,0|OAM_FLIP_H,
	- 2, 24,0x05,2|OAM_FLIP_H,
	 14, 24,0x08,2|OAM_FLIP_H,
	(int8_t)0x80

};

const int8_t metaspr_player_walk_01__1_data[]={

	 11,  0,0x0d,1|OAM_FLIP_H,
	  3,  0,0x0d,1|OAM_FLIP_H,
	 11,  8,0x07,1|OAM_FLIP_H,
	  3,  8,0x07,1|OAM_FLIP_H,

	  6, 16,0x0f,0|OAM_FLIP_H,
	- 2, 24,0x04,2|OAM_FLIP_H,
	 14, 24,0x0a,2|OAM_FLIP_H,
	(int8_t)0x80

};

const int8_t metaspr_player_idle_00__1_data[]={

	 11,  0,0x0d,1|OAM_FLIP_H,
	  3,  0,0x0d,1|OAM_FLIP_H,
	 11,  8,0x07,1|OAM_FLIP_H,
	  3,  8,0x07,1|OAM_FLIP_H,

	  6, 16,0x0f,0|OAM_FLIP_H,
	- 2, 24,0x05,2|OAM_FLIP_H,
	 14, 24,0x0a,2|OAM_FLIP_H,
	(int8_t)0x80

};

const int8_t* const metaspr_list[]={

	metaspr_box_16_16_data,
	metaspr_enemy_bot_walk_00_data,
	metaspr_enemy_bot_walk_01_data,
	metaspr_player_walk_00_data,
	metaspr_player_walk_01_data,
	metaspr_player_idle_00_data,
	metaspr_player_walk_00__1_data,
	metaspr_player_walk_01__1_data,
	metaspr_player_idle_00__1_data

};

