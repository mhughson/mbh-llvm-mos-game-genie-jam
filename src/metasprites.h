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

