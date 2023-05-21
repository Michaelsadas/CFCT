void cgra_execute(void** din_addr, void** dout_addr)
{
	volatile unsigned short cin[132][3] __attribute__((aligned(8))) = {
		{0x4000, 0x4000, 0x0018},
		{0x0006, 0x0000, 0x0019},
		{0x0000, 0x0000, 0x001a},
		{0x0000, 0x0000, 0x001b},
		{0x0800, 0x0000, 0x001c},
		{0x0000, 0x0000, 0x001d},
		{0x6000, 0x4000, 0x0030},
		{0x0006, 0x0000, 0x0031},
		{0x0000, 0x0000, 0x0032},
		{0x0000, 0x0000, 0x0033},
		{0xb800, 0x0004, 0x0034},
		{0x0000, 0x0000, 0x0035},
		{0x2000, 0x0000, 0x0036},
		{0x4000, 0x4000, 0x0038},
		{0x0006, 0x0000, 0x0039},
		{0x0000, 0x0000, 0x003a},
		{0x0000, 0x0000, 0x003b},
		{0x0800, 0x0000, 0x003c},
		{0x0000, 0x0000, 0x003d},
		{0x5000, 0x4000, 0x0040},
		{0x0006, 0x0000, 0x0041},
		{0x0000, 0x0000, 0x0042},
		{0x0000, 0x0000, 0x0043},
		{0xb800, 0x0004, 0x0044},
		{0x0000, 0x0000, 0x0045},
		{0x0000, 0x0000, 0x0046},
		{0x5000, 0x4000, 0x0048},
		{0x0006, 0x0000, 0x0049},
		{0x0000, 0x0000, 0x004a},
		{0x0000, 0x0000, 0x004b},
		{0xa800, 0x0004, 0x004c},
		{0x0000, 0x0000, 0x004d},
		{0x2000, 0x0000, 0x004e},
		{0x4000, 0x4000, 0x0058},
		{0x0006, 0x0000, 0x0059},
		{0x0000, 0x0000, 0x005a},
		{0x0000, 0x0000, 0x005b},
		{0x0800, 0x0000, 0x005c},
		{0x0000, 0x0000, 0x005d},
		{0x0000, 0x0000, 0x0089},
		{0x0100, 0x0000, 0x0091},
		{0x0080, 0x0000, 0x0099},
		{0x0002, 0x0080, 0x00a0},
		{0x0080, 0x0000, 0x00a1},
		{0x0800, 0x0000, 0x00a8},
		{0x0000, 0x0000, 0x00a9},
		{0x0002, 0x0000, 0x00b8},
		{0x0000, 0x0000, 0x00c9},
		{0x0000, 0x1c1b, 0x0108},
		{0x0003, 0x0000, 0x0109},
		{0x0040, 0x0000, 0x0159},
		{0x0000, 0x0800, 0x0160},
		{0x0000, 0x0000, 0x0171},
		{0x8002, 0x0000, 0x0178},
		{0x0000, 0x0004, 0x0180},
		{0x0000, 0x0000, 0x0189},
		{0xc000, 0x0000, 0x0190},
		{0x0000, 0x0000, 0x0198},
		{0x0000, 0x081b, 0x01d8},
		{0x0005, 0x0000, 0x01d9},
		{0x0000, 0xc01b, 0x01e8},
		{0x0006, 0x0000, 0x01e9},
		{0x0000, 0xd41b, 0x01f0},
		{0x0004, 0x0000, 0x01f1},
		{0x0002, 0x0000, 0x0248},
		{0x0050, 0x0001, 0x0250},
		{0x0000, 0x0000, 0x0251},
		{0x4010, 0x0000, 0x0258},
		{0x0000, 0x0035, 0x02b0},
		{0x0001, 0x0000, 0x02b1},
		{0x0000, 0x005a, 0x02b8},
		{0x0005, 0x0000, 0x02b9},
		{0x0000, 0x481b, 0x02c0},
		{0x0006, 0x0000, 0x02c1},
		{0x0001, 0x0000, 0x0320},
		{0x1800, 0x0010, 0x0321},
		{0x0081, 0x0010, 0x0328},
		{0x0000, 0x0000, 0x0329},
		{0x0000, 0x0080, 0x0331},
		{0x777a, 0x001a, 0x0380},
		{0x0008, 0x0000, 0x0381},
		{0x4000, 0x401b, 0x0388},
		{0x0000, 0x0000, 0x0389},
		{0x8000, 0x0001, 0x03e9},
		{0x0004, 0x1000, 0x03f0},
		{0xc300, 0x0010, 0x03f8},
		{0x0000, 0x0000, 0x0400},
		{0x0000, 0x0000, 0x0401},
		{0x0000, 0x0000, 0x0409},
		{0x0000, 0x0017, 0x0450},
		{0x0001, 0x0000, 0x0451},
		{0x0000, 0xc1db, 0x0458},
		{0x0004, 0x0000, 0x0459},
		{0x0000, 0x581b, 0x0460},
		{0x0008, 0x0000, 0x0461},
		{0x0000, 0x0008, 0x04b8},
		{0x0010, 0x0000, 0x04c0},
		{0x0000, 0x0000, 0x04c9},
		{0x0000, 0x0010, 0x04d0},
		{0x0000, 0x0000, 0x04d9},
		{0x5000, 0x4000, 0x0518},
		{0x0006, 0x0000, 0x0519},
		{0x0000, 0x0000, 0x051a},
		{0x0000, 0x0000, 0x051b},
		{0xe800, 0x0004, 0x051c},
		{0x0000, 0x0000, 0x051d},
		{0x2000, 0x0000, 0x051e},
		{0x4000, 0x4000, 0x0520},
		{0x0006, 0x0000, 0x0521},
		{0x0000, 0x0000, 0x0522},
		{0x0000, 0x0000, 0x0523},
		{0x0800, 0x0000, 0x0524},
		{0x0000, 0x0000, 0x0525},
		{0x0000, 0x4000, 0x0528},
		{0x0006, 0x0000, 0x0529},
		{0x0000, 0x0000, 0x052a},
		{0x0000, 0x0000, 0x052b},
		{0x0800, 0x0000, 0x052c},
		{0x0000, 0x0000, 0x052d},
		{0x6000, 0x4000, 0x0530},
		{0x0006, 0x0000, 0x0531},
		{0x0000, 0x0000, 0x0532},
		{0x0000, 0x0000, 0x0533},
		{0xb800, 0x0004, 0x0534},
		{0x0000, 0x0000, 0x0535},
		{0x2000, 0x0000, 0x0536},
		{0x5000, 0x4000, 0x0538},
		{0x0006, 0x0000, 0x0539},
		{0x0000, 0x0000, 0x053a},
		{0x0000, 0x0000, 0x053b},
		{0x0800, 0x0000, 0x053c},
		{0x0000, 0x0000, 0x053d},
	};

	load_cfg((void*)cin, 0x30000, 792);
	load_data(din_addr[0], 0x2a000, 400, 0);
	load_data(din_addr[1], 0x20000, 400, 0);
	load_data(din_addr[2], 0x10000, 400, 0);
	load_data(din_addr[3], 0x8000, 400, 0);
	load_data(din_addr[4], 0x0, 400, 0);
	load_data(din_addr[5], 0x28000, 8, 0);
	config(0x0, 132);
	execute(0x7c05e4);
	store(dout_addr[0], 0x2c000, 400);
	store(dout_addr[1], 0x22000, 400);
	store(dout_addr[2], 0x12000, 400);
	store(dout_addr[3], 0xa000, 400);
	store(dout_addr[4], 0xc000, 400);
}
