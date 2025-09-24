const int8_t metaspr_NTSCData[]={

	6, 6, 6

};

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

const int8_t* const metaspr_list[]={

	metaspr_box_16_16_data,
	metaspr_enemy_bot_walk_00_data,
	metaspr_enemy_bot_walk_01_data

};

const int8_t metaspr_player_walk_00_data[]={
	- 4,-11,0x0f,0,
	  0,  3,0x06,0,
	- 2,- 4,0x05,0,
	  1,- 5,0x06,0,
	- 9,- 5,0x09,0,
	- 7,  3,0x09,0,
	(int8_t)0x80
};
const int8_t metaspr_player_walk_01_data[]={
	- 4,-11,0x0f,0,
	  0,  3,0x06,0,
	- 2,- 4,0x05,0,
	  2,- 6,0x06,(int8_t)(0|OAM_FLIP_V),
	-10,- 6,0x09,(int8_t)(0|OAM_FLIP_V),
	- 7,  3,0x09,0,
	(int8_t)0x80
};

const int8_t* player_metaspr_list[] = {
    metaspr_player_walk_00_data,
    metaspr_player_walk_01_data,
};