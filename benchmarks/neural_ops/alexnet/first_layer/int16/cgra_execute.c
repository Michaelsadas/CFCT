void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[157][3] __attribute__((aligned(8))) = {
		{0x8000, 0x5800, 0x0010},
		{0x0100, 0x00b0, 0x0011},
		{0xff10, 0xffff, 0x0012},
		{0xfffe, 0x01ff, 0x0013},
		{0x0800, 0x0000, 0x0014},
		{0x0000, 0x0000, 0x0015},
		{0x8000, 0x5800, 0x0018},
		{0x0100, 0x00b0, 0x0019},
		{0xff10, 0xffff, 0x001a},
		{0xfffe, 0x01ff, 0x001b},
		{0x0800, 0x0000, 0x001c},
		{0x0000, 0x0000, 0x001d},
		{0x8000, 0x5800, 0x0020},
		{0x0100, 0x00b0, 0x0021},
		{0xff10, 0xffff, 0x0022},
		{0xfffe, 0x01ff, 0x0023},
		{0x0800, 0x0000, 0x0024},
		{0x0000, 0x0000, 0x0025},
		{0x8000, 0x5800, 0x0028},
		{0x2900, 0x00b0, 0x0029},
		{0xfbf8, 0xffff, 0x002a},
		{0xfffa, 0x01ff, 0x002b},
		{0x0800, 0x0000, 0x002c},
		{0x0000, 0x0000, 0x002d},
		{0x8000, 0x5800, 0x0030},
		{0x0100, 0x00b0, 0x0031},
		{0xff10, 0xffff, 0x0032},
		{0xfffe, 0x01ff, 0x0033},
		{0x0800, 0x0000, 0x0034},
		{0x0000, 0x0000, 0x0035},
		{0x8000, 0x5800, 0x0038},
		{0x2900, 0x00b0, 0x0039},
		{0xfbf8, 0xffff, 0x003a},
		{0xfffa, 0x01ff, 0x003b},
		{0x0800, 0x0000, 0x003c},
		{0x0000, 0x0000, 0x003d},
		{0x8000, 0x5800, 0x0040},
		{0x0100, 0x00b0, 0x0041},
		{0xff10, 0xffff, 0x0042},
		{0xfffe, 0x01ff, 0x0043},
		{0x0800, 0x0000, 0x0044},
		{0x0000, 0x0000, 0x0045},
		{0x8000, 0x5800, 0x0048},
		{0x0100, 0x00b0, 0x0049},
		{0xff10, 0xffff, 0x004a},
		{0xfffe, 0x01ff, 0x004b},
		{0x0800, 0x0000, 0x004c},
		{0x0000, 0x0000, 0x004d},
		{0x0000, 0x0000, 0x0068},
		{0x0000, 0x1000, 0x0070},
		{0x0000, 0x0080, 0x0078},
		{0x0100, 0x0000, 0x0079},
		{0x0000, 0x4020, 0x0080},
		{0x0000, 0x4080, 0x0088},
		{0x0000, 0x2000, 0x0090},
		{0x0000, 0x0000, 0x0098},
		{0x0000, 0x4003, 0x00b8},
		{0x0004, 0x0000, 0x00b9},
		{0x0000, 0x4003, 0x00c0},
		{0x0004, 0x0000, 0x00c1},
		{0x0000, 0x4003, 0x00c8},
		{0x0002, 0x0000, 0x00c9},
		{0x0000, 0x4003, 0x00d0},
		{0x0004, 0x0000, 0x00d1},
		{0x0000, 0x8003, 0x00d8},
		{0x0002, 0x0000, 0x00d9},
		{0x0000, 0x8003, 0x00e0},
		{0x0002, 0x0000, 0x00e1},
		{0x0000, 0x0000, 0x0110},
		{0x8000, 0x0000, 0x0118},
		{0xc400, 0x0000, 0x0120},
		{0x0000, 0x0000, 0x0128},
		{0x0000, 0x4081, 0x0160},
		{0x0004, 0x0000, 0x0161},
		{0x0000, 0x4041, 0x0168},
		{0x0004, 0x0000, 0x0169},
		{0x0000, 0x4001, 0x0170},
		{0x0002, 0x0000, 0x0171},
		{0xc000, 0x0000, 0x01b0},
		{0x0000, 0x0003, 0x01b8},
		{0x0000, 0x0000, 0x01c0},
		{0x0000, 0xc081, 0x01f8},
		{0x0004, 0x0000, 0x01f9},
		{0x0000, 0x8801, 0x0200},
		{0x0008, 0x0000, 0x0201},
		{0x0000, 0x4401, 0x0208},
		{0x0008, 0x0000, 0x0209},
		{0x0110, 0x0000, 0x0248},
		{0x0000, 0x8000, 0x0250},
		{0x0001, 0x0000, 0x0251},
		{0x0004, 0x8000, 0x0258},
		{0x0000, 0x0000, 0x0259},
		{0x0800, 0x0000, 0x0260},
		{0x0200, 0x0000, 0x0268},
		{0x0000, 0xc003, 0x0290},
		{0x0008, 0x0000, 0x0291},
		{0x0000, 0x4012, 0x0298},
		{0x0000, 0x0000, 0x0299},
		{0x0000, 0x0003, 0x02a0},
		{0x0007, 0x0000, 0x02a1},
		{0x0000, 0x4012, 0x02a8},
		{0x0000, 0x0000, 0x02a9},
		{0x0000, 0x0083, 0x02b0},
		{0x0003, 0x0000, 0x02b1},
		{0x0000, 0x4012, 0x02b8},
		{0x0000, 0x0000, 0x02b9},
		{0x0010, 0x0000, 0x02e0},
		{0x0000, 0x0000, 0x02e8},
		{0x0040, 0x0000, 0x02f0},
		{0x0000, 0x0000, 0x02f1},
		{0x0000, 0x0000, 0x02f8},
		{0x0000, 0x0000, 0x0308},
		{0x8000, 0x5800, 0x0328},
		{0x0100, 0x00b0, 0x0329},
		{0xff10, 0xffff, 0x032a},
		{0xfffe, 0x01ff, 0x032b},
		{0x0800, 0x0000, 0x032c},
		{0x0000, 0x0000, 0x032d},
		{0x0000, 0x5800, 0x0330},
		{0x0000, 0x00b0, 0x0331},
		{0x6002, 0x0401, 0x0332},
		{0x02c0, 0x0000, 0x0333},
		{0x8800, 0x0004, 0x0334},
		{0x0000, 0x0000, 0x0335},
		{0x0040, 0x0000, 0x0336},
		{0x8000, 0x5800, 0x0338},
		{0x2900, 0x00b0, 0x0339},
		{0xfbf8, 0xffff, 0x033a},
		{0xfffa, 0x01ff, 0x033b},
		{0x0800, 0x0000, 0x033c},
		{0x0000, 0x0000, 0x033d},
		{0x0000, 0x5800, 0x0340},
		{0x0000, 0x00b0, 0x0341},
		{0x6002, 0x0401, 0x0342},
		{0x02c0, 0x0000, 0x0343},
		{0x8800, 0x0004, 0x0344},
		{0x0000, 0x0000, 0x0345},
		{0x0040, 0x0000, 0x0346},
		{0x8000, 0x5800, 0x0348},
		{0x0100, 0x00b0, 0x0349},
		{0xff10, 0xffff, 0x034a},
		{0xfffe, 0x01ff, 0x034b},
		{0x0800, 0x0000, 0x034c},
		{0x0000, 0x0000, 0x034d},
		{0x0000, 0x5800, 0x0350},
		{0x0000, 0x00b0, 0x0351},
		{0x6002, 0x0401, 0x0352},
		{0x02c0, 0x0000, 0x0353},
		{0x9800, 0x0004, 0x0354},
		{0x0000, 0x0000, 0x0355},
		{0x0040, 0x0000, 0x0356},
		{0x8000, 0x5800, 0x0358},
		{0x0100, 0x00b0, 0x0359},
		{0xff10, 0xffff, 0x035a},
		{0xfffe, 0x01ff, 0x035b},
		{0x0800, 0x0000, 0x035c},
		{0x0000, 0x0000, 0x035d},
	};

	load_cfg((void*)cin, 0x24000, 942, 1);
	load_data(din_addr[0], 0x8000, 5202, 0, 1);
	load_data(din_addr[1], 0x10000, 5202, 0, 1);
	load_data(din_addr[2], 0xa000, 5202, 0, 1);
	load_data(din_addr[3], 0x0, 242, 0, 1);
	load_data(din_addr[4], 0x12000, 242, 0, 1);
	load_data(din_addr[5], 0xc000, 242, 0, 1);
	load_data(din_addr[6], 0x2000, 242, 0, 1);
	load_data(din_addr[7], 0x18000, 242, 0, 1);
	load_data(din_addr[8], 0xe000, 242, 0, 1);
	load_data(din_addr[9], 0x4000, 242, 0, 1);
	load_data(din_addr[10], 0x1a000, 242, 0, 1);
	load_data(din_addr[11], 0x14000, 242, 0, 1);
	config(0x0, 157, 1);
	execute(0xfffe, 1);
	store(dout_addr[0], 0x16000, 242, 1);
	store(dout_addr[1], 0x1c000, 242, 1);
	store(dout_addr[2], 0x1e000, 242, 1);
}
